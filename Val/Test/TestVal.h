#ifndef CSL_TEST_VAL_H
#define CSL_TEST_VAL_H CSL_TEST_VAL_H

#include "../../Global.h"
#include "../Val.h"

#include <cppunit/extensions/HelperMacros.h>


namespace csl {
    class TestVal : public CppUnit::TestFixture  {

	CPPUNIT_TEST_SUITE( TestVal );
	CPPUNIT_TEST( testBasics );
	CPPUNIT_TEST( testWordBorders );
	CPPUNIT_TEST_SUITE_END();
    public:
	TestVal();
	void testPrint();
	void testBasics();
	void testWordBorders();
// 	void testConstructionDetails();
// 	void lookupAllKeys();
	
    private:
	std::string path_;

    }; // class TestVal

    CPPUNIT_TEST_SUITE_REGISTRATION( TestVal );

    TestVal::TestVal() :
	CppUnit::TestFixture() {

    }


    void TestVal::testPrint() {
	MinDic<> baseDic;
	baseDic.initConstruction();
	baseDic.addToken( L"anna", 42 );
	baseDic.addToken( L"teil", 42 );
	baseDic.finishConstruction();

	Val val( baseDic, "../csl/Val/Test/small.patterns.txt" );
	Val::CandidateReceiver receiver;

	val.query( L"theyl", &receiver );
	
	for( Val::CandidateReceiver::const_iterator it = receiver.begin(); it != receiver.end(); ++it ) {
	    it->print();
	}

    }




    /**
     * test the basic methods for reading access like getRoot, walk, isFinal etc.
     */
    void TestVal::testBasics() {
 	MinDic<> baseDic( "../csl/Vaam/Test/small.base.mdic" );
	MinDic<> filterDic( "../csl/Vaam/Test/small.filter.mdic" );
	Val val( baseDic,  "../csl/Vaam/Test/small.patterns.txt" );
	
	Val::CandidateReceiver answers;

	// a standard variant
	CPPUNIT_ASSERT( val.query( std::wstring( L"xachen" ), &answers ) );
	CPPUNIT_ASSERT( answers.size() == 1 );
	Interpretation& answer = answers.at( 0 ); 
	CPPUNIT_ASSERT( answer.getBaseWord() == L"aachen" );
	CPPUNIT_ASSERT( answer.getInstruction().size() == 1 );
	CPPUNIT_ASSERT( answer.getInstruction().at( 0 ).getLeft() == L"a" );
	CPPUNIT_ASSERT( answer.getInstruction().at( 0 ).getRight() == L"x" );

	answers.clear();

	///////////// MAX NR OF PATTERNS ///////////////////////
	// with at most 0 variants, this should be no variant
	val.setMaxNrOfPatterns( 0 );
	CPPUNIT_ASSERT( ! val.query( std::wstring( L"xachen" ), &answers ) );
	// with at most 1 variant, this should work again
	val.setMaxNrOfPatterns( 1 );
	CPPUNIT_ASSERT( val.query( std::wstring( L"xachen" ), &answers ) );
	// but not a word with 2 patterns
	CPPUNIT_ASSERT( ! val.query( std::wstring( L"kleintheyle" ), &answers ) );

	// switch back to an infinite nr of patterns
	val.setMaxNrOfPatterns( Val::INFINITE );
	CPPUNIT_ASSERT( val.query( std::wstring( L"kleintheyle" ), &answers ) );


	///////////// MIN NR OF PATTERNS ///////////////////////

	// default should be 0
	CPPUNIT_ASSERT( val.query( std::wstring( L"aachen" ), &answers ) );

	val.setMinNrOfPatterns( 1 );
	CPPUNIT_ASSERT( ! val.query( std::wstring( L"aachen" ), &answers ) );
	
	val.setMinNrOfPatterns( 2 );
	CPPUNIT_ASSERT( ! val.query( std::wstring( L"xachen" ), &answers ) );
	CPPUNIT_ASSERT( val.query( std::wstring( L"xxchen" ), &answers ) );
	
	val.setMinNrOfPatterns( 0 );


// 	///////// FILTER DIC ///////////////////////////////

// 	// without the filterDic_ this should be a variant
// 	CPPUNIT_ASSERT( val.query( std::wstring( L"hanne" ), &answers ) );

// 	// now it should be filtered
// 	val.setFilterDic( filterDic );
// 	CPPUNIT_ASSERT( ! val.query( std::wstring( L"hanne" ), &answers ) );

    }


    // stolen from testVaam
    void TestVal::testWordBorders() {
	std::wcout << "*** TestVal::testWordBorders ***" << std::endl;
 	MinDic<> baseDic;
	baseDic.initConstruction();
	baseDic.addToken( L"abra", 0 );
	baseDic.addToken( L"abracadabra", 0 );
	baseDic.finishConstruction();
	Val val( baseDic, "../csl/Val/Test/patterns.borders.txt" );
	
	Val::CandidateReceiver answers;
	
	// // This is the word as it is in the dic
	// val.query( L"abracadabra", &answers  );
	// CPPUNIT_ASSERT_EQUAL( (size_t) 1, (size_t) answers.size() );
	// CPPUNIT_ASSERT( answers.at(0).getWord() == L"abracadabra" );

	// // wordBegin pattern works at wordBegin : ^ab:beb
	// answers.clear();
	// val.query( L"bebracadabra", &answers  );
	// CPPUNIT_ASSERT_EQUAL( (size_t) 1, (size_t) answers.size() );
	// CPPUNIT_ASSERT( answers.at(0).getBaseWord() == L"abracadabra" );
	// CPPUNIT_ASSERT( answers.at(0).getWord() == L"bebracadabra" );
	
	// // wordBegin pattern does NOT work in the middle
	// answers.clear();
	// val.query( L"abracadbebra", &answers  );
	// CPPUNIT_ASSERT_EQUAL( (size_t) 0, (size_t) answers.size() );
	
	// // standard patterns works also at beginning and end
	// answers.clear();
	// val.query( L"zebracadabra", &answers  );
	// CPPUNIT_ASSERT_EQUAL( (size_t) 1, (size_t) answers.size() );
	// CPPUNIT_ASSERT( answers.at(0).getBaseWord() == L"abracadabra" );
	// CPPUNIT_ASSERT( answers.at(0).getWord() == L"zebracadabra" );

	// answers.clear();
	// val.query( L"abracadzebra", &answers  );
	// CPPUNIT_ASSERT_EQUAL( (size_t) 1, (size_t) answers.size() );
	// CPPUNIT_ASSERT( answers.at(0).getBaseWord() == L"abracadabra" );
	// CPPUNIT_ASSERT( answers.at(0).getWord() == L"abracadzebra" );

	// wordEnd pattern works at the end
	answers.clear();
	val.query( L"abracadabpo", &answers  );
	CPPUNIT_ASSERT_EQUAL( (size_t)1, (size_t)answers.size() );
	CPPUNIT_ASSERT( answers.at(0).getBaseWord() == L"abracadabra" );
	CPPUNIT_ASSERT( answers.at(0).getWord() == L"abracadabpo" );
	
	// wordEnd pattern works at the end of word that is prefix of another one.
	answers.clear();
	val.query( L"abpo", &answers  );
	CPPUNIT_ASSERT_EQUAL( (size_t)1, (size_t)answers.size() );
	CPPUNIT_ASSERT( answers.at(0).getBaseWord() == L"abra" );
	

	// wordEnd pattern does NOT work in the middle
	answers.clear();
	val.query( L"abpocadabra", &answers  );
	CPPUNIT_ASSERT_EQUAL( (size_t)0, (size_t)answers.size() );


	// standard pattern that has wordEnd pattern as prefix works
	answers.clear();
	val.query( L"abtakadabra", &answers  );
	CPPUNIT_ASSERT_EQUAL( (size_t)1, (size_t)answers.size() );
	CPPUNIT_ASSERT( answers.at(0).getBaseWord() == L"abracadabra" );
	CPPUNIT_ASSERT( answers.at(0).getWord() == L"abtakadabra" );

	// this refers to an insertion at wordend: $:sim
	answers.clear();
	val.query( L"abracadabrasim", &answers  );
	CPPUNIT_ASSERT_EQUAL( (size_t)1, (size_t)answers.size() );
	CPPUNIT_ASSERT( answers.at(0).getBaseWord() == L"abracadabra" );
	CPPUNIT_ASSERT( answers.at(0).getWord() == L"abracadabrasim" );

	// this refers to a deletion at wordend: $bra:$
	answers.clear();
	val.query( L"abracada", &answers  );
	CPPUNIT_ASSERT_EQUAL( (size_t)1, (size_t)answers.size() );
	CPPUNIT_ASSERT( answers.at(0).getBaseWord() == L"abracadabra" );
	CPPUNIT_ASSERT( answers.at(0).getWord() == L"abracada" );


	// this refers to an insertion at wordBegin: ^:^rumb
	answers.clear();
	val.query( L"rumbabracadabra", &answers  );
	CPPUNIT_ASSERT_EQUAL( (size_t)1, (size_t)answers.size() );
	CPPUNIT_ASSERT( answers.at(0).getBaseWord() == L"abracadabra" );
	CPPUNIT_ASSERT( answers.at(0).getWord() == L"rumbabracadabra" );

	// this refers to a deletion at wordBegin: ^abr:^
	answers.clear();
	val.query( L"acadabra", &answers  );
	CPPUNIT_ASSERT_EQUAL( (size_t)1, (size_t)answers.size() );
	CPPUNIT_ASSERT( answers.at(0).getBaseWord() == L"abracadabra" );
	CPPUNIT_ASSERT( answers.at(0).getWord() == L"acadabra"  );

	
    }

} // namespace csl


#endif
