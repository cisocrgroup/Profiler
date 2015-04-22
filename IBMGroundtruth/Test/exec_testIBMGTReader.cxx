

#include<IBMGroundtruth/IBMGTReader.h>

int main( int argc, char const** argv ) {

    std::locale::global( std::locale( "" ) );

    OCRCorrection::IBMGTReader r;
    OCRCorrection::Document doc;
    r.parse( argv[1], &doc );

    doc.print();


    return 0;
}
