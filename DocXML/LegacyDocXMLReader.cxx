#ifndef OCRC_LEGACY_DOCXMLREADER_CXX
#define OCRC_LEGACY_DOCXMLREADER_CXX OCRC_LEGACY_DOCXMLREADER_CXX

#include<dirent.h>
#include "./LegacyDocXMLReader.h"


namespace OCRCorrection {

    LegacyDocXMLReader::LegacyDocXMLReader() :
	selected_( false ),
	doc_( 0 ) 
    {
    }

    void LegacyDocXMLReader::parse( std::string const& xmlFile, Document* document ) {

	doc_ = document;

	doc_->newPage();

	xercesc::XMLPlatformUtils::Initialize();
    
	xercesc::SAXParser parser_;
    
	parser_.setDocumentHandler( this );
	parser_.setErrorHandler( this );

	parser_.parse( xmlFile.c_str() );
    
    
	//  xercesc::XMLPlatformUtils::Terminate();
    
    }


    void LegacyDocXMLReader::parseDir( std::string const& init_xmlDir, std::string const& imageDir, Document* doc ) {
	std::string xmlDir( init_xmlDir );
	if( xmlDir.at( 0 ) == '~' ) {
	    xmlDir.replace( 0, 1, getenv( "HOME" ) );
	}
	
	std::wstring wide_xmlDir;
	csl::CSLLocale::string2wstring( xmlDir, wide_xmlDir );


	DIR *pDIR = opendir( xmlDir.c_str() );
	struct dirent *pDirEnt;
	
	std::vector< std::string > dirEntries;
	
	/* Get each directory entry */
	pDirEnt = readdir( pDIR );
	while ( pDirEnt != NULL ) {
	    dirEntries.push_back( pDirEnt->d_name );
	    pDirEnt = readdir( pDIR );
	}
	closedir( pDIR );

	// sort entries
	std::sort( dirEntries.begin(), dirEntries.end() );

	// process all entries
	for( std::vector< std::string >::iterator entry = dirEntries.begin(); entry != dirEntries.end(); ++entry ) {
	    size_t pos = entry->rfind( ".xml" );
	    // only .xml files
	    if( ( pos != std::string::npos ) && ( pos == (entry->size() - 4 ) )  ) {

		std::string inFile = xmlDir + std::string( "/" ) +  *entry;

		// create the image path
		std::string imageFile = xmlDir + std::string( "/" ) +  entry->substr( 0, (entry->size() - 4 ) ) + std::string( ".tif" );
		
		//doc->newPage( imageFile );
		parse( inFile.c_str(), doc );
	    }
	}

	std::wcerr << "OCRC::DocXMLParser::parseDirToDocument: parsed directory " << wide_xmlDir << ", " 
		   << doc->getNrOfTokens() << " tokens, " 
		   << doc->getNrOfPages() << " pages."
		   << std::endl;
        
    
    }




// ---------------------------------------------------------------------------
//  Implementation of the SAX DocumentHandler interface
// ---------------------------------------------------------------------------

// startende Tags, tagname in anführungszeichen
    void LegacyDocXMLReader::startElement(const XMLCh* const name
				    , AttributeList& attrs) {

  
	char* message = XMLString::transcode(name);
	
	content_.clear();
	
	if(strcmp(message, "document") == 0) {
	}
	
	else if(strcmp(message, "page") == 0) {
	    
	    XMLCh const* attrValue = attrs.getValue( "imageFile" );
	    char* attrValue_cstr = XMLString::transcode( attrValue );
	    doc_->newPage( attrValue_cstr );
	    XMLString::release( &attrValue_cstr );
	    std::wcerr << "Page " << doc_->getNrOfPages() << std::endl;
	}
      
	else if( strcmp( message, "token" ) == 0 ) {
      
	    tok_ = new Token(*doc_, doc_->getNrOfTokens(), true);
	    tok_->initGroundtruth();

	    XMLCh const* attrValue = attrs.getValue( "isNormal" );
	    char* attrValue_cstr = XMLString::transcode( attrValue );
	    tok_->getGroundtruth().setNormal( strcmp( attrValue_cstr, "true" ) == 0 );
	    XMLString::release( &attrValue_cstr );
      
	    attrValue = attrs.getValue( "verified" );
	    attrValue_cstr = XMLString::transcode( attrValue );
	    if( strcmp( attrValue_cstr, "true" ) == 0 ) {
		tok_->getGroundtruth().setVerified( Token::VERIFIED_TRUE );
	    }
	    else {
		tok_->getGroundtruth().setVerified( Token::VERIFIED_FALSE );
	    }
	    XMLString::release( &attrValue_cstr );
      
      
//      std::wcout << __FILE__ << ":\"" <<  tok_->getWOCR() << "\"" << std::endl;
	}

	else if(strcmp(message, "groundtruth") == 0) {
	    XMLCh const* attrValue = attrs.getValue( "verified" );
	    if( attrValue ) {
		char* attrValue_cstr = XMLString::transcode( attrValue );
		if( ! strcmp( attrValue_cstr, "true" ) ) {
		    tok_->getGroundtruth().setVerified( Token::VERIFIED_TRUE );
		}
		else if( ! strcmp( attrValue_cstr, "false" ) ) {
		    tok_->getGroundtruth().setVerified( Token::VERIFIED_FALSE );
		}
		else if( ! strcmp( attrValue_cstr, "guessed" ) ) {
		    tok_->getGroundtruth().setVerified( Token::VERIFIED_GUESSED );
		}
		else {
		    throw OCRCException( "OCRC::DocXMLParser: unknown value for attribute 'verified'" );
		}
		
		XMLString::release( &attrValue_cstr );
	    }
	}
	else if(strcmp(message, "classified") == 0) {
	}

	else if(strcmp(message, "wOCR") == 0) {
	}

	else if(strcmp(message, "wOCR_lc") == 0) {
	}

	else if(strcmp(message, "wOrig") == 0) {
	}

	else if(strcmp(message, "wOrig_lc") == 0) {
	}

	else if(strcmp(message, "topCand") == 0) {
	}

	else if(strcmp(message, "ocrInstructions") == 0) {
	}
  
	else if(strcmp(message, "instruction") == 0) {
    
	    XMLSize_t len = attrs.getLength();
	    for (XMLSize_t index = 0; index < len; index++) {
      
		selected_ = false;
		if(strcmp(XMLString::transcode(attrs.getName(index)), "selected") == 0) {
		    if(strcmp(XMLString::transcode(attrs.getValue(index)), "true") == 0) {
			selected_ = true;
		    }
		}   
	    }
	}
    
	else if(strcmp(message, "cands") == 0) {
	}

	else if(strcmp(message, "cand") == 0) {

	    XMLSize_t len = attrs.getLength();
    
	    selected_ = false;
	    for (XMLSize_t index = 0; index < len; index++) {
      
		if(strcmp(XMLString::transcode(attrs.getName(index)), "selected") == 0) {
		    if(strcmp(XMLString::transcode(attrs.getValue(index)), "true") == 0) {
			selected_ = true;
		    }
		}      
	    }
	}

	else if( strcmp( message, "coord" ) == 0 ) {
	    tok_->setCoordinates( 
		XMLString::parseInt(attrs.getValue( "l"  ) ),
		XMLString::parseInt(attrs.getValue( "t"  ) ),
		XMLString::parseInt(attrs.getValue( "r"  ) ),
		XMLString::parseInt(attrs.getValue( "b"  ) )
		);
	}
	else if( strcmp( message, "abbyy_suspicious" ) == 0 ) {
	    if( strcmp( XMLString::transcode(attrs.getValue( "value" ) ), "true") == 0 ) {
		tok_->getAbbyySpecifics().setSuspicious( true );
	    }
	    else {
		tok_->getAbbyySpecifics().setSuspicious( false );
	    }
	}

	XMLString::release(&message);  
    }



// schliessende Tags, Tagname in Anführungszeichen
    void LegacyDocXMLReader::endElement(const XMLCh* const name) {

	char *message = XMLString::transcode(name);

	if(strcmp(message, "document") == 0) {
	}
  
	else if(strcmp(message, "token") == 0) {
      
	    doc_->pushBackToken( tok_ );
	    //std::wcout << L"Parser:" << tok_->getWOCR() << "#" << tok_->getGroundtruth().getHistTrace() << std::endl;
	    tok_ = 0;
      
	}
  
	else if(strcmp(message, "classified") == 0) {
	    tok_->getGroundtruth().setClassified( content_ );
	}

	else if
	    (
		( strcmp(message, "wOCR") == 0 ) ||
		( strcmp(message, "string") == 0 )   // "string" is the older variant for this tag.
		) {
	    tok_->setWOCR( content_ );

	    // find out if wOCR is normal
	    if( tok_->getWOCR().empty() ) {
		tok_->setNormal( false );
	    }
	    else {
		tok_->setNormal( true );
		for( std::wstring::const_iterator c = tok_->getWOCR().begin(); c != tok_->getWOCR().end(); ++c ) {
		    if( ! std::isalnum( *c, csl::CSLLocale::Instance() ) ) {
			tok_->setNormal( false );
			break;
		    }
		}
	    }


	}

	// else if( strcmp( message, "cand" ) == 0 ) {
	//     Candidate cand;
	//     //std::wcout << "content_=" << content_ << ", os=" << cand.parseFromString( content_, 0 ) << std::endl;
	//     if( cand.parseFromString( content_, 0 ) != content_.length() ) {
	// 	throw OCRCException( "OCRC::LegacyDocXMLReader: Parsing of topCand failed" );
	//     }
	//     tok_->addCandidate( cand );
	// }

	else if(strcmp(message, "wOCR_lc") == 0) {
	    // ignore this field
	}

	else if(strcmp(message, "wOrig") == 0) {
	    tok_->getGroundtruth().setWOrig( content_ );
	}

	else if(strcmp(message, "wOrig_lc") == 0) {
	    // ignore this field
	}

	else if(strcmp(message, "wCorr") == 0) {
	    tok_->setWCorr( content_ );
	}

	else if(strcmp(message, "ocrInstructions") == 0) {
	}
	// the following two blocks refer to the new and old format of the xml
	// The first of both should be removed at some time
	else if(strcmp(message, "instruction") == 0) {
	    if( selected_ ) {
                if( !content_.empty() && content_.at(0) == '[' ) { // if regular trace, not SPLIT_MERGE or similar
                    size_t pos = 0;
                    while( ( pos = content_.find( '_', pos ) ) != std::string::npos ) {
                        content_.at( pos ) = ':';
                    }
                }
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
    
	else if(strcmp(message, "cand") == 0) {
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
		
                std::wstring histTrace = content_.substr( left_bracket, right_bracket - left_bracket + 1 );
                
                size_t pos = 0;        
                while( ( pos = histTrace.find( '_', pos ) ) != std::string::npos ) {
                    histTrace.at( pos ) = ':';
                }

                tok_->getGroundtruth().setHistTrace( histTrace );
		selected_ = false;
	    }
	}

	XMLString::release(&message);
    }

    void LegacyDocXMLReader::characters(const XMLCh* chars, XMLSize_t length) {

	char* eingabe_cstr = XMLString::transcode( chars );
	static std::string eingabe;
	static std::wstring eingabe_wide;

	eingabe = eingabe_cstr;
    
	static std::locale loc( CSL_UTF8_LOCALE );
	csl::CSLLocale::string2wstring( eingabe, eingabe_wide );


	content_ += eingabe_wide;
    
	XMLString::release( &eingabe_cstr ); 
    }

    void LegacyDocXMLReader::ignorableWhitespace(const XMLCh* const chars, const XMLSize_t length) {
    
    }

    void LegacyDocXMLReader::startDocument() {
	// std::wcout << __FILE__ <<  ": START DOCUMENT" << std::endl;
    }

    void LegacyDocXMLReader::endDocument() {
    }




// ---------------------------------------------------------------------------
//  Overrides of the SAX ErrorHandler interface
// ---------------------------------------------------------------------------
    void LegacyDocXMLReader::error(const SAXParseException& e) {
	char* message = XMLString::transcode(e.getMessage());
	std::string my_message = "OCRC::DocXMLParser: Xerces error: ";
	my_message += message;
	throw OCRCException( my_message );

	std::wstring wideMessage;
	csl::CSLLocale::string2wstring( std::string( message), wideMessage );
	std::wcerr << wideMessage << std::endl;
	XMLString::release(&message);
    }

    void LegacyDocXMLReader::fatalError(const SAXParseException& e) {
	char* message = XMLString::transcode(e.getMessage());

	std::string my_message = "OCRC::DocXMLParser: Xerces error: ";
	my_message += message;

	throw OCRCException( my_message );

	std::wstring wideMessage;
	csl::CSLLocale::string2wstring( std::string( message), wideMessage );
	std::wcerr << wideMessage << std::endl;

	XMLString::release(&message);
    }

    void LegacyDocXMLReader::warning(const SAXParseException& e) {
	char* message = XMLString::transcode(e.getMessage());
	std::string my_message = "OCRC::DocXMLParser: Xerces error: ";
	my_message += message;

	throw OCRCException( my_message );

	std::wstring wideMessage;
	csl::CSLLocale::string2wstring( std::string( message), wideMessage );
	std::wcerr << wideMessage << std::endl;
	XMLString::release(&message);
    }

} // eon
#endif
