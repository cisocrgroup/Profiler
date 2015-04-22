#ifndef OCRC_GLOBALPROFILEXMLREADER_CXX
#define OCRC_GLOBALPROFILEXMLREADER_CXX OCRC_GLOBALPROFILEXMLREADER_CXX

#include "./GlobalProfileXMLReader.h"

namespace OCRCorrection {

    GlobalProfileXMLReader::GlobalProfileXMLReader( GlobalProfile* gp ) :
	gp_( gp )
    {
    }

    void GlobalProfileXMLReader::parse( std::string const& xmlFile, GlobalProfile* gp ) {
	gp_ = gp;

	xercesc::XMLPlatformUtils::Initialize();
    
	xercesc::SAXParser parser_;
    
	parser_.setDocumentHandler( this );
	parser_.setErrorHandler( this );

	parser_.parse( xmlFile.c_str() );
    
    
	//  xercesc::XMLPlatformUtils::Terminate();
    
    }



// ---------------------------------------------------------------------------
//  Implementation of the SAX DocumentHandler interface
// ---------------------------------------------------------------------------

// startende Tags, tagname in anführungszeichen
    void GlobalProfileXMLReader::startElement(
	const XMLCh* const initName,
	AttributeList& attrs ) {
	
	xmlReaderHelper_.enter( initName );

	if( xmlReaderHelper_.getExternalHandler() ) {
	    xmlReaderHelper_.getExternalHandler()->startElement( initName, attrs );
	    return;
	}


	

	content_.clear();
	
	if( xmlReaderHelper_.name() == "spelling_variants" ) {
	    xmlReaderHelper_.setExternalHandler( new PatternContainerXMLReader( &( gp_->histPatternProbabilities_ ) ) );
	    xmlReaderHelper_.getExternalHandler()->startElement( initName, attrs );
	}
	else if( xmlReaderHelper_.name() == "ocr_errors" ) {
	    xmlReaderHelper_.setExternalHandler( new PatternContainerXMLReader( &( gp_->ocrPatternProbabilities_ ) ) );
	    xmlReaderHelper_.getExternalHandler()->startElement( initName, attrs );
	}
	else if( xmlReaderHelper_.name() == "item" && xmlReaderHelper_.hasParent( "dictionary_distribution" ) ) {
	    
	    
	    gp_->dictDistribution_[XMLReaderHelper::getAttributeWideValue( attrs, "dict" )].frequency = 
		csl::CSLLocale::string2number< float >( XMLReaderHelper::getAttributeValue( attrs, "frequency" ) );
	    gp_->dictDistribution_[XMLReaderHelper::getAttributeWideValue( attrs, "dict" )].proportion = 
		csl::CSLLocale::string2number< float >( XMLReaderHelper::getAttributeValue( attrs, "proportion" ) );
	}
    }



// schliessende Tags, Tagname in Anführungszeichen
    void GlobalProfileXMLReader::endElement( const XMLCh* const initName ) {
	xmlReaderHelper_.leave( initName );
	
	if( xmlReaderHelper_.getExternalHandler() ) {
	    xmlReaderHelper_.getExternalHandler()->endElement( initName );
	    return;
	}
    }

    void GlobalProfileXMLReader::characters(const XMLCh* chars, XMLSize_t length) {

	if( xmlReaderHelper_.getExternalHandler() ) {
	    xmlReaderHelper_.getExternalHandler()->characters( chars, length );
	    return;
	}
	
	char* eingabe_cstr = XMLString::transcode( chars );
	static std::string eingabe;
	static std::wstring eingabe_wide;
	
	eingabe = eingabe_cstr;
	
	csl::CSLLocale::string2wstring( eingabe, eingabe_wide );

	
	content_ += eingabe_wide;
    
	XMLString::release( &eingabe_cstr ); 
    }

    void GlobalProfileXMLReader::ignorableWhitespace(const XMLCh* const chars, const XMLSize_t length) {
	if( xmlReaderHelper_.getExternalHandler() ) {
	    xmlReaderHelper_.getExternalHandler()->ignorableWhitespace( chars, length );
	    return;
	}
    }

    void GlobalProfileXMLReader::startDocument() {

    }

    void GlobalProfileXMLReader::endDocument() {
	// gp_->histPatternProbabilities_.writeToXML();
    }




// ---------------------------------------------------------------------------
//  Overrides of the SAX ErrorHandler interface
// ---------------------------------------------------------------------------
    void GlobalProfileXMLReader::error(const SAXParseException& e) {
	char* message = XMLString::transcode(e.getMessage());
	std::string my_message = "OCRC::DocXMLParser: Xerces error: ";
	my_message += message;
	throw OCRCException( my_message );

	std::wstring wideMessage;
	csl::CSLLocale::string2wstring( std::string( message), wideMessage );
	std::wcerr << wideMessage << std::endl;
	XMLString::release(&message);
    }

    void GlobalProfileXMLReader::fatalError(const SAXParseException& e) {
	char* message = XMLString::transcode(e.getMessage());

	std::string my_message = "OCRC::DocXMLParser: Xerces error: ";
	my_message += message;

	throw OCRCException( my_message );

	std::wstring wideMessage;
	csl::CSLLocale::string2wstring( std::string( message), wideMessage );
	std::wcerr << wideMessage << std::endl;

	XMLString::release(&message);
    }

    void GlobalProfileXMLReader::warning(const SAXParseException& e) {
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
