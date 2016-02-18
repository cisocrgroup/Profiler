#ifndef CSL_TESTMINDIC_H
#define CSL_TESTMINDIC_H CSL_TESTMINDIC_H

#include "../../Global.h"
#include "../FBDic.h"

#include <cppunit/extensions/HelperMacros.h>


namespace csl {
    class TestFBDic : public CppUnit::TestFixture  {

	CPPUNIT_TEST_SUITE( TestFBDic );
	CPPUNIT_TEST( testBasics );
	CPPUNIT_TEST( testFileDump );
	CPPUNIT_TEST_SUITE_END();
    public:
	TestFBDic();

	void run();

	void testBasics();
	void testFileDump();


    private:
    };

    CPPUNIT_TEST_SUITE_REGISTRATION( TestFBDic );

    TestFBDic::TestFBDic() {
    }

    void TestFBDic::run() {
	testBasics();
	testFileDump();
    }
    

    /**
     * test the basic methods for reading access like getRoot, walk, isFinal etc.
     */
    void TestFBDic::testBasics() {
	csl::FBDic<> fbdic;
	fbdic.initConstruction();
	fbdic.addToken( L"albert", 42 );
	fbdic.addToken( L"anna", 43 );
	fbdic.addToken( L"berta", 44 );
	fbdic.finishConstruction();
	
	int ann = 0;

	CPPUNIT_ASSERT( fbdic.getFWDic().lookup( L"anna", &ann ) && ann == 43 );
	CPPUNIT_ASSERT( ! fbdic.getFWDic().lookup( L"annax", &ann ) );

	CPPUNIT_ASSERT( fbdic.getBWDic().lookup( L"anna", &ann )  && ann == 43 );
	CPPUNIT_ASSERT( fbdic.getBWDic().lookup( L"atreb", &ann )  && ann == 44 );
	CPPUNIT_ASSERT( ! fbdic.getBWDic().lookup( L"annax", &ann ) );
	CPPUNIT_ASSERT( ! fbdic.getBWDic().lookup( L"albert", &ann ) );

    }

    void TestFBDic::testFileDump() {
	csl::FBDic<> fbdic3;
	fbdic3.initConstruction();
	fbdic3.addToken( L"albert", 42 );
	fbdic3.addToken( L"anna", 43 );
	fbdic3.addToken( L"berta", 44 );
	fbdic3.addToken( L"lukas", 44 );
	fbdic3.finishConstruction();


 	fbdic3.writeToFile( "test_out___.fbdic" );


	csl::FBDic<> fbdic2( "test_out___.fbdic" );

	
	int ann = 0;

	CPPUNIT_ASSERT( fbdic2.getFWDic().lookup( L"albert", &ann ) && ann == 42 );
	CPPUNIT_ASSERT( fbdic2.getFWDic().lookup( L"anna", &ann ) && ann == 43 );
	CPPUNIT_ASSERT( fbdic2.getFWDic().lookup( L"berta", &ann ) && ann == 44 );
	CPPUNIT_ASSERT( ! fbdic2.getFWDic().lookup( L"annax", &ann ) );

	CPPUNIT_ASSERT( fbdic2.getBWDic().lookup( L"trebla", &ann )  && ann == 42 );
	CPPUNIT_ASSERT( fbdic2.getBWDic().lookup( L"anna", &ann )  && ann == 43 );
	CPPUNIT_ASSERT( fbdic2.getBWDic().lookup( L"atreb", &ann )  && ann == 44 );
	CPPUNIT_ASSERT( ! fbdic2.getBWDic().lookup( L"annax", &ann ) );

	fbdic2.getFWDic().printDic();


    }


} // namespace csl


#endif
