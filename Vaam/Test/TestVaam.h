#ifndef CSL_TEST_VAAM_H
#define CSL_TEST_VAAM_H CSL_TEST_VAAM_H

#include "../../Global.h"
#include "../Vaam.h"

#include <cppunit/extensions/HelperMacros.h>


namespace csl {
    class TestVaam : public CppUnit::TestFixture  {

	CPPUNIT_TEST_SUITE( TestVaam );
	CPPUNIT_TEST( testPatternGraph );
	CPPUNIT_TEST( testBasics );
	CPPUNIT_TEST( testCaseMode );
	CPPUNIT_TEST( testWordBorders );
	CPPUNIT_TEST_SUITE_END();
    public:
	TestVaam();
	void testBasics();
	void testPatternGraph();
// 	void testConstructionDetails();
// 	void lookupAllKeys();
	void testCaseMode();
	void testWordBorders();
	
    private:
	std::string path_;

    }; // class TestVaam

    CPPUNIT_TEST_SUITE_REGISTRATION( TestVaam );

    TestVaam::TestVaam() :
	CppUnit::TestFixture() {

    }

    void TestVaam::testPatternGraph() {
	std::wcout << "*** TestVaam::testPatternGraph ***" << std::endl;

	PatternGraph pg;
	pg.loadPatterns( "../csl/Vaam/Test/small.patterns.txt" );

	// implicit copy constructor
	PatternGraph::State st = pg.getRoot();
	PatternGraph::State st2 = pg.getRoot();

	// == operator
	CPPUNIT_ASSERT( st == st2 );

	// basic walk()-operation and isFinal()
	CPPUNIT_ASSERT( ! st.isFinal() );
	CPPUNIT_ASSERT( st.walk( 't' ) );
	CPPUNIT_ASSERT( st.isFinal() );

	st = pg.getRoot();
	PatternGraph::State st3( st );
	st2 = st.getTransTarget( 't' );
	CPPUNIT_ASSERT( st == pg.getRoot() );
	
    }

    /**
     * test the basic methods for reading access like getRoot, walk, isFinal etc.
     */
    void TestVaam::testBasics() {
	std::wcout << "*** TestVaam::testBasics ***" << std::endl;
 	MinDic<> baseDic( "../csl/Vaam/Test/small.base.mdic" );
	MinDic<> filterDic( "../csl/Vaam/Test/small.filter.mdic" );
	Vaam<> vaam( baseDic,  "../csl/Vaam/Test/small.patterns.txt" );
	
	Vaam<>::CandidateReceiver answers;

	// a standard variant
	CPPUNIT_ASSERT( vaam.query( std::wstring( L"xachen" ), &answers ) );
	CPPUNIT_ASSERT_EQUAL( (size_t) 1, (size_t) answers.size() );
	Interpretation& answer = answers.at( 0 ); 
	CPPUNIT_ASSERT( answer.getBaseWord() == L"aachen" );
	CPPUNIT_ASSERT( answer.getInstruction().size() == 1 );
	CPPUNIT_ASSERT( answer.getInstruction().at( 0 ).getLeft() == L"a" );
	CPPUNIT_ASSERT( answer.getInstruction().at( 0 ).getRight() == L"x" );

	answers.clear();

	///////  space in pattern

	CPPUNIT_ASSERT( vaam.query( std::wstring( L"uvwx yz" ), &answers ) );
	CPPUNIT_ASSERT_EQUAL( (size_t) 1, (size_t) answers.size() );
	Interpretation& answer2 = answers.at( 0 ); 
	CPPUNIT_ASSERT( answer2.getBaseWord() == L"uvwxyz" );
	CPPUNIT_ASSERT( answer2.getInstruction().size() == 1 );
	CPPUNIT_ASSERT( answer2.getInstruction().at( 0 ).getLeft() == L"xy" );
	CPPUNIT_ASSERT( answer2.getInstruction().at( 0 ).getRight() == L"x y" );

	answers.clear();




	///////////// MAX NR OF PATTERNS ///////////////////////
	// with at most 0 variants, this should be no variant
	vaam.setMaxNrOfPatterns( 0 );
	CPPUNIT_ASSERT( ! vaam.query( std::wstring( L"xachen" ), &answers ) );
	// with at most 1 variant, this should work again
	vaam.setMaxNrOfPatterns( 1 );
	CPPUNIT_ASSERT( vaam.query( std::wstring( L"xachen" ), &answers ) );
	// but not a word with 2 patterns
	CPPUNIT_ASSERT( ! vaam.query( std::wstring( L"kleintheyle" ), &answers ) );

	// switch back to an infinite nr of patterns
	vaam.setMaxNrOfPatterns( Vaam<>::INFINITE );
	CPPUNIT_ASSERT( vaam.query( std::wstring( L"kleintheyle" ), &answers ) );


	///////////// MIN NR OF PATTERNS ///////////////////////

	// default should be 0
	CPPUNIT_ASSERT( vaam.query( std::wstring( L"aachen" ), &answers ) );

	vaam.setMinNrOfPatterns( 1 );
	CPPUNIT_ASSERT( ! vaam.query( std::wstring( L"aachen" ), &answers ) );
	
	vaam.setMinNrOfPatterns( 2 );
	CPPUNIT_ASSERT( ! vaam.query( std::wstring( L"xachen" ), &answers ) );
	CPPUNIT_ASSERT( vaam.query( std::wstring( L"xxchen" ), &answers ) );
	
	vaam.setMinNrOfPatterns( 0 );


	///////// FILTER DIC ///////////////////////////////

	// without the filterDic_ this should be a variant
	CPPUNIT_ASSERT( vaam.query( std::wstring( L"hanne" ), &answers ) );

	// now it should be filtered
	vaam.setFilterDic( filterDic );
	CPPUNIT_ASSERT( ! vaam.query( std::wstring( L"hanne" ), &answers ) );
    }

    void TestVaam::testCaseMode() {
	std::wcout << "*** TestVaam::testCaseMode ***" << std::endl;
 	MinDic<> baseDic( "../csl/Vaam/Test/small.base.mdic" );
	Vaam<> vaam( baseDic,  "../csl/Vaam/Test/small.patterns.txt" );
	
	Vaam<>::CandidateReceiver answers;
	
	vaam.setCaseMode( Global::restoreCase );
	
	vaam.query( L"Kleintheyle", &answers  );
	CPPUNIT_ASSERT_EQUAL( (size_t) 1, (size_t) answers.size() );
	Interpretation& answer = answers.at( 0 ); 
	CPPUNIT_ASSERT( answer.getWord() == L"Kleintheyle" );
	CPPUNIT_ASSERT( answer.getBaseWord() == L"Kleinteile" );

    }


    void TestVaam::testWordBorders() {
	std::wcout << "*** TestVaam::testWordBorders ***" << std::endl;
 	MinDic<> baseDic;
	baseDic.initConstruction();
	baseDic.addToken( L"abra", 0 );
	baseDic.addToken( L"abracadabra", 0 );
	baseDic.finishConstruction();
	Vaam<> vaam( baseDic, "../csl/Vaam/Test/patterns.borders.txt" );
	
	Vaam<>::CandidateReceiver answers;
	
	// this is the word as it is in the dic
	vaam.query( L"abracadabra", &answers  );
	CPPUNIT_ASSERT_EQUAL( (size_t) 1, (size_t) answers.size() );
	CPPUNIT_ASSERT( answers.at(0).getWord() == L"abracadabra" );

	// wordBegin pattern works at wordBegin : ^ab:beb
	answers.clear();
	vaam.query( L"bebracadabra", &answers  );
	CPPUNIT_ASSERT_EQUAL( (size_t) 1, (size_t) answers.size() );
	CPPUNIT_ASSERT( answers.at(0).getBaseWord() == L"abracadabra" );
	CPPUNIT_ASSERT( answers.at(0).getWord() == L"bebracadabra" );

	// wordBegin pattern does NOT work in the middle
	answers.clear();
	vaam.query( L"abracadbebra", &answers  );
	CPPUNIT_ASSERT_EQUAL( (size_t) 0, (size_t) answers.size() );
	
	// standard patterns works also at beginning and end
	answers.clear();
	vaam.query( L"zebracadabra", &answers  );
	CPPUNIT_ASSERT_EQUAL( (size_t) 1, (size_t) answers.size() );
	CPPUNIT_ASSERT( answers.at(0).getBaseWord() == L"abracadabra" );
	CPPUNIT_ASSERT( answers.at(0).getWord() == L"zebracadabra" );

	answers.clear();
	vaam.query( L"abracadzebra", &answers  );
	CPPUNIT_ASSERT_EQUAL( (size_t) 1, (size_t) answers.size() );
	CPPUNIT_ASSERT( answers.at(0).getBaseWord() == L"abracadabra" );
	CPPUNIT_ASSERT( answers.at(0).getWord() == L"abracadzebra" );

	// wordEnd pattern works at the end
	answers.clear();
	vaam.query( L"abracadabpo", &answers  );
	CPPUNIT_ASSERT_EQUAL( (size_t)1, (size_t)answers.size() );
	CPPUNIT_ASSERT( answers.at(0).getBaseWord() == L"abracadabra" );
	CPPUNIT_ASSERT( answers.at(0).getWord() == L"abracadabpo" );
	
	// wordEnd pattern works at the end of word that is prefix of another one.
	answers.clear();
	vaam.query( L"abpo", &answers  );
	CPPUNIT_ASSERT_EQUAL( (size_t)1, (size_t)answers.size() );
	CPPUNIT_ASSERT( answers.at(0).getBaseWord() == L"abra" );
	
	// wordEnd pattern does NOT work in the middle
	answers.clear();
	vaam.query( L"abpocadabra", &answers  );
	CPPUNIT_ASSERT_EQUAL( (size_t)0, (size_t)answers.size() );


	// standard pattern that has wordEnd pattern as prefix works
	answers.clear();
	vaam.query( L"abtakadabra", &answers  );
	CPPUNIT_ASSERT_EQUAL( (size_t)1, (size_t)answers.size() );
	CPPUNIT_ASSERT( answers.at(0).getBaseWord() == L"abracadabra" );
	CPPUNIT_ASSERT( answers.at(0).getWord() == L"abtakadabra" );

	// this refers to an insertion at wordend: $:sim
	answers.clear();
	vaam.query( L"abracadabrasim", &answers  );
	CPPUNIT_ASSERT_EQUAL( (size_t)1, (size_t)answers.size() );
	CPPUNIT_ASSERT( answers.at(0).getBaseWord() == L"abracadabra" );
	CPPUNIT_ASSERT( answers.at(0).getWord() == L"abracadabrasim" );

	// this refers to a deletion at wordend: $bra:
	answers.clear();
	vaam.query( L"abracada", &answers  );
	CPPUNIT_ASSERT_EQUAL( (size_t)1, (size_t)answers.size() );
	CPPUNIT_ASSERT( answers.at(0).getBaseWord() == L"abracadabra" );
	CPPUNIT_ASSERT( answers.at(0).getWord() == L"abracada" );


	// this refers to an insertion at wordBegin: ^:rumb
	answers.clear();
	vaam.query( L"rumbabracadabra", &answers  );
	CPPUNIT_ASSERT_EQUAL( (size_t)1, (size_t)answers.size() );
	CPPUNIT_ASSERT( answers.at(0).getBaseWord() == L"abracadabra" );
	CPPUNIT_ASSERT( answers.at(0).getWord() == L"rumbabracadabra" );

	// this refers to a deletion at wordBegin: ^abr:
	answers.clear();
	vaam.query( L"acadabra", &answers  );
	CPPUNIT_ASSERT_EQUAL( (size_t)1, (size_t)answers.size() );
	CPPUNIT_ASSERT( answers.at(0).getBaseWord() == L"abracadabra" );
	CPPUNIT_ASSERT( answers.at(0).getWord() == L"acadabra"  );




	
    }


} // namespace csl


#endif
