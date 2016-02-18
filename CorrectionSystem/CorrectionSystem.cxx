#ifndef OCRCORRECTION_CORRECTIONSYSTEM_CXX
#define OCRCORRECTION_CORRECTIONSYSTEM_CXX OCRCORRECTION_CORRECTIONSYSTEM_CXX


#include "./CorrectionSystem.h"

namespace OCRCorrection {
    CorrectionSystem::CorrectionSystem() {
    }

    CorrectionSystem::~CorrectionSystem() {
    }

    Document& CorrectionSystem::getDocument() {
	return document_;
    }

    Document* CorrectionSystem::getDocumentPointer() {
	return &document_;
    }


    void CorrectionSystem::readConfiguration( char const* configFile ) {
            std::wstring wideFilename = Utils::utf8(configFile);
            //csl::CSLLocale::string2wstring( configFile, wideFilename );


	iniConf_.load( configFile );

	std::wcerr << "cxx::CorrectionSystem::readConfiguration: Finished" << std::endl;
    }


    void CorrectionSystem::loadDocument(  char const* filename ) {
	document_.clear();

	DocXMLReader p;
	p.parse( filename, &document_ );
    }


    void CorrectionSystem::newDocument(  char const* dirName, char const* imageDirName, InputMode inputMode ) {
	std::wcerr
	    << "c++::CorrSys::newDocument."
	    // << "dirName=" << dirName
	    // <<  ", imageDir=" << imageDirName
	    // << ", Input mode is " << inputMode
	    << std::flush
	    << std::endl;

	document_.clear();


	if( inputMode == ABBYY_XML ) {
	    AbbyyXmlParser p;
	    p.parsePageToDocument( dirName, imageDirName, &document_ );
	}
	else if( inputMode == ABBYY_XML_DIR ) {
	    AbbyyXmlParser p;
	    p.parseDirToDocument( dirName, imageDirName, &document_ );
	}
	else {
	    std::wcerr << "OCRCorrection::CorrectionSystem::newDocument: This signature is only for filetype ABBYY_XML and ABBY_XML_DIR" << std::endl;
	    exit( 1 );
	}

	// std::wcerr << "Run profiler" << std::endl;
	// runProfiler();
	// std::wcerr << "done" << std::endl;

	SimpleEnrich se( iniConf_ );
	se.enrichDocument( &document_ );

	std::wcerr << "c++::CorrSys::newDocument: Finished" << std::endl;

    }




    void CorrectionSystem::runProfiler() {
	Profiler profiler;

	std::wcerr << "runProfiler::readConfig" << std::endl;

	try {
	    profiler.readConfiguration( iniConf_ );
	} catch( OCRCException& exc ) {
	    std::wcout << exc.what() << std::endl;
	    throw exc;
	}

	std::wcerr << "Create Profile" << std::endl;
	// createProfile adds candidates to the document object
	profiler.createProfile( document_ );
	std::wcerr << "Finished" << std::endl;
    }



} // eon


#endif
