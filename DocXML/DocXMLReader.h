#ifndef OCRC_DOCXMLREADER_H
#define OCRC_DOCXMLREADER_H OCRC_DOCXMLREADER_H

#include<Document/Document.h>
#include<GlobalProfile/GlobalProfile.h>
#include<GlobalProfile/GlobalProfileXMLReader.h>

#include<Utils/XMLReaderHelper.h>

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>


XERCES_CPP_NAMESPACE_USE

namespace OCRCorrection {

    /**
     * @brief Reads DocXML format and stores the doc in a Document object
     *
     * Some details:
     * - the <wOCR_lc> and <wOrig_lc> -containers are ignored. The Token-object has its own functionality
     *   to set wOCR_lc_ and wOrig_lc_ whenever wOCR or wOrig is assigned a new value.
     */
    class DocXMLReader : HandlerBase {
    public:

	/**
	 * @brief Create a DocXMLReader object
	 */
	DocXMLReader();

	void setVerbose( bool );

	/**
	 * @brief Parse an xml file with the given path and write the document into the given Document-object
	 *
	 * As a third argument, a pointer to a globalProfile object can be given. Then the global profile is parsed
	 * into that structure. If there is a pointer given but no profile-xml in the document, an exception is thrown.
	 *
	 * @param[in]  xmlFile
	 * @param[out] document       object that is filled by the parser
	 * @param[out] globalProfile  object that is filled by the parser
	 */
	void parse( std::string const& xmlFile, Document* document, GlobalProfile* globalProfile = 0 );

	/**
	 * @brief This method parses a whole directory of docXML files into a document. It is deprecated,
	 *        new docXML files are supposed to contain all pages of a document in one file.
	 *
	 * @deprecated 
	 *
	 * @param[in]  Directory containing xml files
	 * @param[in]  Directory containing the respective images
	 * @param[out] document object that is filled by the parser
	 */
	//void parseDir( std::string const& init_xmlDir, std::string const& imageDir, Document* doc );


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

	bool selected_;
	
	std::wstring content_;
	
	Document* doc_;
	GlobalProfile* globalProfile_;
	Token* tok_;
	
	XMLReaderHelper xmlReaderHelper_;

	bool verbose_;

    };


} // eon

#endif
