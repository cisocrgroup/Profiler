#include<iostream>
#include<fstream>


#include<csl/Getopt/Getopt.h>
#include<AltoXML/AltoEnrich.h>

void printHelp() {
    std::wcerr << "Use like: altoEnrich <alto-source-dir> <output-dir>" << std::endl
	       << std::endl
	       << "Options:"                           << std::endl
	       << "--help"                             << std::endl
	       << "--config <ini-file>   profiler configuration  " << std::endl
	       << std::endl
	       << "--out_html <file>     profiler debug html output" << std::endl
	;
}



int main( int argc, char const** argv ) {
    std::locale::global( std::locale( "" ) );
    
    
    csl::Getopt options;
    options.specifyOption( "help",      csl::Getopt::VOID   );
    options.specifyOption( "config",  csl::Getopt::STRING     );
    options.specifyOption( "out",     csl::Getopt::STRING     );
    options.specifyOption( "out_html",     csl::Getopt::STRING     );
    options.specifyOption( "overwrite",   csl::Getopt::VOID                       );
    options.getOptionsAsSpecified( argc, argv );

    if( options.hasOption( "help" ) ) {
	printHelp();
	return EXIT_SUCCESS;
    }

    if( ! ( ( options.getArgumentCount() == 2 )  && 
	    options.hasOption( "config" ) ) ) {
	printHelp();
	return EXIT_FAILURE;
    }

    std::string sourceDir = options.getArgument( 0 );
    std::string outputDir = options.getArgument( 1 );
    


    OCRCorrection::AltoEnrich enrich;

    if( options.hasOption( "out_html" ) ) {
	enrich.setHTMLOutFile( options.getOption( "out_html" ) );
    }

    try {
	enrich.addProfilerData( sourceDir, options.getOption( "config" ), outputDir );
    }
    catch( std::exception const& exc ) {
	std::wcerr << "altoEnrich: Caught: " << csl::CSLLocale::string2wstring( exc.what() ) << std::endl;
    }
    
    
    

    return EXIT_SUCCESS;
}
