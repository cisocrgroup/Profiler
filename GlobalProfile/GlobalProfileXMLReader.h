#ifndef OCRC_GLOBALPROFILEXMLREADER_H
#define OCRC_GLOBALPROFILEXMLREADER_H OCRC_GLOBALPROFILEXMLREADER_H

#include<string>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>

#include "../GlobalProfile/GlobalProfile.h"
#include "../GlobalProfile/PatternContainerXMLReader.h"

XERCES_CPP_NAMESPACE_USE

namespace OCRCorrection {

    /**
     * @brief 
     *
     */
    class GlobalProfileXMLReader : public HandlerBase {
    public:

	/**
	 * @brief Create a GlobalProfileXMLReader object
	 *
	 * @param GlobalProfile object that will be filled by the parser when parse() is invoked.
	 */
	GlobalProfileXMLReader( GlobalProfile* gp = 0 );

	/**
	 * @brief Parse an xml file with the given path and write the profile into the given object(s)
	 *
	 * @param[in]  xmlFile
	 * @param[out] GlobalProfile object that is filled by the parser
	 */
	void parse( std::string const& xmlFile, GlobalProfile* gp );


    private:
	/**
	 * @brief implements the HandlerBase interface from the xerces-c parser
	 */
	void characters(const XMLCh* chars, XMLSize_t length);

	/**
	 * @brief implements the HandlerBase interface from the xerces-c parser
	 */
	void startElement(const XMLCh* const name, AttributeList& attrs);

	/**
	 * @brief implements the HandlerBase interface from the xerces-c parser
	 */
	void endElement(const XMLCh* const name);

	/**
	 * @brief implements the HandlerBase interface from the xerces-c parser
	 */
	void ignorableWhitespace(const XMLCh* const chars, const XMLSize_t length);

	/**
	 * @brief implements the HandlerBase interface from the xerces-c parser
	 */
	void startDocument();

	/**
	 * @brief implements the HandlerBase interface from the xerces-c parser
	 */
	void endDocument();
	
	// -----------------------------------------------------------------------
	//  Handlers for the SAX ErrorHandler interface
	// -----------------------------------------------------------------------

	/**
	 * @brief implements the HandlerBase interface from the xerces-c parser
	 */
	void warning(const SAXParseException& exc);

	/**
	 * @brief implements the HandlerBase interface from the xerces-c parser
	 */
	void error(const SAXParseException& exc);

	/**
	 * @brief implements the HandlerBase interface from the xerces-c parser
	 */
	void fatalError(const SAXParseException& exc);


	std::wstring content_;
	
	GlobalProfile* gp_;

	XMLReaderHelper xmlReaderHelper_;

    };


} // eon

#endif
