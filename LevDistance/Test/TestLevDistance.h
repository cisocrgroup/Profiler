#ifndef CSL_TESTLEVDISTANCE_H
#define CSL_TESTLEVDISTANCE_H CSL_TESTLEVDISTANCE_H

#include "../../Global.h"
#include "../LevDistance.h"

#include <cppunit/extensions/HelperMacros.h>


namespace csl {
    class TestLevDistance : public CppUnit::TestFixture  {

	CPPUNIT_TEST_SUITE( TestLevDistance );
	CPPUNIT_TEST( testBasics );
	CPPUNIT_TEST_SUITE_END();
    public:
	TestLevDistance();

	void run();

	void testBasics();


	
    };

    CPPUNIT_TEST_SUITE_REGISTRATION( TestLevDistance );

    TestLevDistance::TestLevDistance() {
    }

    void TestLevDistance::run() {
	testBasics();
    }
    

    /**
     * test the basic methods
     */
    void TestLevDistance::testBasics() {
	LevDistance ld;

	CPPUNIT_ASSERT_EQUAL( (size_t)1, ld.getLevDistance( L"apfel", L"apfxl" ) );

	CPPUNIT_ASSERT_EQUAL( (size_t)1, ld.getLevDistance( L"apfel", L"xpfel" ) );
	CPPUNIT_ASSERT_EQUAL( (size_t)1, ld.getLevDistance( L"xpfxl", L"xpfel" ) );

	CPPUNIT_ASSERT_EQUAL( (size_t)1, ld.getLevDistance( L"apfel", L"pfel" ) );

	CPPUNIT_ASSERT_EQUAL( (size_t)2, ld.getLevDistance( L"apfel", L"pfxl" ) );

	CPPUNIT_ASSERT_EQUAL( (size_t)1, ld.getLevDistance( L"apfel", L"apfexl" ) );

	CPPUNIT_ASSERT_EQUAL( (size_t)5, ld.getLevDistance( L"apfel", L"" ) );
	CPPUNIT_ASSERT_EQUAL( (size_t)5, ld.getLevDistance( L"", L"apfel" ) );

	CPPUNIT_ASSERT_EQUAL( (size_t)2, ld.getLevDistance( L"durchleüchtigisten", L"duxchleuchtigisten" ) );

    }


} // namespace csl


#endif
