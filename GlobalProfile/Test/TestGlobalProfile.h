#ifndef OCRC_TESTGLOBALPROFILE_H
#define OCRC_TESTGLOBALPROFILE_H OCRC_TESTGLOBALPROFILE_H

#include "../GlobalProfile.h"
#include "../GlobalProfileXMLReader.h"

#include <cppunit/extensions/HelperMacros.h>


namespace OCRCorrection {
    class TestGlobalProfile : public CppUnit::TestFixture  {

	CPPUNIT_TEST_SUITE( TestGlobalProfile );
	CPPUNIT_TEST( testBasics );
	CPPUNIT_TEST_SUITE_END();
    public:
	TestGlobalProfile();

	void run();

	void testBasics();
	void testAlignedIn();
	void testPages();
	void testCandidate();
	void testToken();

    private:
	std::locale loc_;
    };

    CPPUNIT_TEST_SUITE_REGISTRATION( TestGlobalProfile );

    TestGlobalProfile::TestGlobalProfile() {

    }

    void TestGlobalProfile::run() {
	testBasics();
    }
    

    /**
     * test the basic methods
     */
    void TestGlobalProfile::testBasics() {
	GlobalProfile p;
	GlobalProfileXMLReader reader;
	
	reader.parse( "/tmp/test2.profile.xml", &p );

    }

}
#endif
