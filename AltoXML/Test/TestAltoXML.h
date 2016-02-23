#ifndef OCRC_TESTDOCUMENT_H
#define OCRC_TESTDOCUMENT_H OCRC_TESTDOCUMENT_H

#include <Utils/Utils.h>
#include "../AltoXMLReader.h"
#include "../AltoEnrich.h"
#include <cppunit/extensions/HelperMacros.h>


namespace OCRCorrection {
    class TestAltoXML : public CppUnit::TestFixture  {

	CPPUNIT_TEST_SUITE( TestAltoXML );
	CPPUNIT_TEST( testReaderBasics );
	CPPUNIT_TEST( testDirMode   );
	CPPUNIT_TEST( testAltoEnrich   );
	CPPUNIT_TEST_SUITE_END();
    public:
	TestAltoXML();

	void run();
	void testReaderBasics();
	void testDirMode();
	void testAltoEnrich();

    }; // class TestAltoXML

    CPPUNIT_TEST_SUITE_REGISTRATION( TestAltoXML );

    TestAltoXML::TestAltoXML() {

    }

    void TestAltoXML::run() {
	//testReaderBasics();
	testDirMode();
	//testAltoEnrich();

    }


    /**
     * test the basic methods
     */
    void TestAltoXML::testReaderBasics() {
	AltoXMLReader reader;
	Document doc;
	try {
	    reader.parse( Utils::getOCRCBase() + "/cxx/AltoXML/Test/bsb00001830_00035.trans.jpg.ocr.ALTO-Char.xml" , &doc );
	} catch( std::exception& exc ) {
                std::wstring wide_what(Utils::utf8(exc.what()));
                //csl::CSLLocale::string2wstring( exc.what(), wide_what );
	    std::wcerr << "CAUGHT:" << wide_what << std::endl;
	}
	doc.print();
    }

    void TestAltoXML::testDirMode() {
	AltoXMLReader reader;
	Document doc;
	try {
	    reader.parseDir( Utils::getOCRCBase() + "/cxx/AltoXML/Test/alto_dir" , "_NO_IMAGE_DIR_", &doc );
	} catch( std::exception& exc ) {
                std::wcerr << "CAUGHT:" << Utils::utf8(exc.what()) << std::endl;
	    CPPUNIT_FAIL( "TEST" );
	}
	std::wstringstream sstr;
	doc.dumpOCRToPlaintext( sstr );
	std::wstring str =  sstr.str();
	std::wcout << str << std::endl;
	// DOESNT WORK! CPPUNIT_ASSERT( str == L"4 Wachs zerschmilzt beym Feuer, und d" );

    }

    void TestAltoXML::testAltoEnrich() {
	AltoEnrich altoEnrich;

	std::wofstream ofs( "/dev/null" );
	altoEnrich.addProfilerData( Utils::getOCRCBase() + "/cxx/AltoXML/Test/bsb00001830_00035.trans.jpg.ocr.ALTO-Char.xml" , "/mounts/Users/student/uli/implement/OCRC_trunk/dictionaries/ocrc.ini", "/tmp" );

    }


}
#endif
