#include<iostream>

#include <cstdlib>
#include <stdexcept>
#include <sys/types.h>
#include <dirent.h>

#include "./Profiler.h"
#include "../Exceptions.h"
#include <DocXML/DocXMLWriter.h>
#include <DocXML/DocXMLReader.h>
#include <AltoXML/AltoXMLReader.h>
#include "SimpleOutputWriter.h"
#include<IBMGroundtruth/IBMGTReader.h>
#include<Getopt/Getopt.h>


void printHelp() {
    std::wcerr << "Use like: profiler --config=<iniFile> --sourceFile=<xmlFile>"
	       << std::endl
	       << "--sourceFormat DocXML | AltoXML | ABBYY_XML_DIR | TXT   (Default: DocXML)" << std::endl
	       << "[--out_xml  <outputFile> ]  Prints xml containing the lists of hist. variants and ocr errors." << std::endl
	       << "[--out_html <outputFile> ]  Prints all kinds of things to control the Profiler's performance to html." <<  std::endl
	       << "[--out_doc  <outputFile> ]  Prints the document in DocXML format, including correction suggestions." <<  std::endl
	       << std::endl
	       << "[--iterations n]            Sets the number of iterations. This overrides the setting in the config file." <<  std::endl
	       << std::endl
	       << "[--pageRestriction n]       Restrict processing to the initial n pages of the document." <<  std::endl
	       << std::endl
	       << "[--createConfigFile]        Prints a template for a config file to stdout. As specified above, such" <<  std::endl
	       << "                            a config file in ini format is needed to run the profiler." << std::endl
	       << std::endl
	       << "[--imageDir dir]            Only applies to ABBYY_XML_DIR input mode - sets the appropriate image dir." << std::endl
	       << std::endl
	       << "[--out_gtxml <outputFile>]  Prints xml containing the GROUNDTRUTH lists of hist. variants and ocr errors. (if available)" << std::endl
	       << "                            (Only for evaluation with groundtruth documents.)"
               << std::endl
               << "[--simpleOutput]            Print simple text output to stdout"
	       << std::endl
	       << "[--adaptive]                Use adaptive profiler, that uses correction information (defunct)"
               << std::endl
	;
}


int main( int argc, char const** argv ) {

    try {
    std::locale::global( std::locale( "" ) );
    csl::Getopt options;
    options.specifyOption( "help", csl::Getopt::VOID );
    options.specifyOption( "config", csl::Getopt::STRING );
    options.specifyOption( "sourceFile", csl::Getopt::STRING );
    options.specifyOption( "sourceFormat", csl::Getopt::STRING, "DocXML"  );
    options.specifyOption( "iterations", csl::Getopt::STRING );
    options.specifyOption( "pageRestriction", csl::Getopt::STRING );
    options.specifyOption( "xmlOut", csl::Getopt::STRING  );  // DEPRECATED
    options.specifyOption( "htmlOut", csl::Getopt::STRING );  // DEPRECATED
    options.specifyOption( "out_xml", csl::Getopt::STRING );
    options.specifyOption( "out_html", csl::Getopt::STRING );
    options.specifyOption( "out_doc", csl::Getopt::STRING );
    options.specifyOption( "out_none", csl::Getopt::VOID );
    options.specifyOption( "imageDir", csl::Getopt::STRING, "_NO_IMAGE_DIR_" );
    options.specifyOption( "createConfigFile", csl::Getopt::VOID );
    options.specifyOption( "simpleOutput", csl::Getopt::VOID );
    options.specifyOption( "adaptive", csl::Getopt::VOID );

    try {
	options.getOptionsAsSpecified( argc, argv );
    } catch( csl::Getopt::Exception exc ) {

	std::wcerr << "OCRC::profiler: Syntax error in command line call." << std::endl
		   << "Message from the command line parser:" << std::endl
               << OCRCorrection::Utils::utf8(exc.what()) << std::endl
		   << std::endl
		   << "Use: profiler --help" << std::endl
	    ;
	exit( EXIT_FAILURE );
    }

    // for backwards compatibility
    if( options.hasOption( "xmlOut" ) ) {
	std::wcerr << "WARNING: option --xmlOut is deprecated, was renamed to out_xml." << std::endl;
	options.setOption( "out_xml", options.getOption( "xmlOut") );
    }
    if( options.hasOption( "htmlOut" ) ) {
	std::wcerr << "WARNING: option --htmlOut is deprecated, was renamed to out_html." << std::endl;
	options.setOption( "out_html", options.getOption( "htmlOut") );
    }


    //////// print help
    if( options.hasOption( "help" ) ) {
	printHelp();
	exit( EXIT_SUCCESS );
    }


    /////// create config file
    if( options.hasOption( "createConfigFile" ) ) {
	OCRCorrection::Profiler::printConfigTemplate( std::wcout );
	exit( EXIT_SUCCESS );
    }

    ////// check for missing options for running the profiler
    if( ! ( options.hasOption( "config" ) && options.hasOption( "sourceFile" ) ) ) {
	std::wcerr << "Please specify obligatory options --config and --sourceFile" << std::endl
		   << "Use --help to learn more." << std::endl;
	exit( EXIT_FAILURE );
    }

    if( ! ( options.hasOption( "out_xml" ) ||
	    options.hasOption( "out_html" ) ||
	    options.hasOption( "out_doc" ) ||
            options.hasOption( "out_none" ) ||
            options.hasOption("simpleOutput")) ) {
	std::wcerr << "Specify some output." << std::endl
		   << "If you really want to run without any output, say this explicitly using --out_none" << std::endl
		   << "Use --help to learn about output options." << std::endl;
	exit( EXIT_FAILURE );
    }




    OCRCorrection::Profiler profiler;

    std::wstring wideConfigFile;

    try {
            std::wcerr << "Read config from " << OCRCorrection::Utils::utf8(options.getOption( "config" ) ) << std::endl;
	profiler.readConfiguration( options.getOption( "config" ).c_str() );
    }
    catch( std::exception const& exc ) {
            std::wstring wideWhat(OCRCorrection::Utils::utf8(exc.what()));;
            //csl::CSLLocale::string2wstring( exc.what(), wideWhat );
	std::wcerr << "Error while readConfiguration: " << wideWhat << std::endl;
	return EXIT_FAILURE;
    }

    /**
     * @todo catch conversion errors string-->integer
     */
    if( options.hasOption( "iterations" ) ) {
	profiler.setNumberOfIterations( atol( options.getOption( "iterations" ).c_str() ) );
    }

    if( options.hasOption( "pageRestriction" ) ) {
	profiler.setPageRestriction( atol( options.getOption( "pageRestriction" ).c_str() ) );
    }


    if( options.hasOption( "out_html" ) ) {
	profiler.setHTMLOutFile( options.getOption( "out_html" ) );
    }
    profiler.setAdaptive(options.hasOption("adaptive"));

    OCRCorrection::Document document;

    if( options.hasOption( "sourceFormat" ) ) {
	if( options.getOption( "sourceFormat" ) == "TXT" ) {
            OCRCorrection::TXTReader reader;
            reader.parse( options.getOption( "sourceFile" ).c_str(), &document );
            profiler.createProfile( document );
	}
	else if( options.getOption( "sourceFormat" ) == "DocXML" ) {
            OCRCorrection::DocXMLReader reader;
            reader.parse( options.getOption( "sourceFile" ), &document );
	    profiler.createProfile( document );
	}
	else if( options.getOption( "sourceFormat" ) == "AltoXML" ) {
            OCRCorrection::AltoXMLReader reader;
            reader.parse( options.getOption( "sourceFile" ), &document );
	    profiler.createProfile( document );
	}
	else if( options.getOption( "sourceFormat" ) == "ABBYY_XML_DIR" ) {
            OCRCorrection::AbbyyXmlParser reader;
	    reader.parseDirToDocument( options.getOption( "sourceFile" ), options.getOption( "imageDir" ), &document );
	    profiler.createProfile( document );
	}
	else if( options.getOption( "sourceFormat" ) == "IBM_GROUNDTRUTH" ) {
	    OCRCorrection::IBMGTReader r;
	    r.parse( options.getOption( "sourceFile" ).c_str(), &document );
	    profiler.createProfile( document );
	}
	else {
	    std::wcerr << "Unknown sourceFormat! Use: profiler --help" << std::endl;
	    exit( 1 );
	}
    }
    else {
        std::wcerr << "Very strange: no sourceFormat given." << std::endl;
        return EXIT_FAILURE;
    }


    if (options.hasOption("simpleOutput")) {
            OCRCorrection::SimpleOutputWriter(std::wcout, document).write();
    }


    if( options.hasOption( "out_xml" ) ) {
	std::wofstream os( options.getOption( "out_xml" ).c_str() );
	//os.imbue( csl::CSLLocale::Instance() );
	if( ! os.good() ) {
	    std::wcerr << L"OCRC::profiler: Could not open file for xml output" << std::endl;
	    throw std::runtime_error( "OCRC::profiler: Could not open file for xml output" );
	}
	profiler.profile2xml( os );
	os.close();
	std::wcerr << "Wrote xml export." << std::endl;
    }

    if( options.hasOption( "out_doc" ) ) {
        OCRCorrection::DocXMLWriter writer;

        writer.writeXML( document, options.getOption( "out_doc" ).c_str() );
    }




    } catch ( OCRCorrection::OCRCException& exc ) {
            std::wcerr << "OCRC::Profiler: Caught OCRCException:" << OCRCorrection::Utils::utf8(exc.what()) << std::endl;
        return EXIT_FAILURE;
    } catch ( csl::exceptions::cslException& exc ) {
            std::wcerr << "OCRC::Profiler: Caught cslException: " << OCRCorrection::Utils::utf8(exc.what()) << std::endl;
        return EXIT_FAILURE;
    }
    catch( std::exception& exc ) {
    	wprintf( L"profiler: caught exception: %s\n", exc.what() );
        return EXIT_FAILURE;
     }

}
