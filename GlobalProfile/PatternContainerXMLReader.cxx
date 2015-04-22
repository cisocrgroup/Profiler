#ifndef OCRC_PATTERNCONTAINERXMLREADER_CXX
#define OCRC_PATTERNCONTAINERXMLREADER_CXX OCRC_PATTERNCONTAINERXMLREADER_CXX

#include "./PatternContainerXMLReader.h"

namespace OCRCorrection {

    PatternContainerXMLReader::PatternContainerXMLReader( PatternContainer* pc ) :
	pc_( pc )
    {
	
    }

    void PatternContainerXMLReader::parse( std::string const& xmlFile, PatternContainer* pc ) {
	pc_ = pc;
	
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

    void PatternContainerXMLReader::startElement(
	const XMLCh* const initName,
	AttributeList& attrs ) {

	xmlReaderHelper_.enter( initName );

	if( ! pc_ ) throw OCRCException( "OCRC::PatternContainerXMLReader::startElement: no PatternContainer object specified." );


	
	content_.clear();
	
	if( xmlReaderHelper_.name() == "pattern" ) {
//<pattern left="i" right="y" pat_string="i_y" relFreq="0.0389177" absFreq="257.693"/>

	    XMLCh const* attrValue = 0;

	    std::wstring left;
	    attrValue = attrs.getValue( "left" );
	    char* left_cstr = XMLString::transcode( attrValue );
	    csl::CSLLocale::string2wstring( left_cstr, left );
	    XMLString::release( &left_cstr );

	    std::wstring right;
	    attrValue = attrs.getValue( "right" );
	    char* right_cstr = XMLString::transcode( attrValue );
	    csl::CSLLocale::string2wstring( right_cstr, right );
	    XMLString::release( &right_cstr );
	    
	    double relFreq = 0;
	    attrValue = attrs.getValue( "relFreq" );
	    char* relFreq_cstr = XMLString::transcode( attrValue );
	    relFreq = csl::CSLLocale::string2number< double >( std::string( relFreq_cstr ) );
	    XMLString::release( &relFreq_cstr );
	    
	    double absFreq = 0;
	    attrValue = attrs.getValue( "absFreq" );
	    char* absFreq_cstr = XMLString::transcode( attrValue );
	    absFreq = csl::CSLLocale::string2number< double >( std::string( absFreq_cstr ) );
	    XMLString::release( &absFreq_cstr );

	    
	    pc_->setWeight( csl::Pattern( left, right ), relFreq, absFreq );

	}
    }
    

// schliessende Tags, Tagname in Anführungszeichen
    void PatternContainerXMLReader::endElement( const XMLCh* const initName ) {
	xmlReaderHelper_.leave( initName );

	if( ! pc_ ) throw OCRCException( "OCRC::PatternContainerXMLReader::Element: no PatternContainer object specified." );

	
	
    }

    void PatternContainerXMLReader::characters(const XMLCh* chars, XMLSize_t length) {

	char* eingabe_cstr = XMLString::transcode( chars );
	static std::string eingabe;
	static std::wstring eingabe_wide;

	eingabe = eingabe_cstr;
    
	csl::CSLLocale::string2wstring( eingabe, eingabe_wide );


	content_ += eingabe_wide;
    
	XMLString::release( &eingabe_cstr ); 
    }

    void PatternContainerXMLReader::ignorableWhitespace(const XMLCh* const chars, const XMLSize_t length) {
    }

    void PatternContainerXMLReader::startDocument() {

    }

    void PatternContainerXMLReader::endDocument() {

    }




// ---------------------------------------------------------------------------
//  Overrides of the SAX ErrorHandler interface
// ---------------------------------------------------------------------------
    void PatternContainerXMLReader::error(const SAXParseException& e) {
	char* message = XMLString::transcode(e.getMessage());
	std::string my_message = "OCRC::DocXMLParser: Xerces error: ";
	my_message += message;
	throw OCRCException( my_message );

	std::wstring wideMessage;
	csl::CSLLocale::string2wstring( std::string( message), wideMessage );
	std::wcerr << wideMessage << std::endl;
	XMLString::release(&message);
    }

    void PatternContainerXMLReader::fatalError(const SAXParseException& e) {
	char* message = XMLString::transcode(e.getMessage());

	std::string my_message = "OCRC::DocXMLParser: Xerces error: ";
	my_message += message;

	throw OCRCException( my_message );

	std::wstring wideMessage;
	csl::CSLLocale::string2wstring( std::string( message), wideMessage );
	std::wcerr << wideMessage << std::endl;

	XMLString::release(&message);
    }

    void PatternContainerXMLReader::warning(const SAXParseException& e) {
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
