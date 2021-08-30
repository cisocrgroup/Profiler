#ifndef OCRCORRECT_ABBYY_XML_PARSER_H
#define OCRCORRECT_ABBYY_XML_PARSER_H OCRCORRECT_ABBYY_XML_PARSER_H

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/sax/AttributeList.hpp>
#include <string>
#include <exception>
#include <iostream>
#include <dirent.h>
//#include <sys/types.h>

#include "../Exceptions.h"
#include "../Document/Document.h"

#include "./utf8.h"

XERCES_CPP_NAMESPACE_USE

namespace OCRCorrection {

    class AbbyyXmlParser : public HandlerBase {
  
    public:
	// -----------------------------------------------------------------------
	//  Constructors and Destructor
	// -----------------------------------------------------------------------
	AbbyyXmlParser();
    
	~AbbyyXmlParser();
  
  
	void parsePageToDocument( std::string const& filename, std::string const& init_imageDir, OCRCorrection::Document* doc );

	void parseDirToDocument( std::string const& init_xmlDir, std::string const& imageDir, OCRCorrection::Document* doc );

  
	// -----------------------------------------------------------------------
	//  Handlers for the SAX ContentHandler interface
	// -----------------------------------------------------------------------
  
	void characters(const XMLCh* const chars, const XMLSize_t length);
	void startElement(const XMLCh* const name, AttributeList& attrs);
	void endElement(const XMLCh* const name);
	void ignorableWhitespace(const XMLCh* const chars, const XMLSize_t length);
	void startDocument();
	void endDocument();
  
	// -----------------------------------------------------------------------
	//  Handlers for the SAX ErrorHandler interface
	// -----------------------------------------------------------------------
	void warning(const SAXParseException& exc);
	void error(const SAXParseException& exc);
	void fatalError(const SAXParseException& exc);

  
    private:
	void parseToDocument( char const* xmlFilename, OCRCorrection::Document* doc );

	void sendTokenToDocument( Token* temptoken );

	int left_;
	int top_;
	int right_;
	int bottom_;

	// quick'n'dirty for ignoring abbyy feature
	bool inVariant_;

	bool suspicious_;
  
	wchar_t lastchar_;
	wchar_t tempchar_;
  
	OCRCorrection::Token* temptoken_;
	OCRCorrection::Document* doc_;
	std::string currentFileName_;

    };

} // eon

//#include "./AbbyyXmlParser.cxx"

#endif
