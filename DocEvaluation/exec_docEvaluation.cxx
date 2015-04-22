
#include<csl/Getopt/Getopt.h>

#include<DocEvaluation/DocEvaluation.h>
#include<DocXML/DocXMLReader.h>

int main( int argc, char const** argv ) {
    std::locale::global( std::locale( "" ) );

    csl::Getopt options;
    options.specifyOption( "silent", csl::Getopt::VOID );
    options.getOptionsAsSpecified( argc, argv );
    

    OCRCorrection::DocEvaluation docEval;
    
    OCRCorrection::Document doc;
    OCRCorrection::DocXMLReader reader;

    if( options.hasOption( "silent" ) ) {
	docEval.setVerbose( false );
	reader.setVerbose( false );
    }


    try { 
	reader.parse( options.getArgument( 0 ), &doc );

	docEval.analyzeDocument( doc );
    } catch( std::exception& exc ){
    	std::wcerr << "EXC:" << exc.what() << std::endl;
    	return EXIT_FAILURE;
    }
    
}
