#include<AbbyyXmlParser/AbbyyXmlParser.h>
#include<DocXML/DocXMLWriter.h>
#include<Document/Document.h>
#include<csl/Getopt/Getopt.h>


void printHelp() {
    std::wcerr << "Use like: abbyy_transform --file=<abbyy_xml_file>" << std::endl
	       << "          or                               " << std::endl
	       << "          abbyy_transform --dir=<abbyy_xml_dir>  "
	       << std::endl
	       << "Options:" << std::endl
	       << "--outFormat= TXT | DocXML      (Default: TXT)" << std::endl
               << "--imageDir <dir>               (Default: _NO_IMAGE_DIR_)" << std::endl
               << "        useful for output in DocXML format"
	       << "" << std::endl
	       << "" << std::endl
	       << std::endl;
}


int main( int argc, char const** argv ) {
    std::locale::global( std::locale( "" ) );

    csl::Getopt options;
    options.specifyOption( "help", csl::Getopt::VOID );
    options.specifyOption( "file", csl::Getopt::STRING );
    options.specifyOption( "dir", csl::Getopt::STRING );
    options.specifyOption( "imageDir", csl::Getopt::STRING, "_NO_IMAGE_DIR_" );
    options.specifyOption( "outFormat", csl::Getopt::STRING, "TXT" );

    try {
        options.getOptionsAsSpecified( argc, argv );
    } catch( std::exception& exc ) {
        std::wstring wideWhat;
        csl::CSLLocale::string2wstring( exc.what(), wideWhat );
        std::wcerr << "Command line parsing failed: " << wideWhat << std::endl;
        return EXIT_FAILURE;
    }

    if( options.hasOption( "help") ) {
	printHelp();
	return EXIT_SUCCESS;
    }

        if( options.getArgumentCount() != 0 ) {
	printHelp();
	return EXIT_FAILURE;
    }

    OCRCorrection::AbbyyXmlParser p;
    OCRCorrection::Document d;

    try {
	if( options.hasOption( "file" ) ) {
	    p.parsePageToDocument( options.getOption( "file" ), options.getOption("imageDir"), &d );
	}
	else if( options.hasOption( "dir" ) ) {
	    p.parseDirToDocument( options.getOption( "dir" ), options.getOption("imageDir"), &d );
	}
	else {
	    printHelp();
	    return 1;
	}
    } catch( std::exception const& ex ) {
	std::wstring wideWhat;
	csl::CSLLocale::string2wstring( ex.what(), wideWhat );
	std::wcerr << "Parsing of abbyy xml failed: " << wideWhat << std::endl;
	std::wcerr << "Abort." << std::endl;
	return EXIT_FAILURE;
    }


    try { 
	if( options.getOption( "outFormat" ) == "TXT" ) {
	    d.dumpOCRToPlaintext( std::wcout );
	}
	else if( options.getOption( "outFormat" ) == "DocXML" ) {
	    OCRCorrection::DocXMLWriter writer;
	    writer.writeXML( d, std::wcout );
	}
	else {
	    printHelp();
	    return 1;
	}
    } catch( std::exception const& ex ) {
	std::wcerr << "Output failed: " << csl::CSLLocale::string2wstring( ex.what() ) << std::endl;
	std::wcerr << "Abort." << std::endl;
	return EXIT_FAILURE;
    }

    return 0;
}
