#ifndef OCRCORRECT_ABBYY_XML_PARSER_CXX
#define OCRCORRECT_ABBYY_XML_PARSER_CXX OCRCORRECT_ABBYY_XML_PARSER_CXX

#include "./AbbyyXmlParser.h"

namespace OCRCorrection {


    AbbyyXmlParser::AbbyyXmlParser() :
	left_ ( 0 ),
	top_ ( 0 ),
	right_ ( 0 ) ,
	bottom_ ( 0 ),
	inVariant_( false ),
	suspicious_ ( false ),
	lastchar_ ( 0 ),
	temptoken_( 0 )

    {
    }


    AbbyyXmlParser::~AbbyyXmlParser() {

    }

    /**
     * @brief parses abbyy output of a single page.
     */
    void AbbyyXmlParser::parsePageToDocument( std::string const& init_filename, std::string const& init_imageDir, OCRCorrection::Document* doc ) {
 	std::string imageDir( init_imageDir );
	if( imageDir.at( 0 ) == '~' ) {
	    imageDir.replace( 0, 1, getenv( "HOME" ) );
	}

	std::string filename( init_filename );
	if( filename.at( 0 ) == '~' ) {
	    filename.replace( 0, 1, getenv( "HOME" ) );
	}

	std::string fileDir;
	std::string fileBasename;
	size_t pos = filename.rfind( "/" );
	if( pos == std::string::npos ) {
	    fileDir = "";
	    fileBasename = filename;
	}
	else {
	    fileDir = filename.substr( 0, pos );
	    fileBasename = filename.substr( pos + 1 );
	}

	pos = fileBasename.rfind( ".xml" );
	// only .xml files
	if( ( pos != std::string::npos ) && ( pos == (fileBasename.size() - 4 ) )  ) {

	    // std::string inFile = filename;

	    // create the image path
	    std::string imageFile = imageDir + std::string( "/" ) +  fileBasename.substr( 0, (fileBasename.size() - 4 ) ) + std::string( ".tif" );

	    doc->newPage()
		.setImageFile( imageFile );

	    parseToDocument( filename.c_str(), doc );
	}
	else { // no "xml" extension
	    std::string message = "OCRC::AbbyyXmlParser::parsePageToDocument: not an xml file: ";
	    message += filename;
	    throw OCRCException( message );
	}
    }



    void AbbyyXmlParser::parseDirToDocument( std::string const& init_xmlDir, std::string const& init_imageDir, OCRCorrection::Document* doc ) {

	std::string xmlDir( init_xmlDir );
	if( xmlDir.empty() ) {
	    throw OCRCException( "OCRC::AbbyyXmlParser::parseDirToDocument: xmlDir is empty string." );
	}
	if( xmlDir.at( 0 ) == '~' ) {
	    xmlDir.replace( 0, 1, getenv( "HOME" ) );
	}

	std::string imageDir( init_imageDir );
	if( imageDir.empty() ) {
	    throw OCRCException( "OCRC::AbbyyXmlParser::parseDirToDocument: imageDir is empty string." );
	}
	if( imageDir.at( 0 ) == '~' ) {
	    imageDir.replace( 0, 1, getenv( "HOME" ) );
	}

	std::wstring wideXmlDir = Utils::utf8(xmlDir);
    std::wstring wideImageDir = Utils::utf8(imageDir);
	// csl::CSLLocale::CSLLocale::string2wstring( xmlDir, wideXmlDir );
	// csl::CSLLocale::CSLLocale::string2wstring( imageDir, wideImageDir );
	std::wcerr << "OCRC::AbbyyXmlParser::parseDirToDocument: Parse " << wideXmlDir << ", images in " << wideImageDir << std::endl;

	DIR *pDIR = opendir( xmlDir.c_str() );

	if( ! pDIR ) {
	    if( errno == ENOENT ) {
		throw OCRCException( "OCRC::AbbyyXmlParser::parseDirToDocument: No such directory." );
	    }
	    else {
		throw OCRCException( "OCRC::AbbyyXmlParser::parseDirToDocument: access to directory failed." );
	    }

	}

	std::wstring wide_xmlDir = Utils::utf8(xmlDir);
	//csl::CSLLocale::string2wstring( xmlDir, wide_xmlDir );


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
	    if( *entry == "." || *entry == ".." ) {
		continue;
	    }
	    size_t pos = entry->rfind( ".xml" );
	    // only .xml files
	    if( ( pos != std::string::npos ) && ( pos == (entry->size() - 4 ) )  ) {

		std::string inFile = xmlDir + std::string( "/" ) +  *entry;

		// create the image path
		std::string imageFile = imageDir + std::string( "/" ) +  entry->substr( 0, (entry->size() - 4 ) ) + std::string( ".tif" );

		doc->newPage( imageFile );
		parseToDocument( inFile.c_str(), doc );
	    }
	    else { // no "xml" extension
                std::wstring wideEntry = Utils::utf8(*entry);
                //csl::CSLLocale::string2wstring( *entry, wideEntry );
		std::wcerr << "OCRC::AbbyyXmlParser::parseDirToDocument: Ignored non-xml file " << wideEntry << std::endl;
	    }
	}

	std::wcerr << "OCRC::AbbyyXmlParser::parseDirToDocument: parsed directory " << wide_xmlDir << ", "
		   << doc->getNrOfTokens() << " tokens, "
		   << doc->getNrOfPages() << " pages."
		   << std::endl;


    }

    void AbbyyXmlParser::parseToDocument( char const* filename, OCRCorrection::Document* doc )  {
	currentFileName_ = filename;
	doc_ = doc;

	xercesc::XMLPlatformUtils::Initialize();
	xercesc::SAXParser parser_;

	parser_.setDocumentHandler( this );
	parser_.setErrorHandler( this );
	parser_.parse( filename );

	//xercesc::XMLPlatformUtils::Terminate();

	std::wcerr << "OCRC::AbbyyXmlParser::parseToDocument: parsed file: "
		   << doc->getNrOfTokens() << " tokens, "
		   << doc->getNrOfPages() << " pages."
		   << std::endl;

    }




// ---------------------------------------------------------------------------
//  Implementation of the SAX DocumentHandler interface
// ---------------------------------------------------------------------------

// startende Tags, tagname in anführungszeichen
    void AbbyyXmlParser::startElement(const XMLCh* const name
				      , AttributeList& attrs) {

	char* message = XMLString::transcode(name);

	if(strcmp(message, "document") == 0) {
	}

	else if(strcmp(message, "page") == 0) {
	}

	else if(strcmp(message, "block") == 0) {
	}

	else if(strcmp(message, "region") == 0) {
	}

	else if(strcmp(message, "rect") == 0) {
	}

	else if(strcmp(message, "text") == 0) {
	}

	else if(strcmp(message, "par") == 0) {
	}

	else if(strcmp(message, "line") == 0) {

	  XMLSize_t len = attrs.getLength();

	  for (XMLSize_t index = 0; index < len; index++) {

	    if(strcmp(XMLString::transcode(attrs.getName(index)), "t") == 0) {
	      top_ = XMLString::parseInt(attrs.getValue(index));
	    }

	    if(strcmp(XMLString::transcode(attrs.getName(index)), "b") == 0) {
	      bottom_ = XMLString::parseInt(attrs.getValue(index));
	    }

	  }

	}
	else if(strcmp(message, "variantText") == 0) {
	    inVariant_ = true;
	}

	else if(strcmp(message, "formatting") == 0) {
	}

	else if(strcmp(message, "charParams") == 0) {

	    XMLSize_t len = attrs.getLength();

	    for (XMLSize_t index = 0; index < len; index++) {

		if(strcmp(XMLString::transcode(attrs.getName(index)), "l") == 0) {
		    left_ = XMLString::parseInt(attrs.getValue(index));
		    if( left_ < 0 ) left_ = 0; //throw OCRCException( "OCRC::AbbyyXmlParser: charParam-tag with coordinate 'left' smaller than 0" );
		}

		if(strcmp(XMLString::transcode(attrs.getName(index)), "t") == 0) {
		  // commented out because line coordinates are used for bottom and top to ensure aligned tokens
		  //top_ = XMLString::parseInt(attrs.getValue(index));
		}

		if(strcmp(XMLString::transcode(attrs.getName(index)), "r") == 0) {
		    right_ = XMLString::parseInt(attrs.getValue(index));
		    if( right_ < 0 ) right_ = 0; //throw OCRCException( "OCRC::AbbyyXmlParser: charParam-tag with coordinate 'right' smaller than 0" );
		}

		if(strcmp(XMLString::transcode(attrs.getName(index)), "b") == 0) {
		  // commented out because line coordinates are used for bottom and top to ensure aligned tokens
		  //bottom_ = XMLString::parseInt(attrs.getValue(index));
		}

		if(strcmp(XMLString::transcode(attrs.getName(index)), "suspicious") == 0) {
		    suspicious_ = true;
		}
	    }
	}

	XMLString::release(&message);
    }

// schliessende Tags, Tagname in Anführungszeichen
    void AbbyyXmlParser::endElement(const XMLCh* const name) {

	char *message = XMLString::transcode(name);

	if(strcmp(message, "document") == 0) {
	}
	else if(strcmp(message, "page") == 0) {
	}
	else if(strcmp(message, "block") == 0) {
	}
	else if(strcmp(message, "region") == 0) {
	}
	else if(strcmp(message, "rect") == 0) {
	}
	else if(strcmp(message, "variantText") == 0) {
	    inVariant_ = false;
	}
	else if(strcmp(message, "text") == 0) {
	}
	// adding additional linebreak at end of paragraph
	else if(strcmp(message, "par") == 0) {

	    wchar_t t = L'\n';
	    OCRCorrection::Character* tempchar = new OCRCorrection::Character(t, 0, 0, 0, 0);
	    temptoken_ = new OCRCorrection::Token(*doc_, doc_->getNrOfTokens(), false );

	    try {
	      temptoken_->addCharacter( *tempchar );
	    } catch ( OCRCorrection::OCRCException ) {
		std::wcerr << "Added \\0 char:" << __FILE__ << __LINE__ << std::endl;
	    }


	    sendTokenToDocument( temptoken_ );
	    temptoken_ = 0;
	    //std::wcout<<"Pushback 1 <"<<temptoken_->getWOCR()<<">"<<std::endl;
	    lastchar_ = 0;


	}
	// at end of line, pushback actual token and add newline token
	else if(strcmp(message, "line") == 0) {

	  if( temptoken_ != 0) sendTokenToDocument( temptoken_ );
	  temptoken_ = 0;
	  //std::wcout<<"Pushback 2 <"<<temptoken_->getWOCR()<<">"<<std::endl;
	  wchar_t t = L'\n';
	  OCRCorrection::Character* tempchar = new OCRCorrection::Character(t, 0, 0, 0, 0);
	  temptoken_ = new OCRCorrection::Token(*doc_, doc_->getNrOfTokens(), false );

	  try {
	    temptoken_->addCharacter( *tempchar );
	  } catch ( OCRCorrection::OCRCException ) {
	    std::wcerr << "Added \\0 char:" << __FILE__ << __LINE__ << std::endl;
	  }


	  sendTokenToDocument( temptoken_ );
	  temptoken_ = 0;
	  //std::wcout<<"Pushback 3 <"<<temptoken_->getWOCR()<<">"<<std::endl;

	  lastchar_ = 0;

	}

	else if(strcmp(message, "formatting") == 0) {
	}

	else if(strcmp(message, "charParams") == 0) {

	    // quick and dirty for ignoring abbyy feature of variants
	    if(!inVariant_) {
		OCRCorrection::Character* tempchar = new OCRCorrection::Character( tempchar_, left_, top_, right_, bottom_, suspicious_ );


		// lastchar_ not set, open new token (happens at begin of document and after closing </line> and </par> tags)
		if(!lastchar_) {
		    temptoken_ = new OCRCorrection::Token( *doc_, doc_->getNrOfTokens(), false );

		    try {
			temptoken_->addCharacter( *tempchar );
		    } catch ( OCRCorrection::OCRCException ) {
			std::wcerr << "Added \\0 char:" << __FILE__ << __LINE__ << std::endl;
		    }


		    lastchar_ = tempchar_;
		}
		else {
		    // previous char alnum and actual char alnum -> attach tempchar_ to token
		    if( (Document::isWord( lastchar_ )) && (Document::isWord( tempchar_ ))) {

			try {
			    temptoken_->addCharacter( *tempchar );
			} catch ( OCRCorrection::OCRCException ) {
			    std::wcerr << "Added \\0 char:" << __FILE__ << __LINE__ << std::endl;
			}

		    }
		    // previous char non-alnum and actual char alnum -> pushback token, open new one, attach tempchar_
		    else if( (!Document::isWord( lastchar_ )) && (Document::isWord( tempchar_ ))) {

			sendTokenToDocument( temptoken_ );
			temptoken_ = 0;
			//std::wcout<<"Pushback 4 <"<<temptoken_->getWOCR()<<">"<<std::endl;
			temptoken_ = new OCRCorrection::Token( *doc_, doc_->getNrOfTokens(), false );

			try {
			    temptoken_->addCharacter( *tempchar );
			} catch ( OCRCorrection::OCRCException ) {
			    std::wcerr << "Added \\0 char:" << __FILE__ << __LINE__ << std::endl;
			}

		    }
		    // previous char alnum and actual char non-alnum -> pushback token, open new one, attach tempchar_
		    else if( (Document::isWord( lastchar_ )) && (!Document::isWord( tempchar_ ))) {

			sendTokenToDocument( temptoken_ );
			temptoken_ = 0;
			//std::wcout<<"Pushback 5 <"<<temptoken_->getWOCR()<<">"<<std::endl;
			temptoken_ = new OCRCorrection::Token( *doc_, doc_->getNrOfTokens(), false );

			try {
			    temptoken_->addCharacter( *tempchar );
			} catch ( OCRCorrection::OCRCException ) {
			    std::wcerr << "Added \\0 char:" << __FILE__ << __LINE__ << std::endl;
			}

		    }
		    // previous char non-alnum and actual char non-alnum -> attach tempchar_ to token
		    else if( (!Document::isWord( lastchar_ )) && (!Document::isWord( tempchar_ ))) {

			try {
			    temptoken_->addCharacter( *tempchar );
			} catch ( OCRCorrection::OCRCException ) {
			    std::wcerr << "Added \\0 char:" << __FILE__ << __LINE__ << std::endl;
			}
		    }
		}

		suspicious_ = false;
		lastchar_ = tempchar_;
	    }
	}

	XMLString::release(&message);
    }

// Text gefunden, im Parser wird nur Text behandelt der zwischen <charParam> und </charParam> steht
// (die Verarbeitung von tempchar_ findet im closing tag von charParam statt, ergo werden nur Buchstaben
// verarbeitet die zwischen <charParam> und </charParam> stehen
    void AbbyyXmlParser::characters(const XMLCh* const chars, const XMLSize_t length) {


        char* input = XMLString::transcode(chars);

	//	fwprintf( stderr,L"-->%c\n", chars[0] );

	int offset = 0;
	int utf8offset = 0;

	std::wstring wideInput = Utils::utf8(input);
	//csl::CSLLocale::string2wstring( input, wideInput );
	// 	int test = GetUtf8( eingabe, offset, utf8offset );
// 	tempchar_ = test;
	if( wideInput.length() != 1 ) {
	    std::wcerr << "OCRC::AbbyyXMLParser::characters: WARNING: Assumption that each string has one character does not hold: '" << wideInput << "'" << std::endl;
	}
	tempchar_ = wideInput.at( 0 );
	//std::wcout<<"<"<<tempchar_<<">"<<std::endl;
    }

    void AbbyyXmlParser::ignorableWhitespace(const XMLCh* const chars, const XMLSize_t length) {

	char* characters = XMLString::transcode(chars);
	//std::wcout << "Hab das gesehen: " << characters << std::endl;

    }

    void AbbyyXmlParser::startDocument() {
	//std::wcout << "Beginne Dokument" << std::endl;
    }

    void AbbyyXmlParser::endDocument() {

	//std::wcout << "Dokument zuende" << std::endl;
	// print document (for testing only)
	//doc_->print( std::wcout );
    }


// ---------------------------------------------------------------------------
//  Overrides of the SAX ErrorHandler interface
// ---------------------------------------------------------------------------
    void AbbyyXmlParser::error(const SAXParseException& e) {
	char* message = XMLString::transcode(e.getMessage());
	throw OCRCorrection::OCRCException( "AbbyyXMLParser:: Error" );
	//cout << message << endl;
	XMLString::release(&message);
    }

    void AbbyyXmlParser::fatalError(const SAXParseException& e) {
	char* message_c_str = XMLString::transcode(e.getMessage());
	std::string message =
	    std::string( "OCRC::AbbyyXMLParser:: Fatal Error: " )
	    + std::string( message_c_str )
	    + ", xmlFile=" + currentFileName_;

	throw OCRCorrection::OCRCException( message.c_str() );
	//cout << message << endl;
	XMLString::release( &message_c_str );
    }

    void AbbyyXmlParser::warning(const SAXParseException& e) {
	char* message = XMLString::transcode(e.getMessage());
	//cout << message << endl;
	XMLString::release(&message);
    }


    void AbbyyXmlParser::sendTokenToDocument( Token* temptoken ) {
	// check for normal-ness
	temptoken_->setNormal( true );
	for( std::wstring::const_iterator c = temptoken_->getWOCR().begin(); c != temptoken_->getWOCR().end(); ++c ) {
                //if( !std::isalnum( *c, csl::CSLLocale::Instance() ) ) {
                if (!Document::isWord(*c)) {
                        temptoken_->setNormal( false );
                        break;
                }
	}

	doc_->pushBackToken(temptoken_);
    }

} // eon

#endif
