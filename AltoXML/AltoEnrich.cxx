#ifndef OCRC_ALTOENRICH_CXX
#define OCRC_ALTOENRICH_CXX OCRC_ALTOENRICH_CXX


#include "./AltoEnrich.h"

namespace OCRCorrection {
// ---------------------------------------------------------------------------
//  Implementation of the SAX DocumentHandler interface
// ---------------------------------------------------------------------------

    AltoEnrich::AltoEnrich() : 
	ostream_( 0 )
    {
	xercesc::XMLPlatformUtils::Initialize();
    }

    AltoEnrich::~AltoEnrich() {
	xercesc::XMLPlatformUtils::Terminate();
    }
    
    void AltoEnrich::addProfilerData( std::string const& altoDir, std::string const& configFile, std::string const& init_outputDir ) {
	
	outputDir_ = init_outputDir;
	

	//////// read alto xml into slave document ///////
	AltoXMLReader reader;
	slaveDocument_.clear();
	reader.parseDir( altoDir, "_NO_IMAGE_DIR_", &slaveDocument_ );
	
	if( slaveDocument_.empty() ) {
	    throw OCRCException( "OCRC::AltoEnrich: Slave document empty." );
	}
	
	
	/////// Add profiler data to slave document //////////
	Profiler profiler;

	if( ! htmlOutFile_.empty() ) {
	    profiler.setHTMLOutFile( htmlOutFile_ );
	}
	
	std::wcerr << "runProfiler::readConfig" << std::endl;
	
	try {
	    profiler.readConfiguration( configFile );
	    
	    std::wcerr << "Create Profile" << std::endl;
	    // createProfile adds candidates to the document object
	    profiler.createProfile( slaveDocument_ );
	    std::wcerr << "Finished" << std::endl;
	} catch( OCRCException& exc ) {
	    std::wcout << exc.what() << std::endl;
	    throw exc;
	}
	

	parseDir( altoDir );
	
    }
    

    void AltoEnrich::parseDir( std::string const& init_xmlDir ) {

	if( init_xmlDir.empty() ) {
	    throw OCRCException( "OCRC::AltoXMLReader::parseDirToDocument: xmlDir is empty string." );
	}
	
	std::string xmlDir = Utils::normalizeFilePath( init_xmlDir );
	
	
	std::wstring wideXmlDir;
	csl::CSLLocale::string2wstring( xmlDir, wideXmlDir );
	std::wcerr << "OCRC::AltoEnrich::parseDir: Parse " << wideXmlDir << std::endl;
	

	xercesc::SAXParser parser;
	parser.setDocumentHandler( this );
	parser.setErrorHandler( this );

	///////  handle directory I/O
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

	
	

	//////// set slaveIterator to the beginning of the document
	slavePageIterator_ = slaveDocument_.pagesBegin();

	if( slavePageIterator_->begin() == slavePageIterator_->end() ) {
	    throw OCRCException( "OCRC::AltoEnrich: UUuhhh" );
	}
	
	
	/////////  process all pages
	for( std::vector< std::string >::iterator entry = dirEntries.begin(); entry != dirEntries.end(); ++entry ) {
	    if( *entry == "." || *entry == ".." ) {
		continue;
	    }
	    
	    // only .xml files
	    if( entry->compare( entry->size() - 4, 4, ".xml" ) == 0 ) {
		if( slavePageIterator_ == slaveDocument_.pagesEnd() ) {
		    throw OCRCException( "OCRC::AltoEnrich: Expected more pages in slave document." );
		}

		slaveIterator_ = slavePageIterator_->begin();


		
		std::string inFile = xmlDir + std::string( "/" ) +  *entry;
		std::string outFile = outputDir_ + std::string( "/" ) +  *entry;

		ostream_ = new std::wofstream( outFile.c_str() );
		
		std::wcout << "Parse " << csl::CSLLocale::string2wstring( inFile ) << std::endl;
		parser.parse( inFile.c_str() );

		ostream_->close();
		delete( ostream_ );

		
		if( slaveIterator_ != slavePageIterator_->end() ) {
		    while( slaveIterator_ != slavePageIterator_->end() ) {
			std::wcout << "LEFT: '" << slaveIterator_->getWOCR() << "'" << std::endl;
			++slaveIterator_;
		    }
		}
		++slavePageIterator_;
		std::wcerr << "Successfully wrote " << csl::CSLLocale::string2wstring( outFile ) << std::endl;

	    }
	    else { // no "xml" extension
		std::wstring wideEntry;
		csl::CSLLocale::string2wstring( *entry, wideEntry );
		std::wcerr << "OCRC::AltoXMLReader::parseDirToDocument: Ignored non-xml file " << wideEntry << std::endl;
	    }
	}
	
	std::wcerr << "OCRC::AltoEnrich::parseDirToDocument: parsed directory" << std::endl;
        
    
    }


    void AltoEnrich::startElement( const XMLCh* const name,
				      xercesc::AttributeList& attrs ) {
	
	xmlReaderHelper_.enter( name );
	
	std::wstring wideName;
	csl::CSLLocale::string2wstring( xmlReaderHelper_.name(), wideName );
	

	
	if( xmlReaderHelper_.name() == "String" ) {
	    char* key_cstr = 0;
	    char* value_cstr = 0;
	    std::wstring key_str, value_str;

	    std::wcerr << "startStringElement: " << XMLReaderHelper::getAttributeWideValue( attrs, "CONTENT" ) << std::endl;

	    
	    std::wstring coord_id = 
		XMLReaderHelper::getAttributeWideValue( attrs, "HPOS" ) + L"_" + 
		XMLReaderHelper::getAttributeWideValue( attrs, "WIDTH" ) + L"_" + 
		XMLReaderHelper::getAttributeWideValue( attrs, "VPOS" ) + L"_" + 
		XMLReaderHelper::getAttributeWideValue( attrs, "HEIGHT" );


	    if( slaveIterator_ == slavePageIterator_->end() ) {
		throw OCRCException( std::string( "OCRC::AltoEnrich: Unexpected end of slave page. alto string=" ) + XMLReaderHelper::getAttributeValue( attrs, "CONTENT" ) );
	    }

	    // skip all slave tokens not fitting the id of the current master token
	    while( coord_id != slaveIterator_->getExternalId() ) {
		std::wcerr << "Skip: " << slaveIterator_->getWOCR() << std::endl;
		++slaveIterator_;
		if( slaveIterator_ == slavePageIterator_->end() ) {
		    throw OCRCException( "OCRC::AltoEnrich: Unexpected end of slave page (during skip)." );
		}
	    }

	    // collect all slave tokens fitting the id of the current master token.
	    // One master (alto) token can be split up in several internal tokens
	    size_t tokenCount = 0;
	    Document::iterator tokenWithCandidates = slavePageIterator_->end();
	    std::wstring prefix;
	    std::wstring suffix;
	    while( ( slaveIterator_ != slavePageIterator_->end() ) && 
		   ( coord_id == slaveIterator_->getExternalId() ) ) {

		if( slaveIterator_->getNrOfCandidates() > 0 ) {
		    if( tokenWithCandidates != slavePageIterator_->end() ) {
			throw OCRCException( "OCRC::AltoEnrich: Error: For one Alto token, found more than one internal token with correction candidates" );
		    }
		    tokenWithCandidates = slaveIterator_;
		}
		else { // no candidates
		    if( tokenWithCandidates == slavePageIterator_->end() ) {
			prefix += slaveIterator_->getWOCR();
		    }
		    else {
			suffix += slaveIterator_->getWOCR();
		    }
		}
		++slaveIterator_;
	    }

	    std::wstring slaveWOCR;
	    if( tokenWithCandidates != slavePageIterator_->end() ) { // found a token with candidates
		slaveWOCR = prefix + tokenWithCandidates->getWOCR() + suffix;
	    }
	    else { // found no token with candidates: all contents went to prefix
		slaveWOCR = prefix;
	    }
	    
	    if( slaveWOCR != XMLReaderHelper::getAttributeWideValue( attrs, "CONTENT" ) ) {
		std::wcout << "COMP: " << slaveWOCR << " : " <<  XMLReaderHelper::getAttributeWideValue( attrs, "CONTENT" ) 
			   << std::endl;
		// std::wcout << "COMP: " << (*).getExternalId() << " : " <<  XMLReaderHelper::getAttributeWideValue( attrs, "ID" ) 
		// 	   << std::endl;
		throw OCRCException( "OCRC::AltoEnrich: alignment of documents failed." );
	    }


	    // write tag, with additional attributes for correction suggestions WS1, WS2, WS3
	    *ostream_ << "<" << wideName;
	    for( size_t i = 0; i < attrs.getLength(); ++i ) {
		XMLCh const* key = attrs.getName( i );
		key_cstr = xercesc::XMLString::transcode( key );
		csl::CSLLocale::string2wstring( key_cstr, key_str );
		
		XMLCh const* value = attrs.getValue( i );
		value_cstr = xercesc::XMLString::transcode( value );
		csl::CSLLocale::string2wstring( value_cstr, value_str );
		
		
		*ostream_ << " " << key_str << "=\"" << xml_escape( value_str ) << "\"";
		xercesc::XMLString::release( &key_cstr );
		xercesc::XMLString::release( &value_cstr );
	    }

	    if( tokenWithCandidates != slavePageIterator_->end() ) {
		size_t candCount = 0;
		for( Token::CandidateIterator candIterator = tokenWithCandidates->candidatesBegin();
		     ( candIterator != slaveIterator_->candidatesEnd() ) && candCount < 3;
		     ++candIterator
		    ) {
		    std::wstring suggestion = prefix + candIterator->getString() + suffix;
		    if( suggestion != slaveWOCR ) {
			*ostream_ << " WS" << candCount + 1 << "=\"" << xml_escape( suggestion ) << "\"";
			++candCount;
		    }
		}
		//*ostream_ << " NrC=\"" << tokenWithCandidates->getNrOfCandidates() << "\"";
	    }


	    *ostream_ << ">";

		     
	}

	else { // copy tag one by one!
	    *ostream_ << "<" << wideName;
	    char* key_cstr = 0;
	    char* value_cstr = 0;
	    std::wstring key_str, value_str;
	    for( size_t i = 0; i < attrs.getLength(); ++i ) {
		XMLCh const* key = attrs.getName( i );
		key_cstr = xercesc::XMLString::transcode( key );
		csl::CSLLocale::string2wstring( key_cstr, key_str );

		XMLCh const* value = attrs.getValue( i );
		value_cstr = xercesc::XMLString::transcode( value );
		csl::CSLLocale::string2wstring( value_cstr, value_str );
		
		*ostream_ << " " << key_str << "=\"" << xml_escape( value_str ) << "\"";

		xercesc::XMLString::release( &key_cstr );
		xercesc::XMLString::release( &value_cstr );
	    } 
	    *ostream_ << ">";
	}
    }
    


    void AltoEnrich::endElement(const XMLCh* const initName) {

	std::string name( xmlReaderHelper_.name() ); 
	xmlReaderHelper_.leave( initName );

	std::wstring wideName;
	csl::CSLLocale::string2wstring( name, wideName );
	*ostream_ << "</" << wideName << ">";

    }

    void AltoEnrich::characters(const XMLCh* chars, XMLSize_t length) {

	char* eingabe_cstr = xercesc::XMLString::transcode( chars );
	static std::string eingabe;
	static std::wstring eingabe_wide;

	eingabe = eingabe_cstr;
    
	csl::CSLLocale::string2wstring( eingabe, eingabe_wide );

	*ostream_ << xml_escape( eingabe_wide );
    
	xercesc::XMLString::release( &eingabe_cstr ); 
    }

    void AltoEnrich::ignorableWhitespace(const XMLCh* const chars, const XMLSize_t length) {
	char* eingabe_cstr = xercesc::XMLString::transcode( chars );
	static std::string eingabe;
	static std::wstring eingabe_wide;

	eingabe = eingabe_cstr;
    
	csl::CSLLocale::string2wstring( eingabe, eingabe_wide );

	*ostream_ << xml_escape( eingabe_wide );
    
	xercesc::XMLString::release( &eingabe_cstr ); 
    }

    void AltoEnrich::startDocument() {
    }

    void AltoEnrich::endDocument() {
    }




// ---------------------------------------------------------------------------
//  Overrides of the SAX ErrorHandler interface
// ---------------------------------------------------------------------------
    void AltoEnrich::error(const xercesc::SAXParseException& e) {
	char* message = xercesc::XMLString::transcode(e.getMessage());
	std::string my_message = "OCRC::AltoXMLParser: Xerces error: ";
	my_message += message;
	throw OCRCException( my_message );

	std::wstring wideMessage;
	csl::CSLLocale::string2wstring( std::string( message), wideMessage );
	std::wcerr << wideMessage << std::endl;
	xercesc::XMLString::release(&message);
    }

    void AltoEnrich::fatalError(const xercesc::SAXParseException& e) {
	char* message = xercesc::XMLString::transcode(e.getMessage());

	std::string my_message = "OCRC::AltoXMLParser: Xerces error: ";
	my_message += message;

	throw OCRCException( my_message );

	std::wstring wideMessage;
	csl::CSLLocale::string2wstring( std::string( message), wideMessage );
	std::wcerr << wideMessage << std::endl;

	xercesc::XMLString::release(&message);
    }

    void AltoEnrich::warning(const xercesc::SAXParseException& e) {
	char* message = xercesc::XMLString::transcode(e.getMessage());
	std::string my_message = "OCRC::AltoXMLParser: Xerces error: ";
	my_message += message;

	throw OCRCException( my_message );

	std::wstring wideMessage;
	csl::CSLLocale::string2wstring( std::string( message), wideMessage );
	std::wcerr << wideMessage << std::endl;
	xercesc::XMLString::release(&message);
    }

    std::wstring AltoEnrich::xml_escape( std::wstring const& input ) {
	std::wstring str = input;
	size_t pos = 0;
	while( ( pos = str.find( '&', pos ) ) != std::wstring::npos ) {
	    str.replace( pos, 1, L"&amp;" ); 
	    ++pos;
	}
	pos = 0;
	while( ( pos = str.find( '>', pos ) ) != std::wstring::npos ) {
	    str.replace( pos, 1, L"&gt;" ); 
	    ++pos;
	}
	pos = 0;
	while( ( pos = str.find( '<', pos ) ) != std::wstring::npos ) {
	    str.replace( pos, 1, L"&lt;" ); 
	    ++pos;
	}
	return str;
    }
    

}



#endif
