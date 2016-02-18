#include<iostream>
#include<string>
#include "../CorrectionSystem/CorrectionSystem.h"
#include<DocXML/DocXMLWriter.h>
#include<Getopt/Getopt.h>
#include<Stopwatch.h>



void printHelp() {
    std::wcout
	<< L"Use like: testCorrectionSystem --config=<configFile> --sourceDir=<xmlDir> --imageDir=<imageDir> --docXMLOut=<docXMLOutput>" << std::endl
	<< L"xmlDir contains abbyy xml output files" << std::endl
	;
}

int main( int argc, char const** argv ) {

    std::locale::global( std::locale( "" ) );

    csl::Getopt options;
    options.specifyOption( "help", csl::Getopt::STRING );
    options.specifyOption( "config", csl::Getopt::STRING );
    options.specifyOption( "file", csl::Getopt::STRING );
    options.specifyOption( "sourceDir", csl::Getopt::STRING );
    options.specifyOption( "imageDir", csl::Getopt::STRING );
    options.specifyOption( "docXMLOut", csl::Getopt::STRING );
    options.specifyOption( "profileOut", csl::Getopt::STRING );
    options.getOptionsAsSpecified( argc, argv );


    OCRCorrection::CorrectionSystem corrSys;

    if( options.hasOption( "help" ) ) {
	printHelp();
	exit( EXIT_SUCCESS );
    }

    if( ! (
	    options.hasOption( "config" )
	    && options.hasOption( "sourceDir" )
	    && options.hasOption( "imageDir" )
	    && options.hasOption( "docXMLOut" )
	    ) ) {
	printHelp();
	exit( EXIT_FAILURE );
    }

    corrSys.readConfiguration( options.getOption( "config" ).c_str() );

    corrSys.newDocument( options.getOption( "sourceDir" ).c_str(), options.getOption( "imageDir" ).c_str(), OCRCorrection::CorrectionSystem::ABBYY_XML_DIR );

    OCRCorrection::DocXMLWriter writer;
    writer.writeXML( corrSys.getDocument(), options.getOption( "docXMLOut" ) );

    return( EXIT_SUCCESS );
}
