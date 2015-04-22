#include "./TestFrequencyList.h"


int main() {

    std::locale::global( std::locale( "" ) );
    
    OCRCorrection::TestFrequencyList tester;

    tester.run();

    return 0;
}
