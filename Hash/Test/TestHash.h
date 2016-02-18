#ifndef CSL_TESTHASH_H
#define CSL_TESTHASH_H CSL_TESTHASH_H

#include "../../Global.h"
#include "../Hash.h"

#include <cppunit/extensions/HelperMacros.h>


namespace csl {
    class TestHash : public CppUnit::TestFixture  {

	CPPUNIT_TEST_SUITE( TestHash );
	CPPUNIT_TEST( testBasics );
	CPPUNIT_TEST_SUITE_END();
    public:
	TestHash();

	void run();

	void testBasics();


    private:
	csl::Hash<> hash_;
	
    };

    /**
     * test the basic methods for reading access like getRoot, walk, isFinal etc.
     */
    void TestHash::testBasics() {

} // namespace csl


#endif
