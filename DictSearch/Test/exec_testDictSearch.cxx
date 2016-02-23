#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "./TestDictSearch.h"


int main() {



    // Adds the test to the list of test to run
    CppUnit::TextUi::TestRunner runner;
    runner.addTest( csl::TestDictSearch::suite() );
    
//     // Change the default outputter to a compiler error format outputter
//     runner.setOutputter( new CppUnit::CompilerOutputter( &runner.result(),
// 							 std::cerr ) );

    // Run the tests.
    bool wasSucessful = runner.run();

    // Return error code 1 if one of the tests failed.
    return wasSucessful ? 0 : 1;    
    
}
