#ifndef CSL_TESTMINDIC_H
#define CSL_TESTMINDIC_H CSL_TESTMINDIC_H

#include "../../Global.h"
#include "../FBDicString.h"

#include <cppunit/extensions/HelperMacros.h>


namespace csl {
    class TestFBDicString : public CppUnit::TestFixture  {

	CPPUNIT_TEST_SUITE( TestFBDicString );
	CPPUNIT_TEST( testBasics );
	CPPUNIT_TEST_SUITE_END();
    public:
	TestFBDicString();

	void run();

	void testBasics();


    };

    CPPUNIT_TEST_SUITE_REGISTRATION( TestFBDicString );

    TestFBDicString::TestFBDicString() {
    }

    void TestFBDicString::run() {
	testBasics();
    }
    

    void TestFBDicString::testBasics() {
	csl::FBDicString dic;
	dic.compileDic( "../csl/FBDicString/Test/test.lex" ); 

    }

} // namespace csl


#endif
