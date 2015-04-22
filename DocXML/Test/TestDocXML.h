#ifndef OCRC_TESTDOCUMENT_H
#define OCRC_TESTDOCUMENT_H OCRC_TESTDOCUMENT_H

#include "../DocXMLReader.h"
#include "../DocXMLWriter.h"
#include<AbbyyXmlParser/AbbyyXmlParser.h>
#include <cppunit/extensions/HelperMacros.h>


namespace OCRCorrection {
    class TestDocXML : public CppUnit::TestFixture  {

	CPPUNIT_TEST_SUITE( TestDocXML );
	CPPUNIT_TEST( testReaderBasics );
	CPPUNIT_TEST( testWriterBasics );
	CPPUNIT_TEST( testReadWrite );
	CPPUNIT_TEST( testWriter_Abbyy2DocXML );
	CPPUNIT_TEST_SUITE_END();
    public:
	TestDocXML();

	void run();

	void testReaderBasics();
	void testWriterBasics();
	void testReadWrite();
	void testWriter_Abbyy2DocXML();


    private:
	std::locale loc_;
    };

    CPPUNIT_TEST_SUITE_REGISTRATION( TestDocXML );

    TestDocXML::TestDocXML() {
	
    }

    void TestDocXML::run() {
	// testReaderBasics();
	// testWriterBasics();
        testReadWrite();
	// testWriter_Abbyy2DocXML();
	
    }
    

    /**
     * test the basic methods
     */
    void TestDocXML::testReaderBasics() {
	DocXMLReader reader;
	Document doc;
	reader.parse( "../DocXML/Test/docdir/test1.xml", &doc );
	doc.print();
    }
    
    /**
     * test the basic methods
     */
    void TestDocXML::testWriterBasics() {

	DocXMLReader reader;
	DocXMLWriter writer;

	Document doc;
	std::wcout << "Read" << std::endl;
	reader.parse( "/tmp/doc.xml", &doc );
	std::wcout << "Write" << std::endl;
	writer.writeXML( doc, std::wcout );


    }


    void TestDocXML::testReadWrite() {
	Document doc, doc2;
	AbbyyXmlParser abbyyReader;
	DocXMLReader reader;
	DocXMLWriter writer;

	std::string tmpFile = "/tmp/testDocXML.out.xml";

	abbyyReader.parsePageToDocument( "../DocXML/Test/abbyy_doc.xml", "_NO_IMAGE_DIR_", &doc );
	std::wcout << "Document has " << doc.getNrOfTokens() << " tokens." << std::endl;
	writer.writeXML( doc, tmpFile );
	std::wcout << "XML was written." << std::endl;

	reader.parse( tmpFile, &doc2 );
	std::wcout << "XML was read." << std::endl;
	std::wcout << "Document has " << doc2.getNrOfTokens() << " tokens." << std::endl;

    }

    void TestDocXML::testWriter_Abbyy2DocXML() {
	Document doc;
	AbbyyXmlParser abbyyReader;
	DocXMLWriter writer;

	abbyyReader.parsePageToDocument( "../DocXML/Test/abbyy_doc.xml", "_NO_IMAGE_DIR_", &doc );
	writer.writeXML(doc, std::wcout );
	
    }


}
#endif
