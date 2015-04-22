#ifndef OCRC_Profiler_DOCXMLPARSER_CXX
#define OCRC_Profiler_DOCXMLPARSER_CXX OCRC_Profiler_DOCXMLPARSER_CXX

#include<dirent.h>
#include "./AltoXMLReader.h"


namespace OCRCorrection {

    AltoXMLReader::AltoXMLReader() :
	selected_( false ),
	doc_( 0 )
    {
	xercesc::XMLPlatformUtils::Initialize();
    }

    AltoXMLReader::~AltoXMLReader()  {
	xercesc::XMLPlatformUtils::Terminate();
    }


    void AltoXMLReader::parse( std::string const& xmlFile, Document* document ) {

	doc_ = document;

    
	xercesc::SAXParser parser;
    
	parser.setDocumentHandler( this );
	parser.setErrorHandler( this );

	parser.parse( xmlFile.c_str() );
    
    
	//xercesc::XMLPlatformUtils::Terminate();
    
    }

    void AltoXMLReader::parseDir( std::string const& init_xmlDir, std::string const& init_imageDir, OCRCorrection::Document* doc ) {

	if( init_xmlDir.empty() ) {
	    throw OCRCException( "OCRC::AltoXMLReader::parseDirToDocument: xmlDir is empty string." );
	}
	if( init_imageDir.empty() ) {
	    throw OCRCException( "OCRC::AltoXMLReader::parseDirToDocument: imageDir is empty string." );
	} 
	
	std::string xmlDir = Utils::normalizeFilePath( init_xmlDir );
	
	std::string imageDir = Utils::normalizeFilePath( init_imageDir );
	
	std::wstring wideXmlDir, wideImageDir;
	csl::CSLLocale::string2wstring( xmlDir, wideXmlDir );
	csl::CSLLocale::string2wstring( imageDir, wideImageDir );
	std::wcerr << "OCRC::AltoXMLReader::parseDirToDocument: Parse " << wideXmlDir << ", images in " << wideImageDir << std::endl;
	
	DIR *pDIR = opendir( xmlDir.c_str() );
	
	if( ! pDIR ) {
	    if( errno == ENOENT ) {
		throw OCRCException( "OCRC::AltoXMLReader::parseDirToDocument: No such directory." );
	    }
	    else {
		throw OCRCException( "OCRC::AltoXMLReader::parseDirToDocument: access to directory failed." );
	    }
	}
	
	std::wstring wide_xmlDir;
	csl::CSLLocale::string2wstring( xmlDir, wide_xmlDir );


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

	    // only .xml files
	    if( entry->compare( entry->size() - 4, 4, ".xml" ) == 0 ) {

		std::string inFile = xmlDir + std::string( "/" ) +  *entry;

		// create the image path
		std::string imageFile = imageDir + std::string( "/" ) +  entry->substr( 0, (entry->size() - 4 ) ) + std::string( ".tif" );
	    
		doc->newPage()
		    .setSourceFile( inFile )
		    .setImageFile( imageFile );

		parse( inFile.c_str(), doc );
	    }
	    else { // no "xml" extension
		std::wstring wideEntry;
		csl::CSLLocale::string2wstring( *entry, wideEntry );
		std::wcerr << "OCRC::AltoXMLReader::parseDirToDocument: Ignored non-xml file " << wideEntry << std::endl;
	    }
	}

	std::wcerr << "OCRC::AltoXMLReader::parseDirToDocument: parsed directory " << wide_xmlDir << ", " 
		   << doc->getNrOfTokens() << " tokens, " 
		   << doc->getNrOfPages() << " pages."
		   << std::endl;
        
    
    }



// ---------------------------------------------------------------------------
//  Implementation of the SAX DocumentHandler interface
// ---------------------------------------------------------------------------

    void AltoXMLReader::startElement( const XMLCh* const name,
				      xercesc::AttributeList& attrs ) {

	xmlReaderHelper_.enter( name );

	content_.clear();
	
	if( xmlReaderHelper_.name() == "Page" ) {
	    ++pagesPerFile_;
	}


	else if( xmlReaderHelper_.name() == "String" ) {
	    std::wstring content = XMLReaderHelper::getAttributeWideValue( attrs, "CONTENT" );
	    bool isNormal;
	    
	    size_t left = csl::CSLLocale::string2number< int >( XMLReaderHelper::getAttributeValue( attrs, "HPOS" ) );
	    size_t top = csl::CSLLocale::string2number< int >( XMLReaderHelper::getAttributeValue( attrs, "VPOS" ) );

	    std::wstring coord_id = 
		XMLReaderHelper::getAttributeWideValue( attrs, "HPOS" ) + L"_" + 
		XMLReaderHelper::getAttributeWideValue( attrs, "WIDTH" ) + L"_" + 
		XMLReaderHelper::getAttributeWideValue( attrs, "VPOS" ) + L"_" + 
		XMLReaderHelper::getAttributeWideValue( attrs, "HEIGHT" );
            
            size_t tokenCount = 0;
            size_t normalCount = 0;
	    size_t beginOfToken = 0;
	    size_t border = 0;
            while( ( border = Document::findBorder( content, beginOfToken, &isNormal ) ) != std::wstring::npos ) {
		++tokenCount;
                if( isNormal ) ++normalCount;
                tok_ = new Token( *doc_, doc_->getNrOfTokens(), isNormal );
                tok_->setWOCR( content.substr( beginOfToken, border - beginOfToken ) );
		beginOfToken = border;
                
                tok_->setCoordinates( 
		    left,
		    top,
		    left + csl::CSLLocale::string2number< int >( XMLReaderHelper::getAttributeValue( attrs, "WIDTH" ) ),
		    top + csl::CSLLocale::string2number< int >( XMLReaderHelper::getAttributeValue( attrs, "HEIGHT" ) )
		    );
		
                tok_->setExternalId( coord_id );              
	    
                doc_->pushBackToken( tok_ );
                tok_ = 0;
            }

	    // if more than one normal token was produced, ignore the complete sequence for profiling
            if( normalCount > 1 ) {
		for( Document::iterator it = doc_->end() - tokenCount; it != doc_->end(); ++it ) {
		    it->setDontTouch( true );
		}
                std::wcerr << "Alto token " << content << " produced " << normalCount << " normal tokens." << std::endl;
		//exit( 1 );
            }
            else if( tokenCount > 1 ) {
		//std::wcout << "Alto token " << content << " produced " << tokenCount << " tokens." << std::endl;
	    }

	}


	else if( xmlReaderHelper_.name() == "SP" ) {
	    doc_->pushBackToken( L" ", false );

	}
	else if( xmlReaderHelper_.name() == "HYP" ) {
	    // For the moment, this is inserted as a standard, not-normal token.
	    // The hyphenation info should be used better.
	    doc_->pushBackToken( XMLReaderHelper::getAttributeWideValue( attrs, "CONTENT" ), false );
	}
  
    }



    void AltoXMLReader::endElement(const XMLCh* const initName) {

	std::string name( xmlReaderHelper_.name() ); 
	xmlReaderHelper_.leave( initName );

	if( name == "TextLine" ) {
	    doc_->pushBackToken( L"\n", false );
	}
    }

    void AltoXMLReader::characters(const XMLCh* chars, XMLSize_t length) {

	char* eingabe_cstr = xercesc::XMLString::transcode( chars );
	static std::string eingabe;
	static std::wstring eingabe_wide;

	eingabe = eingabe_cstr;
    
	csl::CSLLocale::string2wstring( eingabe, eingabe_wide );


	content_ += eingabe_wide;
    
	xercesc::XMLString::release( &eingabe_cstr ); 
    }

    void AltoXMLReader::ignorableWhitespace(const XMLCh* const chars, const XMLSize_t length) {
    
    }

    void AltoXMLReader::startDocument() {
	pagesPerFile_ = 0;
	// std::wcout << __FILE__ <<  ": START DOCUMENT" << std::endl;
    }

    void AltoXMLReader::endDocument() {
	if( pagesPerFile_ > 1 ) {
	    std::wcerr << "OCRC::AltoXMLReader::endDocument: Warning: alto file contains more than one page." << std::endl; 
	}
    }




// ---------------------------------------------------------------------------
//  Overrides of the SAX ErrorHandler interface
// ---------------------------------------------------------------------------
    void AltoXMLReader::error(const xercesc::SAXParseException& e) {
	char* message = xercesc::XMLString::transcode(e.getMessage());
	std::string my_message = "OCRC::AltoXMLParser: Xerces error: ";
	my_message += message;
	throw OCRCException( my_message );

	std::wstring wideMessage;
	csl::CSLLocale::string2wstring( std::string( message), wideMessage );
	std::wcerr << wideMessage << std::endl;
	xercesc::XMLString::release(&message);
    }

    void AltoXMLReader::fatalError(const xercesc::SAXParseException& e) {
	char* message = xercesc::XMLString::transcode(e.getMessage());

	std::string my_message = "OCRC::AltoXMLParser: Xerces error: ";
	my_message += message;

	throw OCRCException( my_message );

	std::wstring wideMessage;
	csl::CSLLocale::string2wstring( std::string( message), wideMessage );
	std::wcerr << wideMessage << std::endl;

	xercesc::XMLString::release(&message);
    }

    void AltoXMLReader::warning(const xercesc::SAXParseException& e) {
	char* message = xercesc::XMLString::transcode(e.getMessage());
	std::string my_message = "OCRC::AltoXMLParser: Xerces error: ";
	my_message += message;

	throw OCRCException( my_message );

	std::wstring wideMessage;
	csl::CSLLocale::string2wstring( std::string( message), wideMessage );
	std::wcerr << wideMessage << std::endl;
	xercesc::XMLString::release(&message);
    }

} // eon
#endif
