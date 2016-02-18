


#include<Getopt/Getopt.h>
#include<Val/Val.h>

#include"../Exceptions.h"

void printHelp() {
    std::wcout << "Use like: createHistoricLexicon --baseDict <modern.mdic> --patternFile <patterns.txt>" <<  std::endl;
}

int main( int argc, char const** argv ) {

    std::locale::global( std::locale( "" ));

    csl::Getopt options;
    options.specifyOption( "help", csl::Getopt::VOID );
    options.specifyOption( "baseDict", csl::Getopt::STRING );
    options.specifyOption( "patternFile", csl::Getopt::STRING );
    options.getOptionsAsSpecified( argc, argv );

    if( options.hasOption( "help" ) ) {
	printHelp();
	return EXIT_SUCCESS;
    }

    if( ! ( options.hasOption( "baseDict" ) && options.hasOption( "patternFile" ) ) ) {
	printHelp();
	return EXIT_FAILURE;
    }


    std::wstring line;

    csl::MinDic<> baseDict( options.getOption( "baseDict" ).c_str() );

    csl::Val val( baseDict, options.getOption( "patternFile" ).c_str() );
    csl::Val::CandidateReceiver candset;


    std::wstring answerString;
    while( std::getline( std::wcin, line ).good() ) {
	candset.clear();
	answerString.clear();
	val.query( line, &candset );
	std::sort( candset.begin(), candset.end() );
	if( ( ! candset.empty() )  &&
	    ( ! candset.at( 0 ).getHistInstruction().empty() ) ) {

	    std::wcout << line << "#";
	    for( csl::Val::CandidateReceiver::const_iterator it = candset.begin(); it != candset.end(); ++it ) {
		std::wcout << *it;
		if( it + 1 != candset.end() ) std::wcout << "|";
	    }
	    std::wcout << std::endl;
	}
    }
    if( errno == EILSEQ ) { // catch encoding error
	throw OCRCorrection::OCRCException( "createHistoricLexicon: Encoding error in input sequence." );
    }


    return 0;
};
