#ifndef OCRC_Profiler_DOCXMLPARSER_CXX
#define OCRC_Profiler_DOCXMLPARSER_CXX OCRC_Profiler_DOCXMLPARSER_CXX

#include<dirent.h>
#include "./DocXMLReader.h"


namespace OCRCorrection {

    DocXMLReader::DocXMLReader() :
	selected_( false ),
	doc_( 0 ),
	globalProfile_( 0 ),
	verbose_( true )
    {

    }


    void DocXMLReader::setVerbose( bool b ) {
	verbose_ = b;
    }


    void DocXMLReader::parse( std::string const& xmlFile, Document* document, GlobalProfile* globalProfile ) {

	doc_ = document;
	globalProfile_ = globalProfile;

	xercesc::XMLPlatformUtils::Initialize();

	xercesc::SAXParser parser;

	parser.setDocumentHandler( this );
	parser.setErrorHandler( this );

	parser.parse( xmlFile.c_str() );


	//  xercesc::XMLPlatformUtils::Terminate();

    }


    // DEPRECATED!!!
    // void DocXMLReader::parseDir( std::string const& init_xmlDir, std::string const& imageDir, Document* doc ) {
    // 	std::string xmlDir( init_xmlDir );
    // 	if( xmlDir.at( 0 ) == '~' ) {
    // 	    xmlDir.replace( 0, 1, getenv( "HOME" ) );
    // 	}

    // 	std::wstring wide_xmlDir;
    // 	csl::CSLLocale::string2wstring( xmlDir, wide_xmlDir );


    // 	DIR *pDIR = opendir( xmlDir.c_str() );
    // 	struct dirent *pDirEnt;

    // 	std::vector< std::string > dirEntries;

    // 	/* Get each directory entry */
    // 	pDirEnt = readdir( pDIR );
    // 	while ( pDirEnt != NULL ) {
    // 	    dirEntries.push_back( pDirEnt->d_name );
    // 	    pDirEnt = readdir( pDIR );
    // 	}
    // 	closedir( pDIR );

    // 	// sort entries
    // 	std::sort( dirEntries.begin(), dirEntries.end() );

    // 	// process all entries
    // 	for( std::vector< std::string >::iterator entry = dirEntries.begin(); entry != dirEntries.end(); ++entry ) {
    // 	    size_t pos = entry->rfind( ".xml" );
    // 	    // only .xml files
    // 	    if( ( pos != std::string::npos ) && ( pos == (entry->size() - 4 ) )  ) {

    // 		std::string inFile = xmlDir + std::string( "/" ) +  *entry;

    // 		// create the image path
    // 		std::string imageFile = xmlDir + std::string( "/" ) +  entry->substr( 0, (entry->size() - 4 ) ) + std::string( ".tif" );

    // 		doc->newPage( imageFile );
    // 		parse( inFile.c_str(), doc );
    // 	    }
    // 	}

    // 	std::wcerr << "OCRC::DocXMLParser::parseDirToDocument: parsed directory " << wide_xmlDir << ", "
    // 		   << doc->getNrOfTokens() << " tokens, "
    // 		   << doc->getNrOfPages() << " pages."
    // 		   << std::endl;


    // }




// ---------------------------------------------------------------------------
//  Implementation of the SAX DocumentHandler interface
// ---------------------------------------------------------------------------

    void DocXMLReader::startElement( const XMLCh* const name,
				     AttributeList& attrs ) {

	xmlReaderHelper_.enter( name );

	// if( xmlReaderHelper_.getExternalHandler() ) {
	//     xmlReaderHelper_.getExternalHandler()->startElement( name, attrs );
	//     return;
	// }


	content_.clear();

	if( xmlReaderHelper_.name() == "document" ) {
	}

	else if( xmlReaderHelper_.name() == "globalProfile" ) {
	    xmlReaderHelper_.setExternalHandler( new GlobalProfileXMLReader( globalProfile_ ) );
	    xmlReaderHelper_.getExternalHandler()->startElement( name, attrs );
	}

	else if( xmlReaderHelper_.name() == "page" ) {
	    doc_->newPage()
		.setSourceFile( XMLReaderHelper::hasAttribute( attrs, "sourceFile" ) ?
				XMLReaderHelper::getAttributeValue( attrs, "sourceFile" ) : "" )
		.setImageFile ( XMLReaderHelper::getAttributeValue( attrs, "imageFile" ) )
		;

	    //if( verbose_ ) std::wcerr << "Page " << doc_->getNrOfPages() << std::endl;
	}

	else if( xmlReaderHelper_.name() == "token" ) {

	    tok_ = new Token( *doc_, doc_->getNrOfTokens(), true );
	    tok_->initGroundtruth();
	    tok_->getGroundtruth().setNormal( xmlReaderHelper_.getAttributeValue( attrs, "isNormal" ) == "true" );


//      std::wcout << __FILE__ << ":\"" <<  tok_->getWOCR() << "\"" << std::endl;
	}

	else if( xmlReaderHelper_.name() == "groundtruth" ) {
	    if( xmlReaderHelper_.hasAttribute( attrs, "verified" ) ) {
		std::string attrValue = xmlReaderHelper_.getAttributeValue( attrs, "verified" );
		if( attrValue == "true" ) {
		    tok_->getGroundtruth().setVerified( Token::VERIFIED_TRUE );
		}
		else if( attrValue == "false" ) {
		    tok_->getGroundtruth().setVerified( Token::VERIFIED_FALSE );
		}
		else if( attrValue == "guessed" ) {
		    tok_->getGroundtruth().setVerified( Token::VERIFIED_GUESSED );
		}
		else {
		    throw OCRCException( "OCRC::DocXMLParser: unknown value for attribute 'verified'" );
		}

	    }
	}

	else if( xmlReaderHelper_.name() == "coord" ) {
            tok_->setCoordinates(
                    Utils::toNum<float>(XMLReaderHelper::getAttributeValue(attrs, "l")),
                    Utils::toNum<float>(XMLReaderHelper::getAttributeValue(attrs, "t")),
                    Utils::toNum<float>(XMLReaderHelper::getAttributeValue(attrs, "r")),
                    Utils::toNum<float>(XMLReaderHelper::getAttributeValue(attrs, "b"))
                    );
	}
	else if( xmlReaderHelper_.name() == "abbyy_suspicious" ) {
	    if( xmlReaderHelper_.getAttributeValue( attrs, "value")  == "true"  ) {
		tok_->getAbbyySpecifics().setSuspicious( true );
	    }
	    else {
		tok_->getAbbyySpecifics().setSuspicious( false );
	    }
	}
    }



    void DocXMLReader::endElement(const XMLCh* const name) {

	xmlReaderHelper_.leave( name );

	// if( xmlReaderHelper_.getExternalHandler() ) {
	//     xmlReaderHelper_.getExternalHandler()->endElement( name );
	//     return;
	// }

	char *message = XMLString::transcode(name);

	if(strcmp(message, "document") == 0) {
	}

	else if(strcmp(message, "token") == 0) {

	    doc_->pushBackToken( tok_ );
	    //std::wcout << L"Parser:" << tok_->getWOCR() << "#" << tok_->getGroundtruth().getHistTrace() << std::endl;
	    tok_ = 0;

	} else if(strcmp(message, "ext_id") == 0) {
	  tok_->setExternalId( content_ );
	}

	else if(strcmp(message, "classified") == 0) {
	    tok_->getGroundtruth().setClassified( content_ );
	}

	else if(strcmp(message, "wOCR") == 0) {
            tok_->setWOCR( content_ );
	    // find out if wOCR is normal
	    if( tok_->getWOCR().empty() ) {
                tok_->setNormal( false );
	    }
	    else {
                tok_->setNormal( true );
                for( std::wstring::const_iterator c = tok_->getWOCR().begin(); c != tok_->getWOCR().end(); ++c ) {
                        if(!Document::isWord(*c)) {
                                tok_->setNormal( false );
                                break;
                        }
                }
	    }
	}

	/*
	 * Ignore candidates in docxml import
	 *
	else if( strcmp( message, "cand" ) == 0 ) {
	    Candidate cand;
	    if( cand.parseFromString( content_, 0 ) != content_.length() ) {
		throw OCRCException( "OCRC::DocXMLReader: Parsing of topCand failed" );
	    }
	    tok_->addCandidate( cand );
	}
	*/

	else if(strcmp(message, "wOCR_lc") == 0) {
                tok_->setWOCR_lc(content_);
	}

	else if(strcmp(message, "wOrig") == 0) {
	    tok_->getGroundtruth().setWOrig( content_ );
	}

	else if(strcmp(message, "wOrig_lc") == 0) {
	    // ignore this field
	}

	else if(strcmp(message, "wCorr") == 0 and not content_.empty()) {
		tok_->metadata()["correction"] = content_;
		tok_->metadata()["correction-lc"] = Utils::tolower(content_);
	}
	else if (strcmp(message, "wGT") == 0 and not content_.empty()) {
		tok_->metadata()["groundtruth"] = content_;
		tok_->metadata()["groundtruth-lc"] = Utils::tolower(content_);
	}

	else if(strcmp(message, "ocrInstructions") == 0) {
	}
	// the following two blocks refer to the new and old format of the xml
	// The first of both should be removed at some time
	else if(strcmp(message, "instruction") == 0) {
	    if( selected_ ) {
		tok_->getGroundtruth().setOCRTrace( content_ );
		selected_ = false;
	    }
	}
	else if(strcmp(message, "ocrTrace") == 0) {
	    tok_->getGroundtruth().setOCRTrace( content_ );
	}
	// these 2 blocks should at some point completely replace the "cand" block below
	else if(strcmp(message, "histTrace") == 0) {
	    tok_->getGroundtruth().setHistTrace( content_ );
	}
	else if(strcmp(message, "baseWord") == 0) {
	    tok_->getGroundtruth().setBaseWord( content_ );
	}

	else if(strcmp(message, "cands") == 0) {
	}

	else if(strcmp(message, "cand_old") == 0) {
	    if( selected_ ) {
		size_t colon = content_.find( L':', 0 );
		if( colon == std::wstring::npos ) {
		    throw OCRCException( "OCRC::DocXMLParser::endElement: Parse of hist instruction failed (colon not found)" );
		}
		size_t plus = content_.find( L'+', colon );
		if( plus == std::wstring::npos ) {
		    throw OCRCException( "OCRC::DocXMLParser::endElement: Parse of hist instruction failed (plus not found)" );
		}
		size_t left_bracket = content_.find( L'[', plus );
		if( left_bracket == std::wstring::npos ) {
		    throw OCRCException( "OCRC::DocXMLParser::endElement: Parse of hist instruction failed (left_bracket not found)" );
		}
		size_t right_bracket = content_.find( L']', left_bracket );
		if( right_bracket == std::wstring::npos ) {
		    throw OCRCException( "OCRC::DocXMLParser::endElement: Parse of hist instruction failed (right_bracket not found)" );
		}

		tok_->getGroundtruth().setBaseWord( content_.substr( colon +1, plus - colon - 1  ) ); // note that separators are not part of the catch
		tok_->getGroundtruth().setHistTrace( content_.substr( left_bracket, right_bracket - left_bracket + 1 ) );
		selected_ = false;
	    }
	}

	XMLString::release(&message);
    }

    void DocXMLReader::characters(const XMLCh* chars, XMLSize_t length) {
	char* eingabe_cstr = XMLString::transcode( chars );
	content_ += Utils::utf8(eingabe_cstr);
	XMLString::release( &eingabe_cstr );
    }

    void DocXMLReader::ignorableWhitespace(const XMLCh* const chars, const XMLSize_t length) {

    }

    void DocXMLReader::startDocument() {
	// std::wcout << __FILE__ <<  ": START DOCUMENT" << std::endl;
    }

    void DocXMLReader::endDocument() {
    }




// ---------------------------------------------------------------------------
//  Overrides of the SAX ErrorHandler interface
// ---------------------------------------------------------------------------
    void DocXMLReader::error(const SAXParseException& e) {
	char* message = XMLString::transcode(e.getMessage());
	std::string my_message = "OCRC::DocXMLParser: Xerces error: ";
	my_message += message;
	throw OCRCException( my_message );

	std::wstring wideMessage = Utils::utf8(message);
	//csl::CSLLocale::string2wstring( std::string( message), wideMessage );
	std::wcerr << wideMessage << std::endl;
	XMLString::release(&message);
    }

    void DocXMLReader::fatalError(const SAXParseException& e) {
	char* message = XMLString::transcode(e.getMessage());

	std::string my_message = "OCRC::DocXMLParser: Xerces error: ";
	my_message += message;

	throw OCRCException( my_message );

	std::wstring wideMessage = Utils::utf8(message);
	//csl::CSLLocale::string2wstring( std::string( message), wideMessage );
	std::wcerr << wideMessage << std::endl;

	XMLString::release(&message);
    }

    void DocXMLReader::warning(const SAXParseException& e) {
	char* message = XMLString::transcode(e.getMessage());
	std::string my_message = "OCRC::DocXMLParser: Xerces error: ";
	my_message += message;

	throw OCRCException( my_message );

	std::wstring wideMessage = Utils::utf8(message);
	//csl::CSLLocale::string2wstring( std::string( message), wideMessage );
	std::wcerr << wideMessage << std::endl;
	XMLString::release(&message);
    }

} // eon
#endif
