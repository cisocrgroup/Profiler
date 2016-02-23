#include "./AbbyyXmlParser.h"
#include <Getopt/Getopt.h>


int main( int argc, char const** argv ) {

    csl::Getopt options( argc, argv );

    std::locale::global( std::locale( "" ) );

    OCRCorrection::AbbyyXmlParser p;
    OCRCorrection::Document doc;

    if( options.hasOption( "file" ) ) {
	p.parsePageToDocument( options.getOption( "file" ).c_str(), "_IMAGEDIR_", &doc );
    }
    else if( options.hasOption( "dir" ) ) {
	p.parseDirToDocument( options.getOption( "dir" ), "_IMAGEDIR_", &doc );
    }
    else {
	std::wcerr<< "run like:"<<std::endl
		  << "testAbbyyXmlParser --file=<xmlFile>" << std::endl
		  << "testAbbyyXmlParser --dir=<xmlDir>" << std::endl
		  << std::endl;
	exit( 1 );
    }

    doc.print();

    return 0;
};
