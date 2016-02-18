#ifndef CSL_TEST_PATTERN_H
#define CSL_TEST_PATTERN_H CSL_TEST_PATTERN_H

#include "../Pattern.h"
#include "../PatternSet.h"
#include "../PosPattern.h"
#include "../Interpretation.h"
#include "../PatternProbabilities.h"
#include "../ComputeInstruction.h"
#include "../PatternGraph.h"

#include <cppunit/extensions/HelperMacros.h>


namespace csl {
    /**
     * This test class subsumes tests for the classes Pattern, PosPattern and Instruction
     * @author Ulrich Reffle, 2008
     */
    class TestPattern : public CppUnit::TestFixture  {

  	CPPUNIT_TEST_SUITE( TestPattern );
	CPPUNIT_TEST( testPattern );
	CPPUNIT_TEST( testPatternSet );
	CPPUNIT_TEST( testPosPattern );
	CPPUNIT_TEST( testInstruction );
	CPPUNIT_TEST( testInterpretation );
	CPPUNIT_TEST( testPatternProbabilities );
	CPPUNIT_TEST( testPatternGraph );
	CPPUNIT_TEST_SUITE_END();
    public:

	void testPattern();
	void testPatternSet();
	void testPosPattern();
	void testInstruction();
	void testInterpretation();
	void testPatternProbabilities();
	void testPatternGraph();

	void run();
    private:

    }; // class TestPattern

    CPPUNIT_TEST_SUITE_REGISTRATION( TestPattern );


    void TestPattern::run() {
  	testPattern();
  	testPatternSet();
  	testPosPattern();
 	testInstruction();
 	testInterpretation();
  	testPatternProbabilities();
	testPatternGraph();
    }

    /**
     * test the basic methods of class Pattern.
     */
    void TestPattern::testPattern() {
	// test constructor and the getters.
	Pattern p1( L"left", L"right" );
	CPPUNIT_ASSERT( p1.getLeft() == L"left" );
	CPPUNIT_ASSERT( p1.getRight() == L"right" );

	Pattern p2( L"xy", L"x y" );
	CPPUNIT_ASSERT( p2.getLeft() == L"xy" );
	CPPUNIT_ASSERT( p2.getRight() == L"x y" );

    }

    void TestPattern::testPatternSet() {
	PatternSet pSet;
	pSet.loadPatterns( "../csl/Pattern/Test/small.patterns.txt" );

	PatternSet::const_iterator pat = pSet.begin();

 	CPPUNIT_ASSERT( ( pat->getLeft() == L"a" ) && ( pat->getRight() == L"x" ) );
	++pat;
	CPPUNIT_ASSERT( ( pat->getLeft() == L"ei" ) && ( pat->getRight() == L"ey" )  );
	++pat;
	CPPUNIT_ASSERT( ( pat->getLeft() == L"k" ) && ( pat->getRight() == L"h" )  );
	++pat;
	CPPUNIT_ASSERT( ( pat->getLeft() == L"t" ) && ( pat->getRight() == L"th" )  );
	++pat;
	CPPUNIT_ASSERT( ( pat->getLeft() == L"t" ) && ( pat->getRight() == L"x" )  );
	++pat;
	CPPUNIT_ASSERT( ( pat->getLeft() == L"xy" ) && ( pat->getRight() == L"x y" )  );
	++pat;

	CPPUNIT_ASSERT( pat == pSet.end()  );


    }

    void TestPattern::testPosPattern() {

	// construct PosPattern from existing Pattern
	PosPattern pp1( L"left", L"right", 3 );
	CPPUNIT_ASSERT( pp1.getLeft() == L"left" );
	CPPUNIT_ASSERT( pp1.getRight() == L"right" );
	CPPUNIT_ASSERT( pp1.getPosition() == 3 );

	std::wstring str = pp1.toString();
	CPPUNIT_ASSERT( str == L"(left_right,3)" );

	PosPattern pp2;
	pp2.parseFromString( str );
	CPPUNIT_ASSERT( pp2.getLeft() == L"left" );
	CPPUNIT_ASSERT( pp2.getRight() == L"right" );
	CPPUNIT_ASSERT( pp2.getPosition() == 3 );

	// if posPattern is not at the beginning of the str
	str = L"bliblablu(left_right,3)bla";

	PosPattern pp3;
	size_t end_of_pp = pp3.parseFromString( str, 9 );
	CPPUNIT_ASSERT( pp3.getLeft() == L"left" );
	CPPUNIT_ASSERT( pp3.getRight() == L"right" );
	CPPUNIT_ASSERT( pp3.getPosition() == 3 );
	CPPUNIT_ASSERT( end_of_pp == 23 );

	// in a realistic Instruction
	str = L"[(left_right,3)(le_ri,42)]";

	PosPattern pp4;
	end_of_pp = pp4.parseFromString( str, 1 );
	CPPUNIT_ASSERT( pp4.getLeft() == L"left" );
	CPPUNIT_ASSERT( pp4.getRight() == L"right" );
	CPPUNIT_ASSERT( pp4.getPosition() == 3 );
	CPPUNIT_ASSERT( end_of_pp == 15 );

	PosPattern pp5;
	end_of_pp = pp5.parseFromString( str, 15 );
	CPPUNIT_ASSERT( pp5.getLeft() == L"le" );
	CPPUNIT_ASSERT( pp5.getRight() == L"ri" );
	CPPUNIT_ASSERT( pp5.getPosition() == 42 );
	CPPUNIT_ASSERT( end_of_pp == 25 );


    }

    void TestPattern::testInstruction() {
	Instruction instr;
	instr.push_back( PosPattern( L"left", L"right", 3 ) );
	instr.push_back( PosPattern( L"le", L"ri", 42 ) );

	CPPUNIT_ASSERT( instr.size() == 2 );
	CPPUNIT_ASSERT( instr.at( 0 ).getLeft() == L"left" );
	CPPUNIT_ASSERT( instr.at( 0 ).getRight() == L"right" );
	CPPUNIT_ASSERT( instr.at( 0 ).getPosition() == 3 );
	CPPUNIT_ASSERT( instr.at( 1 ).getLeft() == L"le" );
	CPPUNIT_ASSERT( instr.at( 1 ).getRight() == L"ri" );
	CPPUNIT_ASSERT( instr.at( 1 ).getPosition() == 42 );

	std::wstring str = instr.toString();
	std::wcout << str << std::endl;
	CPPUNIT_ASSERT( str == L"[(left_right,3)(le_ri,42)]" );

	Instruction instr2;
	instr2.parseFromString( str, 0 );
	CPPUNIT_ASSERT( instr2.size() == 2 );
	CPPUNIT_ASSERT( instr2.at( 0 ).getLeft() == L"left" );
	CPPUNIT_ASSERT( instr2.at( 0 ).getRight() == L"right" );
	CPPUNIT_ASSERT( instr2.at( 0 ).getPosition() == 3 );
	CPPUNIT_ASSERT( instr2.at( 1 ).getLeft() == L"le" );
	CPPUNIT_ASSERT( instr2.at( 1 ).getRight() == L"ri" );
	CPPUNIT_ASSERT( instr2.at( 1 ).getPosition() == 42 );

	// parse empty instruction
	Instruction instr3;
	instr3.parseFromString( L"[]", 0 );
	CPPUNIT_ASSERT( instr3.size() == 0 );

    }

    void TestPattern::testInterpretation() {
	Interpretation interp;
	interp.setWord( L"theil" );
	interp.setBaseWord( L"teil" );
	interp.getHistInstruction().push_back( csl::PosPattern( L"t", L"th", 0 ) );
	interp.setLevDistance( 1 );

	std::wstring str = interp.toString();
	std::wcout << str << std::endl;
	CPPUNIT_ASSERT( str == L"theil:teil+[(t_th,0)],dist=1" );

	interp.clear();
	interp.parseFromString( str, 0 );
	CPPUNIT_ASSERT( interp.getWord() == L"theil" );
	CPPUNIT_ASSERT( interp.getBaseWord() == L"teil" );
	CPPUNIT_ASSERT( interp.getBaseWordScore() == size_t(-1) );
	CPPUNIT_ASSERT( interp.getLevDistance() == 1 );
	CPPUNIT_ASSERT( interp.getHistInstruction().size() == 1 );
	CPPUNIT_ASSERT( interp.getHistInstruction().at( 0 ).getLeft() == L"t" );
	CPPUNIT_ASSERT( interp.getHistInstruction().at( 0 ).getRight() == L"th" );
	CPPUNIT_ASSERT( interp.getHistInstruction().at( 0 ).getPosition() == 0 );
    }

    void TestPattern::testPatternProbabilities() {
	PatternProbabilities pp;
	CPPUNIT_ASSERT( pp.getWeight( Pattern( L"t", L"th" ) ) == PatternProbabilities::UNDEF );
	pp.setWeight( Pattern( L"t", L"th" ), 0.35 );
	CPPUNIT_ASSERT( pp.getWeight( Pattern( L"t", L"th" ) ) == static_cast< float >( 0.35 ) );

	pp.setDefault( PatternProbabilities::PatternType( 1, 2 ), 1.3 );

	CPPUNIT_ASSERT( pp.getWeight( Pattern( L"t", L"th" ) ) == static_cast< float >( 0.35 ) ); // as before
	CPPUNIT_ASSERT( pp.getWeight( Pattern( L"x", L"yz" ) ) == static_cast< float >( 1.3 ) ); // default value
	CPPUNIT_ASSERT( pp.getWeight( Pattern( L"xy", L"z" ) ) == PatternProbabilities::UNDEF ); // as before

	pp.clear();
	CPPUNIT_ASSERT( pp.getWeight( Pattern( L"t", L"th" ) ) == PatternProbabilities::UNDEF );
	CPPUNIT_ASSERT( pp.getWeight( Pattern( L"x", L"yz" ) ) == PatternProbabilities::UNDEF );
	CPPUNIT_ASSERT( pp.getWeight( Pattern( L"xy", L"z" ) ) == PatternProbabilities::UNDEF );


    }



    void TestPattern::testPatternGraph() {
	std::wcout << "TestPattern::TestPatternGraph()" << std::endl;

	PatternGraph pg;
	pg.loadPatterns( "../csl/Val/Test/small.patterns.txt" );

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


	/////  Forward, index right
	PatternGraph pg2( PatternGraph::FORWARD, PatternGraph::INDEX_RIGHT );
	pg2.loadPatterns( "../csl/Val/Test/small.patterns.txt" );
	pg2.toDot();

	st = pg2.getRoot();


    }

} // namespace csl


#endif
