#ifndef OCRC_ALTOXMLREADER_H
#define OCRC_ALTOXMLREADER_H OCRC_ALTOXMLREADER_H

#include<Utils/Utils.h>
#include<Document/Document.h>

#include<Utils/XMLReaderHelper.h>
#include<xercesc/sax/HandlerBase.hpp>


namespace OCRCorrection {

    /**
     * @brief Reads AltoXML format and stores the doc in a Document object
     *
     */
    class AltoXMLReader : xercesc::HandlerBase {
    public:

	/**
	 * @brief Create a AltoXMLReader object
	 */
	AltoXMLReader();

	~AltoXMLReader();

	/**
	 * @brief Parse an xml file with the given path and write the document into the given Document-object
	 *
	 * @param[in]  xmlFile
	 * @param[out] document       object that is filled by the parser
	 */
	void parse( std::string const& xmlFile, Document* document );

	void parseDir( std::string const& init_xmlDir, std::string const& imageDir, OCRCorrection::Document* doc );


    private:
	/**
	 * @brief implements the HandlerBase interface from the xerces-c parser
	 */
	void characters(const XMLCh* chars, XMLSize_t length);

	/**
	 * @brief implements the HandlerBase interface from the xerces-c parser
	 */
	void startElement(const XMLCh* const name, xercesc::AttributeList& attrs);

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
	void warning(const xercesc::SAXParseException& exc);

	/**
	 * @brief implements the HandlerBase interface from the xerces-c parser
	 */
	void error(const xercesc::SAXParseException& exc);

	/**
	 * @brief implements the HandlerBase interface from the xerces-c parser
	 */
	void fatalError(const xercesc::SAXParseException& exc);

	bool selected_;
	
	std::wstring content_;

	/**
	 * @brief This is to check if one alto file contains several pages.
	 */
	size_t pagesPerFile_;
	
	Document* doc_;
	Token* tok_;
	
	XMLReaderHelper xmlReaderHelper_;

    };


} // eon

#endif
