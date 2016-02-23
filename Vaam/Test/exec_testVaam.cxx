#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "./TestVaam.h"


int main() {

// this is easier for debugging
#if false
    csl::TestVaam testVaam;
    
    testVaam.testWordBorders();
    return( 1 );
#endif


    // Adds the test to the list of test to run
    CppUnit::TextUi::TestRunner runner;
    runner.addTest( csl::TestVaam::suite() );
    
//     // Change the default outputter to a compiler error format outputter
//     runner.setOutputter( new CppUnit::CompilerOutputter( &runner.result(),
// 							 std::cerr ) );

    // Run the tests.
    bool wasSucessful = runner.run();

    // Return error code 1 if the one of tests failed.
    return wasSucessful ? 0 : 1;    

}
