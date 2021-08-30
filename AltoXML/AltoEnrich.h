#ifndef OCRC_ALTOENRICH_H
#define OCRC_ALTOENRICH_H OCRC_ALTOENRICH_H

#include <string>

#include "../AltoXML/AltoXMLReader.h"
#include "../Document/Document.h"
#include "../Profiler/Profiler.h"
#include "../Utils/XMLReaderHelper.h"

#include <xercesc/sax/HandlerBase.hpp>


namespace OCRCorrection {
    class AltoEnrich : xercesc::HandlerBase {
    public:
	AltoEnrich();


	~AltoEnrich();

	/**
	 * @brief Specify a path where to write profiler debug html. An empty string disables the output.
	 */
	inline void setHTMLOutFile( std::string const& file ) {
	    htmlOutFile_= file;
	}

	/**
	 * @brief Reads a document in alto format from a given dierctory, adds profiling information
	 *        and prints Alto xml files to the specified directory
	 *
	 * @param altoDir A directory containing alto xml files.
	 * @param configFile A config file for teh profiling.
	 * @param outDir The output directory
	 *
	 */
	void addProfilerData( std::string const& altoDir, std::string const& configFile, std::string const&  outDir );

    private:

	void parseDir( std::string const& xmlDir );

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

	static std::wstring xml_escape( std::wstring const& input );


	std::wofstream* ostream_;

	Document slaveDocument_;
	Document::iterator slaveIterator_;
	Document::PageIterator slavePageIterator_;

	std::string outputDir_;

	/**
	 * @brief If this string is non-empty, then it is treated as a path
	 *        to write the profiler's html output.
	 *
	 */
	std::string htmlOutFile_;

	XMLReaderHelper xmlReaderHelper_;

    };


}


#endif
