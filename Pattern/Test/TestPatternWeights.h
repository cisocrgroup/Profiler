#ifndef CSL_TEST_PATTERN_H
#define CSL_TEST_PATTERN_H CSL_TEST_PATTERN_H

#include "../Pattern.h"
#include "../PatternSet.h"
#include "../PosPattern.h"
#include "../PatternWeights.h"
#include "../ComputeInstruction.h"

#include <cppunit/extensions/HelperMacros.h>


namespace csl {
    /**
     * This test class tests class PatternWeights
     * @author Ulrich Reffle, 2009
     */
    class TestPatternWeights : public CppUnit::TestFixture  {

	CPPUNIT_TEST_SUITE( TestPatternWeights );
	CPPUNIT_TEST( testBasics );
	CPPUNIT_TEST( testFileDump );
	CPPUNIT_TEST_SUITE_END();
    public:
	void testBasics();
	void testFileDump();

    private:
	
    }; // class TestPatternWeights

    CPPUNIT_TEST_SUITE_REGISTRATION( TestPatternWeights );


    void TestPatternWeights::testBasics() {
	PatternWeights pw;
	CPPUNIT_ASSERT( pw.getWeight( Pattern( L"t", L"th" ) ) == PatternWeights::UNDEF );
	pw.setWeight( Pattern( L"t", L"th" ), 0.35 );
	CPPUNIT_ASSERT( pw.getWeight( Pattern( L"t", L"th" ) ) == static_cast< float >( 0.35 ) );
	
	pw.setDefault( PatternWeights::PatternType( 1, 2 ), 1.3 );
	
	CPPUNIT_ASSERT( pw.getWeight( Pattern( L"t", L"th" ) ) == static_cast< float >( 0.35 ) ); // as before
	CPPUNIT_ASSERT( pw.getWeight( Pattern( L"x", L"yz" ) ) == static_cast< float >( 1.3 ) ); // default value
	CPPUNIT_ASSERT( pw.getWeight( Pattern( L"xy", L"z" ) ) == PatternWeights::UNDEF ); // as before

	pw.reset();
	CPPUNIT_ASSERT( pw.getWeight( Pattern( L"t", L"th" ) ) == PatternWeights::UNDEF );
	CPPUNIT_ASSERT( pw.getWeight( Pattern( L"x", L"yz" ) ) == PatternWeights::UNDEF );
	CPPUNIT_ASSERT( pw.getWeight( Pattern( L"xy", L"z" ) ) == PatternWeights::UNDEF );
    }

    void TestPatternWeights::testFileDump() {
	PatternWeights pw;
	pw.setWeight( Pattern( L"t", L"th" ), 0.35 );
	pw.setWeight( Pattern( L"i", L"y" ), 0.36 );
	pw.setWeight( Pattern( L"a", L"b" ), 0.37 );
	pw.setWeight( Pattern( L"a", L"ä" ), 0.38 );
	pw.setWeight( Pattern( L"ö", L"o" ), 0.39 );

	pw.writeToFile( "test.weights.txt" );

	PatternWeights pw2;
	try {
	    pw2.loadFromFile( "test.weights.txt" );
	}
	catch( std::exception ) {
	    CPPUNIT_FAIL( "Unexpected exception" );
	}
	CPPUNIT_ASSERT( pw.getWeight( Pattern( L"t", L"th" ) ) == static_cast< float >( 0.35 ) );
	CPPUNIT_ASSERT( pw.getWeight( Pattern( L"i", L"y" ) ) == static_cast< float >( 0.36 ) );
	CPPUNIT_ASSERT( pw.getWeight( Pattern( L"a", L"b" ) ) == static_cast< float >( 0.37 ) );
	CPPUNIT_ASSERT( pw.getWeight( Pattern( L"a", L"ä" ) ) == static_cast< float >( 0.38 ) );
	CPPUNIT_ASSERT( pw.getWeight( Pattern( L"ö", L"o" ) ) == static_cast< float >( 0.39 ) );

	// check behaviour for wrong filepaths
	PatternWeights pw3;
	try {
	    pw3.loadFromFile( "test.weights.txt" );
	    CPPUNIT_FAIL( "This should throw an exception" );
	}
	catch( std::exception ) {
	}

    }



} // namespace csl


#endif
