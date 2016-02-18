#ifndef CSL_TEST_PATTERN_H
#define CSL_TEST_PATTERN_H CSL_TEST_PATTERN_H

#include "../ComputeInstruction.h"

#include <cppunit/extensions/HelperMacros.h>


namespace csl {
    /**
     * This test class subsumes tests for the classes Pattern, PosPattern and Instruction
     * @author Ulrich Reffle, 2008
     */
    class TestComputeInstruction : public CppUnit::TestFixture  {

  	CPPUNIT_TEST_SUITE( TestComputeInstruction );
	CPPUNIT_TEST( testBasics );
	CPPUNIT_TEST( testProfilerSetting );
	CPPUNIT_TEST_SUITE_END();
    public:

	void testBasics();
	void testProfilerSetting();

	void run();
    private:
	
    }; // class TestPattern

    CPPUNIT_TEST_SUITE_REGISTRATION( TestComputeInstruction );

    
    void TestComputeInstruction::run() {
	testBasics();
	testProfilerSetting();
    }



    void TestComputeInstruction::testBasics() {
	ComputeInstruction ci;
	ci.setDebug( 1 );
	PatternProbabilities pp;
	ci.connectPatternProbabilities( pp );

	std::vector< Instruction > instructions;
	

	// all operations are tested separately


	// sub
	pp.clear();
	pp.setWeight( Pattern( L"x", L"y" ), 0.0001 );
	CPPUNIT_ASSERT( ci.computeInstruction( L"abxcd", L"abycd", &instructions ) ); // 1 sub
	CPPUNIT_ASSERT( instructions.size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 0 ) == PosPattern( L"x", L"y", 2 ) );


	// insert
	pp.clear();
	instructions.clear();
	pp.setWeight( Pattern( L"", L"x" ), 0.0001 );
	CPPUNIT_ASSERT( ci.computeInstruction( L"muh", L"mxuh", &instructions ) ); // 1 ins
	CPPUNIT_ASSERT( instructions.size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 0 ) == PosPattern( L"", L"x", 1 ) );

	// delete
	pp.clear();
	instructions.clear();
	pp.setWeight( Pattern( L"x", L"" ), 0.0001 );
	CPPUNIT_ASSERT( ci.computeInstruction( L"mxuh", L"muh", &instructions ) ); // 1 del
	CPPUNIT_ASSERT( instructions.size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 0 ) == PosPattern( L"x", L"", 1 ) );

	// merge
	pp.clear();
	instructions.clear();
	pp.setWeight( Pattern( L"xy", L"z" ), 0.0001 );
	CPPUNIT_ASSERT( ci.computeInstruction( L"mxyuh", L"mzuh", &instructions ) ); // 1 del
	CPPUNIT_ASSERT( instructions.size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 0 ) == PosPattern( L"xy", L"z", 1 ) );

	// split
	pp.clear();
	instructions.clear();
	pp.setWeight( Pattern( L"x", L"yz" ), 0.0001 );
	CPPUNIT_ASSERT( ci.computeInstruction( L"mxuh", L"myzuh", &instructions ) ); // 1 del
	CPPUNIT_ASSERT( instructions.size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 0 ) == PosPattern( L"x", L"yz", 1 ) );

	
	pp.clear();
	pp.setDefault( PatternProbabilities::PatternType( 0, 1 ), 0.00001 ); // standard ins
	pp.setDefault( PatternProbabilities::PatternType( 1, 0 ), 0.00001 ); // standard del
	pp.setDefault( PatternProbabilities::PatternType( 1, 1 ), 0.00001 ); // standard sub

	pp.setDefault( PatternProbabilities::PatternType( 2, 1 ), 0.00001 ); // standard merge
	pp.setDefault( PatternProbabilities::PatternType( 1, 2 ), 0.00001 ); // standard split
	
	instructions.clear();
	CPPUNIT_ASSERT( ci.computeInstruction( L"abcde", L"abxcde", &instructions ) );

	instructions.clear();
	CPPUNIT_ASSERT( ci.computeInstruction( L"abcde", L"axcdy", &instructions ) ); // 2 sub


	instructions.clear();
	CPPUNIT_ASSERT( ci.computeInstruction( L"muh", L"mxuh", &instructions ) ); // 1 ins
	std::wcout << "#####" << std::endl;
	for( std::vector< Instruction >::const_iterator it = instructions.begin(); it != instructions.end(); ++it ) {
	    it->print();std::wcout<<std::endl;
	}
	CPPUNIT_ASSERT_EQUAL( (size_t)3, instructions.size() );
	CPPUNIT_ASSERT( instructions.at( 0 ).size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 0 ) == PosPattern( L"", L"x", 1 ) );
	CPPUNIT_ASSERT( instructions.at( 1 ).size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 1 ).at( 0 ) == PosPattern( L"u", L"xu", 1 ) );
	CPPUNIT_ASSERT( instructions.at( 2 ).size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 2 ).at( 0 ) == PosPattern( L"m", L"mx", 0 ) );
	instructions.clear();


	CPPUNIT_ASSERT( ci.computeInstruction( L"muh", L"xmuh", &instructions ) ); // 1 ins at beginning of word

	CPPUNIT_ASSERT( instructions.size() == 2 );
	CPPUNIT_ASSERT( instructions.at( 0 ).size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 0 ) == PosPattern( L"", L"x", 0 ) );
	CPPUNIT_ASSERT( instructions.at( 1 ).size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 1 ).at( 0 ) == PosPattern( L"m", L"xm", 0 ) );
	instructions.clear();


	CPPUNIT_ASSERT( ci.computeInstruction( L"muh", L"mxh", &instructions ) ); // 1 sub
	CPPUNIT_ASSERT( instructions.size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 0 ) == PosPattern( L"u", L"x", 1 ) );
	instructions.clear();

	CPPUNIT_ASSERT( ci.computeInstruction( L"muh", L"xuh", &instructions ) ); // 1 sub at beginning of word
	CPPUNIT_ASSERT( instructions.size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 0 ) == PosPattern( L"m", L"x", 0 ) );
	instructions.clear();

	// deactivate the merges and splits again
	pp.setDefault( PatternProbabilities::PatternType( 2, 1 ), PatternProbabilities::UNDEF ); // standard merge
	pp.setDefault( PatternProbabilities::PatternType( 1, 2 ), PatternProbabilities::UNDEF ); // standard split

	CPPUNIT_ASSERT( ci.computeInstruction( L"milk", L"nrilk", &instructions ) ); // 1 ins, 1 sub
	CPPUNIT_ASSERT( instructions.size() == 2 );
	CPPUNIT_ASSERT( instructions.at( 0 ).size() == 2 );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 0 ) == PosPattern( L"", L"n", 0 ) );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 1 ) == PosPattern( L"m", L"r", 0 ) );
	CPPUNIT_ASSERT( instructions.at( 1 ).at( 0 ) == PosPattern( L"m", L"n", 0 ) );
	CPPUNIT_ASSERT( instructions.at( 1 ).at( 1 ) == PosPattern( L"", L"r", 1 ) );
	instructions.clear();

	CPPUNIT_ASSERT( ci.computeInstruction( L"murnau", L"mumau", &instructions )  ); // 1 del, 1 sub
	CPPUNIT_ASSERT( instructions.size() == 2 );
	CPPUNIT_ASSERT( instructions.at( 0 ).size() == 2 );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 0 ) == PosPattern( L"r", L"", 2 ) );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 1 ) == PosPattern( L"n", L"m", 3 ) );
	CPPUNIT_ASSERT( instructions.at( 1 ).at( 0 ) == PosPattern( L"r", L"m", 2 ) );
	CPPUNIT_ASSERT( instructions.at( 1 ).at( 1 ) == PosPattern( L"n", L"", 3 ) );
	instructions.clear();

	pp.setWeight( Pattern( L"m", L"rn" ), 0.35 );
	CPPUNIT_ASSERT( ci.computeInstruction( L"milk", L"rnilk", &instructions ) ); // 1 split
	CPPUNIT_ASSERT( instructions.size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 0 ) == PosPattern( L"m", L"rn", 0 ) );
	instructions.clear();

	CPPUNIT_ASSERT( ci.computeInstruction( L"milk", L"rnilkx", &instructions ) ); // 1 split, 1 insertion
	CPPUNIT_ASSERT( instructions.size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).size() == 2 );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 0 ) == PosPattern( L"m", L"rn", 0 ) );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 1 ) == PosPattern( L"", L"x", 4 ) );
	instructions.clear();

	pp.setWeight( Pattern( L"m", L"n" ), 0.54 );
	CPPUNIT_ASSERT( ci.computeInstruction( L"milkman", L"nilkrnan", &instructions ) ); // 1 sub 0.54, 1 split 0.35
	CPPUNIT_ASSERT( instructions.size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).size() == 2 );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 0 ) == PosPattern( L"m", L"n", 0 ) );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 1 ) == PosPattern( L"m", L"rn", 4 ) );


	// this is a setup that is used for the Profiler, for instance
	/// obviously the merges and splits give additional trouble
	pp.clear();
	pp.setDefault( PatternProbabilities::PatternType( 1, 1 ), 0.00001 );
	pp.setDefault( PatternProbabilities::PatternType( 1, 0 ), 0.00001 );
	pp.setDefault( PatternProbabilities::PatternType( 0, 1 ), 0.00001 );
	pp.setDefault( PatternProbabilities::PatternType( 2, 1 ), 0.00001 );
	pp.setDefault( PatternProbabilities::PatternType( 1, 2 ), 0.00001 );
	
	instructions.clear();
	CPPUNIT_ASSERT( ci.computeInstruction( L"muh", L"mxuh", &instructions ) ); // 1 ins inside the word

	std::wcout << "size is "<< instructions.size() << std::endl;

 	for( std::vector< Instruction >::const_iterator it = instructions.begin(); it != instructions.end(); ++it ) {
 	    it->print();std::wcout<<std::endl;
 	}

	CPPUNIT_ASSERT( instructions.size() == 3 );
	CPPUNIT_ASSERT( instructions.at( 0 ).size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 0 ) == PosPattern( L"", L"x", 1 ) );
	CPPUNIT_ASSERT( instructions.at( 1 ).size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 1 ).at( 0 ) == PosPattern( L"u", L"xu", 1 ) );
	CPPUNIT_ASSERT( instructions.at( 2 ).size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 2 ).at( 0 ) == PosPattern( L"m", L"mx", 0 ) );
	instructions.clear();

	CPPUNIT_ASSERT( ci.computeInstruction( L"muh", L"xmuh", &instructions ) ); // 1 ins at beginning
	CPPUNIT_ASSERT( instructions.size() == 2 );
	CPPUNIT_ASSERT( instructions.at( 0 ).size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 0 ) == PosPattern( L"", L"x", 0 ) );
	CPPUNIT_ASSERT( instructions.at( 1 ).size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 1 ).at( 0 ) == PosPattern( L"m", L"xm", 0 ) );
	instructions.clear();
	
	////// smartMerge //////
	// now those annoying pseudo-merges should disappear
	pp.setSmartMerge( true );
	
	CPPUNIT_ASSERT( ci.computeInstruction( L"muh", L"mxuh", &instructions ) ); // 1 ins inside the word
	CPPUNIT_ASSERT( instructions.size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 0 ) == PosPattern( L"", L"x", 1 ) );
	instructions.clear();
	
	CPPUNIT_ASSERT( ci.computeInstruction( L"muh", L"xmuh", &instructions ) ); // 1 ins at beginning
	CPPUNIT_ASSERT( instructions.size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).size() == 1 );
	CPPUNIT_ASSERT( instructions.at( 0 ).at( 0 ) == PosPattern( L"", L"x", 0 ) );
	instructions.clear();

	
	// What about spaces in words? Here's a real-world example
	CPPUNIT_ASSERT( ci.computeInstruction( L"eure churfuerstliche", L"fuerstltche", &instructions ) );
	instructions.clear();
	
	
	// use only sub as default operations
	pp.clear();
	pp.setDefault( PatternProbabilities::PatternType( 1, 1 ), 1 );
	
	CPPUNIT_ASSERT( ! ci.computeInstruction( L"muh", L"xmuh", &instructions ) ); // ins (not allowed)
	CPPUNIT_ASSERT( instructions.empty() );

	instructions.clear();
	
	CPPUNIT_ASSERT( ! ci.computeInstruction( L"galich", L"lich", &instructions )  ); // del (not allowed)
	CPPUNIT_ASSERT( instructions.empty() );
	
	instructions.clear();

	CPPUNIT_ASSERT( ! ci.computeInstruction( L"glich", L"galich", &instructions ) ); // ins (not allowed)
	CPPUNIT_ASSERT( instructions.empty() );
    }


    /**
     * This test might change with time. Here I test the profiler setup
     */
    void TestComputeInstruction::testProfilerSetting() {
	ComputeInstruction ci;
	ci.setDebug( 1 );
	PatternProbabilities pp;
	ci.connectPatternProbabilities( pp );
	pp.setDefault( csl::PatternWeights::PatternType( 1, 1 ), 1e-5 );
	pp.setDefault( csl::PatternWeights::PatternType( 2, 1 ), 1e-5 );
	pp.setDefault( csl::PatternWeights::PatternType( 1, 2 ), 1e-5 );
	pp.setDefault( csl::PatternWeights::PatternType( 0, 1 ), csl::PatternWeights::UNDEF );
	pp.setDefault( csl::PatternWeights::PatternType( 1, 0 ), 1e-5 );

	ci.computeInstruction( L"ickorell", L"corell" );	
	
	
    }


} // namespace csl


#endif
