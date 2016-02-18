#ifndef CSL_TESTMINDIC_H
#define CSL_TESTMINDIC_H CSL_TESTMINDIC_H

#include "../../Global.h"
#include "../MinDic.h"

#include <cppunit/extensions/HelperMacros.h>


namespace csl {
    class TestMinDic : public CppUnit::TestFixture  {

	CPPUNIT_TEST_SUITE( TestMinDic );
	CPPUNIT_TEST( testBasics );
	CPPUNIT_TEST_SUITE_END();
    public:
	TestMinDic();

	void run();

	void testBasics();
// 	void testConstructionDetails();
// 	void lookupAllKeys();


    private:
	csl::MinDic<> mdic_;
	std::wstring oneKey_;
	int oneValue_;
	
    };

    CPPUNIT_TEST_SUITE_REGISTRATION( TestMinDic );

    TestMinDic::TestMinDic() {
	oneKey_  = L"aachen";
	oneValue_ = 42;
	mdic_.initConstruction();
	mdic_.addToken( oneKey_.c_str(), oneValue_ );
	mdic_.finishConstruction();
    }

    void TestMinDic::run() {
	testBasics();
    }
    

    /**
     * test the basic methods for reading access like getRoot, walk, isFinal etc.
     */
    void TestMinDic::testBasics() {
	/*
	 * walk key char by char and test if
	 * - key could be walked completely
	 * - pos_1 is a valid state of the automaton
	 * - pos_1 is a final state
	 */
	StateId_t pos_1 = mdic_.getRoot(); // set pos_1 to root
	const wchar_t* c = 0;
	for( c = oneKey_.c_str(); *c && pos_1; ++c ) {
	    pos_1 = mdic_.walk( pos_1, *c );
	}

	CPPUNIT_ASSERT( ! *c );
	CPPUNIT_ASSERT( pos_1 );
	assert( mdic_.isFinal( pos_1 ) );
	int ann = 0;

	/*
	 * test if the same result is returned by walkStr
	 */
	uint_t pos_2 = mdic_.walkStr( mdic_.getRoot(), oneKey_.c_str() );
	CPPUNIT_ASSERT( pos_1 == pos_2 );
	
	/*
	 * test if walking with the empty string works
	 */
	std::wstring empty = L"";
	CPPUNIT_ASSERT( mdic_.walkStr( mdic_.getRoot(), empty.c_str() ) == mdic_.getRoot() );


	/*
	 * test if the proper annotation is returned for the key
	 */
//	assert( mdic_.getAnnotation( (uchar*)key, &ann ) && ( ann == 15 ) );

	/*
	 * Test if walking with an invalid char is just failing and not causing any trouble
	 */
// 	pos_1 = mdic_.getRoot(); // set pos_1 to root
// 	assert( alph_.code( '#' ) == 0 );
// 	assert( mdic_.walk( pos_1, alph_.code( '#' ) ) == 0 );

// 	assert( ! mdic_.walkStr( mdic_.getRoot(), (uchar*)"dies#das" ) );
// 	assert( ! mdic_.walkStr( mdic_.getRoot(), (uchar*)"dies#" ) );


    }

//     void TestMinDic::lookupAllKeys() {
// 	std::ifstream fileHandle( lexFile_ );
// 	if ( !fileHandle.good() ) {
// 	    throw exceptions::badFileHandle( "Couldn't open file '" +
// 					     std::string( lexFile_ ) +
// 					     "' for reading." );
// 	}
// 	uchar line[Global::lengthOfLongStr];
// 	uchar* valueString = 0;
	
// 	bool testOk = true;
// 	size_t nrOfLines = 0;
// 	int value;
// 	while ( fileHandle.getline( ( char* ) line, Global::lengthOfLongStr ) )  {
// 	    uchar* c = ( uchar* )strchr( ( char* )line, Global::keyValueDelimiter );
	    
// 	    if( c ) {
// 		*c = 0;
// 		valueString = ( c + 1 );
// 	    }
// 	    uint_t pos = mdic_.walkStr( mdic_.getRoot(), line );
// 	    assert( mdic_.getAnnotation( line, &value ) );
// 	    testOk = testOk && pos && mdic_.isFinal( pos ) && ( atoi( (char*)valueString ) == value );
	    
// 	    ++nrOfLines;
// 	}
// 	assert( nrOfLines == mdic_.getNrOfKeys() );
// 	assert( testOk );
//     }
} // namespace csl


#endif
