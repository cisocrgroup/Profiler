#include "MinDic/MinDic.h"
#include "Getopt/Getopt.h"
#include "Global.h"
#include<iostream>


void printHelp() {
    std::wcerr<< "Use like:                compileMD <txtDic> [<binDic>]"   << std::endl
	      << "          cat <txtDic> | compileMD --stdin  <binDic>" << std::endl
	      << std::endl
	      << std::endl;
}

/**
 * @brief Program used to compute a MinDic<> from a word list and dump the automaton to a file.
 *
 */
int main(int argc, char const** argv) {
    std::locale::global( std::locale( "" ) );

    csl::Getopt options;
    options.specifyOption( "help", csl::Getopt::VOID );
    options.specifyOption( "stdin", csl::Getopt::VOID );
    options.getOptionsAsSpecified( argc, argv );

    if( options.hasOption( "help" ) ) {
	    printHelp();
	    return EXIT_SUCCESS;
    }

    if( options.getArgumentCount() == 0 ) {
	printHelp();
	return EXIT_FAILURE;
    }

    if( options.hasOption( "stdin" ) ) {
	if( options.getArgumentCount() != 1 ) {
	    printHelp();
	    return EXIT_FAILURE;
	}

	try {
	    csl::MinDic< int > t;

	    t.initConstruction();

	    std::wstring line;
	    int annotation = 0;

	    while( std::getline( std::wcin, line ), std::wcin.good() ) {

		t.parseAnnotation( &line, &annotation );
		t.addToken( line.c_str(), annotation );
	    }
	    if( errno == EILSEQ ) { // catch encoding error
		throw csl::exceptions::badInput( "MinDic::compileDic: Encoding error in input sequence." );
	    }
	    t.finishConstruction();

	    t.writeToFile( options.getArgument( 0 ).c_str() );

	    //   t.toDot();
	    //   t.printCells();
	    return EXIT_SUCCESS;

	} catch ( csl::exceptions::cslException ex ) {
	    std::wcout<<"compileMD failed: "<<ex.what()<<std::endl;
	    return EXIT_FAILURE;
	} catch ( std::exception ex ) {
	    std::wcout<<"compileMD failed: "<<ex.what()<<std::endl;
	    return EXIT_FAILURE;
	}

    }
    else  {
	try {
	    std::string inFile = options.getArgument( 0 );
	    std::string outFile;

	    if( options.getArgumentCount() == 2 ) {
		outFile = options.getArgument( 1 );
	    }
	    else {
		outFile = inFile;
		if( outFile.substr( outFile.size() - 4 ) == ".lex" ) {
		    outFile.replace( outFile.size() - 4, 4, ".mdic" );
		}
		else {
		    std::wcerr << "Your input filename does not end with '.lex'. In this case please provide an output filename as second argument." << std::endl;
		    return EXIT_FAILURE;
		}
	    }

	    csl::MinDic< int > t;
	    t.compileDic( inFile.c_str() );

	    t.writeToFile( outFile.c_str() );

	    //   t.toDot();
	    //   t.printCells();

	} catch ( std::exception ex ) {
	    std::wcout<<"compileMD failed: "<<ex.what()<<std::endl;
	    return EXIT_FAILURE;
	}

    }

}
