#ifndef OCRCORRECTION_TEST_FREQUENCY_LIST_H
#define OCRCORRECTION_TEST_FREQUENCY_LIST_H OCRCORRECTION_TEST_FREQUENCY_LIST_H

#include "../FrequencyList.h"

#include <cppunit/extensions/HelperMacros.h>


namespace OCRCorrection {
    class TestFrequencyList : public CppUnit::TestFixture  {

	CPPUNIT_TEST_SUITE( TestFrequencyList );
	CPPUNIT_TEST( testBasics );
	CPPUNIT_TEST( testFileDump );
	CPPUNIT_TEST_SUITE_END();
    public:
	TestFrequencyList();

	void run();
	void testBasics();
	void testFileDump();
	
    private:

    }; // class TestFrequencyList

    CPPUNIT_TEST_SUITE_REGISTRATION( TestFrequencyList );

    TestFrequencyList::TestFrequencyList() :
	CppUnit::TestFixture() {
	
    }

    
    void TestFrequencyList::run() { 
	testBasics();
	testFileDump();
    }


    /**
     * test the basic functionality
     */
    void TestFrequencyList::testBasics() {
	FrequencyList freqlist;

	csl::FBDic<> modernDict;
	modernDict.loadFromFile( "/mounts/data/proj/impact/lexika/anwendungen/AND09/staticlex_de.fbdic" );

	FrequencyList::Trainer trainer( freqlist, "/mounts/Users/student/uli/implement/OCRC_trunk/dictionaries/guessTraces.ini" );
	
	Document doc;
//	doc.parseTXT( ".txt" );
	doc.pushBackToken( L"abracadabra", true );
	doc.pushBackToken( L" ", false );
	doc.pushBackToken( L"bliblablu", true );
	doc.pushBackToken( L"fluchtete", true );
	doc.pushBackToken( L"Groß", true );

	doc.pushBackToken( L"bliblablu", true );
	doc.pushBackToken( L"ganzkomischeswort", true );

	trainer.doTraining( doc );
	
	trainer.finishTraining();
	trainer.writeCorpusLexicon( "corpuslexicon.lex" );

	// lowercased??
	CPPUNIT_ASSERT_MESSAGE( "lowercase works", freqlist.getBaseWordFrequency( L"groß" ) == 1 );

	CPPUNIT_ASSERT( freqlist.getNrOfTrainingTokens() == 6 );

	// basic testing for external baseWord frequencies
	csl::MinDic< float > baseWordFreqs;
	baseWordFreqs.initConstruction();
	baseWordFreqs.addToken( L"anna", 42.17 );
	baseWordFreqs.addToken( L"berta", 43.18 );
	baseWordFreqs.finishConstruction();
	
	freqlist.connectBaseWordFrequency( &baseWordFreqs );

	CPPUNIT_ASSERT( fabs( freqlist.getBaseWordFrequency( L"anna" ) - 42.17 ) < 0.0001 );

    }

    /**
     * test if load/dump from/to disk work
     */
    void TestFrequencyList::testFileDump() {
	FrequencyList freqlist;

	csl::FBDic<> modernDict;
	modernDict.loadFromFile( "/mounts/data/proj/impact/lexika/anwendungen/AND09/staticlex_de.fbdic" );

	FrequencyList::Trainer trainer( freqlist, "/mounts/Users/student/uli/implement/OCRC_trunk/dictionaries/guessTraces.ini" );
	
	Document doc;
//	doc.parseTXT( ".txt" );
	doc.pushBackToken( L"abracadabra", true );
	doc.pushBackToken( L" ", false );
	doc.pushBackToken( L"fluchtete", true );
	trainer.doTraining( doc );
	
	trainer.finishTraining();
	freqlist.writeToFile( "testFreqlist.binfrq", "testFreqlist.txt" );
	
	FrequencyList freqlist2;
	freqlist2.loadFromFile( "testFreqlist.binfrq", "testFreqlist.txt" );
	
	CPPUNIT_ASSERT( ( freqlist2.getInterpretationCount( L"fluchtete:fluchtete+[]" ) == 1 ) );
	CPPUNIT_ASSERT( ( freqlist2.getInterpretationCount( L"abracadabra:abrakadabra+[(k_c,4)]" ) == 1 ) );
	CPPUNIT_ASSERT( freqlist2.getBaseWordFrequency( L"abrakadabra" ) == 1 );

	CPPUNIT_ASSERT( freqlist2.getNrOfTrainingTokens() == 2 );

    }



} // eon


#endif
