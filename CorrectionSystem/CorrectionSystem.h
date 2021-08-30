#ifndef OCRCORRECTION_CORRECTIONSYSTEM_H
#define OCRCORRECTION_CORRECTIONSYSTEM_H OCRCORRECTION_CORRECTIONSYSTEM_H

#include<stdexcept>

#include "../TXTReader/TXTReader.h"
#include "../TXTReader/AlignedTXTReader.h"
#include "../AbbyyXmlParser/AbbyyXmlParser.h"
#include "../DocXML/DocXMLReader.h"
#include "../Document/Document.h"
#include "../AlphaScore/AlphaScore.h"
#include "../Stopwatch.h"
#include "../Utils/Utils.h"
#include "../Profiler/Profiler.h"
#include "../SimpleEnrich/SimpleEnrich.h"
#include "../DictSearch/DictSearch.h"
#include "../INIConfig/INIConfig.h"

namespace OCRCorrection {

    /**
     * @brief This class - nomen est omen - forms the central unit of the correction system.
     *
     * It initialises the Document object and applies a number of utilities to add correction candidates,
     * scores etc.
     *
     *
     */
    class CorrectionSystem {
    public:
	/**
	 * @brief encodes different types of input document.
	 *
	 * IMPORTANT: Remember to keep this syncronized with the java enum in
	 *            jav/correctionBackend/FileType.java
	 */
	enum InputMode {TXT, ALIGN_TXT, DOC_XML, ABBYY_XML, ABBYY_XML_DIR };
	//               0         1      2        3               4

	/**
	 * @brief constructor
	 */
	CorrectionSystem();

	/**
	 * @brief destructor
	 */
	~CorrectionSystem();

	/**
	 * @brief reads an ini configuration file
	 */
	void readConfiguration( char const* configFile );

	/**
	 * @brief Load an existing document in DocXML format
	 *
	 */
	void loadDocument( char const* docFile );

	/**
	 * @brief Create a new document object from the given text file
	 */
	void newDocument( const char* dirName, const char* imageDirName, InputMode inputMode );

	void runProfiler();

	/*
	 * An alternative method to runProfiler() was removed from this class after revision 1483.
	 * It simply used DictSearch to compute a set of candidates for each token.
	 */

	/**
	 * @brief returns a refernce to the document object
	 *
	 * @return a refernce to the document object
	 */
	Document& getDocument();

	/**
	 * needed by jni
	 */
	Document* getDocumentPointer();


    private:
        /**
         * @brief The document currently processed
         */
	Document document_;

	csl::DictSearch dictSearch_;

	csl::INIConfig iniConf_;

    };


} // ns OCRCorrection

#endif
