#ifndef OCRC_TESTINICONFIG_H
#define OCRC_TESTINICONFIG_H OCRC_TESTINICONFIG_H

#include<fstream>
#include<string.h>

#include "../INIConfig.h"
#include<cppunit/extensions/HelperMacros.h>


namespace csl {
    class TestINIConfig : public CppUnit::TestFixture  {

	CPPUNIT_TEST_SUITE( TestINIConfig );
	CPPUNIT_TEST( testBasics );
	CPPUNIT_TEST( testLongLine );
	CPPUNIT_TEST( testVariableSubstitution );
	CPPUNIT_TEST_SUITE_END();
    public:
	TestINIConfig();

	void run();

	void testBasics();
	void testLongLine();
	void testVariableSubstitution();


    private:
    };

    CPPUNIT_TEST_SUITE_REGISTRATION( TestINIConfig );

    TestINIConfig::TestINIConfig() {
    }

    void TestINIConfig::run() {
	testBasics();
	testLongLine();
	testVariableSubstitution();
    }
    

    /**
     * test the basic methods
     */
    void TestINIConfig::testBasics() {

	std::ofstream iniFile( "test.ini" );
	iniFile << "int_test = 42" << std::endl;
	iniFile << "double_test = 42.4242" << std::endl;
	iniFile << "string_test = 'passt scho'" << std::endl;
	iniFile << "bad_double = 42,4242" << std::endl; // with comma!

	iniFile << "bool_test_true = true"   << std::endl;
	iniFile << "bool_test_false = false" << std::endl;
	iniFile << "bool_test_0 = 0"   << std::endl;
	iniFile << "bool_test_1 = 1"   << std::endl;
	iniFile << "bool_test_1_5 = 1.5"   << std::endl;
	iniFile << "bool_test_bla = bla"   << std::endl;
	
	iniFile << "[eineKategorie]" << std::endl;
	iniFile << "string_test = 'passt immer no'" << std::endl;
	iniFile << "[andereKategorie]" << std::endl;
	iniFile << "string_test = 'passt anders auch'" << std::endl;
	
	iniFile.close();

	INIConfig iniconf( "test.ini" );

	CPPUNIT_ASSERT( iniconf.getint( ":int_test" ) == 42 );
	CPPUNIT_ASSERT( iniconf.getdouble( ":double_test" ) == 42.4242 );
	CPPUNIT_ASSERT( ! strcmp( iniconf.getstring( ":string_test" ), "passt scho" ) );
	CPPUNIT_ASSERT( ! strcmp( iniconf.getstring( "eineKategorie:string_test" ), "passt immer no" ) );

	try {
	    double d = iniconf.getdouble( ":bad_double" );
	    CPPUNIT_FAIL( "Exception expected" );
	} 
	catch( exceptions::cslException& exc ) {
	    // as expected
	}
	catch( std::exception& exc ) {
	    CPPUNIT_FAIL( "Did not expect this kind of exception." );
	}

	CPPUNIT_ASSERT( iniconf.getbool( ":bool_test_true" ) == true );
	CPPUNIT_ASSERT( iniconf.getbool( ":bool_test_false" ) == false );
	CPPUNIT_ASSERT( iniconf.getbool( ":bool_test_0" ) == false );
	CPPUNIT_ASSERT( iniconf.getbool( ":bool_test_1" ) == true );

	try {
	    CPPUNIT_ASSERT( iniconf.getbool( ":bool_test_1_5" ) == true );
	} catch( exceptions::cslException& exc ) {
	    // as expected
	} catch( std::exception& exc ) {
	    CPPUNIT_FAIL( "Did not expect this kind of exception." );
	}

	try {
	    CPPUNIT_ASSERT( iniconf.getbool( ":bool_test_bla" ) == true );
	} catch( exceptions::cslException& exc ) {
		// as expected
	} catch( std::exception& exc ) {
	    CPPUNIT_FAIL( "Did not expect this kind of exception." );
	}


	

	// test SectionIterator
	INIConfig::SectionIterator it = iniconf.sectionsBegin();
	CPPUNIT_ASSERT_EQUAL( std::string( *it ), std::string("einekategorie") );
	++it;
	CPPUNIT_ASSERT( std::string( *it ) == "anderekategorie" );
	++it;
	CPPUNIT_ASSERT( it == iniconf.sectionsEnd() );

    }

    void TestINIConfig::testLongLine() {
	std::ofstream iniFile( "test.ini" );
	iniFile << "A very long line, longer than 1024:";
	for( size_t i = 0 ; i < 1050; ++i ) {
	    iniFile << "x";
	}
	iniFile << std::endl;
	iniFile.close();
	
	CPPUNIT_ASSERT_THROW( INIConfig iniconf( "test.ini" ), exceptions::cslException  );
    }

    void TestINIConfig::testVariableSubstitution() {
	std::ofstream iniFile( "test.ini" );
	iniFile << "int_test=42" << std::endl;
	iniFile << "should_replace=${:int_test}mal" << std::endl;
	iniFile << "should_not_replace=${:unknown_key}mal" << std::endl;
	
	iniFile.close();
	INIConfig iniconf( "test.ini" );

	CPPUNIT_ASSERT_EQUAL( std::string( iniconf.getstring( ":should_replace" ) ), std::string( "42mal" ) );
	CPPUNIT_ASSERT_EQUAL( std::string( iniconf.getstring( ":should_not_replace" ) ), std::string( "${:unknown_key}mal" ) );
    }

} // eon
#endif
