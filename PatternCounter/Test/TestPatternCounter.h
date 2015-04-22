#ifndef OCRCORRECTION_TEST_FREQUENCY_LIST_H
#define OCRCORRECTION_TEST_FREQUENCY_LIST_H OCRCORRECTION_TEST_FREQUENCY_LIST_H

#include "../PatternCounter.h"

#include <cppunit/extensions/HelperMacros.h>


namespace OCRCorrection {
    class TestPatternCounter : public CppUnit::TestFixture  {

	CPPUNIT_TEST_SUITE( TestPatternCounter );
	CPPUNIT_TEST( testBasics );
	CPPUNIT_TEST_SUITE_END();
    public:
	TestPatternCounter();
	void testBasics();
	
    private:

    }; // class TestPatternCounter

    CPPUNIT_TEST_SUITE_REGISTRATION( TestPatternCounter );

    TestPatternCounter::TestPatternCounter() :
	CppUnit::TestFixture() {
	
    }


    /**
     * test the basic functionality
     */
    void TestPatternCounter::testBasics() {

	PatternCounter pc;
	pc.registerNGrams( L"abcdabcde" );

	CPPUNIT_ASSERT( pc.getNGramCount( L"a" ) == 2 );
	CPPUNIT_ASSERT( pc.getNGramCount( L"b" ) == 2 );
	CPPUNIT_ASSERT( pc.getNGramCount( L"c" ) == 2 );
	CPPUNIT_ASSERT( pc.getNGramCount( L"d" ) == 2 );
	CPPUNIT_ASSERT( pc.getNGramCount( L"e" ) == 1 ); // last char

	CPPUNIT_ASSERT( pc.getNGramCount( L"ab" ) == 2 );
	CPPUNIT_ASSERT( pc.getNGramCount( L"bc" ) == 2 );
	CPPUNIT_ASSERT( pc.getNGramCount( L"cd" ) == 2 );
	CPPUNIT_ASSERT( pc.getNGramCount( L"de" ) == 1 ); // last 2-gram

    }


} // eon


#endif
