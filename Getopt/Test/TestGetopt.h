#ifndef CSL_TESTGETOPT_H
#define CSL_TESTGETOPT_H CSL_TESTGETOPT_H

#include "../../Global.h"
#include "../Getopt.h"

#include <cppunit/extensions/HelperMacros.h>


namespace csl {
    class TestGetopt : public CppUnit::TestFixture  {

	CPPUNIT_TEST_SUITE( TestGetopt );
	CPPUNIT_TEST( testBasics );
	CPPUNIT_TEST( testSpecified );
	CPPUNIT_TEST( testFailure );
	CPPUNIT_TEST_SUITE_END();
    public:

	void run();

	void testBasics();
	void testSpecified();
	void testFailure();

    };

    CPPUNIT_TEST_SUITE_REGISTRATION( TestGetopt );

//     TestGetopt::TestGetopt() {
//     }

    void TestGetopt::run() {
	testBasics();
	testSpecified();
	testFailure();
    }
    

    /**
     * test the basic functionality
     */
    void TestGetopt::testBasics() {
	// test old way, without any specification of options
	char const* argv[] = { "progname", "--flag1=1", "--flag2=value", "arg1", "arg2" };
	int argc = 5;
	
	Getopt getopt;
	getopt.getOptions( argc, argv );

	CPPUNIT_ASSERT( getopt.hasOption( "flag1" ) );
	CPPUNIT_ASSERT( getopt.hasOption( "flag2" ) );

	CPPUNIT_ASSERT( getopt.getOption( "flag1" ) == "1" );
	CPPUNIT_ASSERT( getopt.getOption( "flag2" ) == "value" );

	CPPUNIT_ASSERT( getopt.getArgumentCount() == 2 );
	CPPUNIT_ASSERT( getopt.getArgument( 0 ) == "arg1" );
	CPPUNIT_ASSERT( getopt.getArgument( 1 ) == "arg2" );
    }


    void TestGetopt::testSpecified() {
	if( 1 ) { // create artificial block
	    Getopt getopt;
	
	    getopt.specifyOption( "flag1", Getopt::VOID );
	    getopt.specifyOption( "flag2", Getopt::STRING );
	    getopt.specifyOption( "flag3", Getopt::STRING );
	    char const* argv[] = { "progname", "--flag1", "--flag2=value", "--flag3", "value3", "arg1", "arg2" };
	    int argc = 7;
	
	    getopt.getOptionsAsSpecified( argc, argv );

	    CPPUNIT_ASSERT( getopt.hasOption( "flag1" ) );
	    CPPUNIT_ASSERT( getopt.hasOption( "flag2" ) );
	    CPPUNIT_ASSERT( getopt.hasOption( "flag3" ) );

	    CPPUNIT_ASSERT( ! getopt.hasOption( "flag4" ) );

	    CPPUNIT_ASSERT( getopt.getOption( "flag1" ) == "1" );
	    CPPUNIT_ASSERT( getopt.getOption( "flag2" ) == "value" );
	    CPPUNIT_ASSERT( getopt.getOption( "flag3" ) == "value3" );

	    CPPUNIT_ASSERT( getopt.getArgumentCount() == 2 );
	    CPPUNIT_ASSERT( getopt.getArgument( 0 ) == "arg1" );
	    CPPUNIT_ASSERT( getopt.getArgument( 1 ) == "arg2" );
	}

	if( 1 ) {
	    // unknown option throws exception (without '=' )
	    Getopt getopt;
	    getopt.specifyOption( "flag1", Getopt::VOID );
	    char const* argv[] = { "progname", "--flag2", "arg1", "arg2" };
	    int argc = 5;
	    CPPUNIT_ASSERT_THROW( getopt.getOptionsAsSpecified( argc, argv ), csl::exceptions::cslException );
	}
	if( 1 ) {
	    // unknown option throws exception (with '=' )
	    Getopt getopt;
	    getopt.specifyOption( "flag1", Getopt::VOID );
	    char const* argv[] = { "progname", "--flag2=bla", "arg1", "arg2" };
	    int argc = 5;
	    CPPUNIT_ASSERT_THROW( getopt.getOptionsAsSpecified( argc, argv ), csl::exceptions::cslException );
	}
	if( 1 ) {
	    // value for void option with '=' throws exception
	    Getopt getopt;
	    getopt.specifyOption( "flag1", Getopt::VOID );
	    char const* argv[] = { "progname", "--flag1=bla", "arg1", "arg2" };
	    int argc = 5;
	    CPPUNIT_ASSERT_THROW( getopt.getOptionsAsSpecified( argc, argv ), csl::exceptions::cslException );
	}

    }

    /**
     * test the basic functionality
     */
    void TestGetopt::testFailure() {
	/*
	 * This syntax is not allowed when using getOptions()
	 */
	char const* argv[] = { "progname", "--flag1", "arg1" };
	int argc = 5;
	Getopt getopt;
	CPPUNIT_ASSERT_THROW( getopt.getOptions( argc, argv ), csl::exceptions::cslException );


    }

} // namespace csl


#endif
