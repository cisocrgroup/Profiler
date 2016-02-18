#include<cstdlib>
#include "MinDic/MinDic.h"
#include<errno.h>
#include<Getopt/Getopt.h>

using namespace csl;

int main( int argc, char const** argv ) {
    std::locale::global( std::locale("") ); // set the environment's default locale

    Getopt options( argc, argv );

    try {

	if( ! options.hasOption( "defaultFreq" )  && options.getArgumentCount() != 1 ) {
	    std::cerr<<"Use like: addScoreToLex --defaultScore=n words_with_scores.mdic  < somelex.lex > somelex_with_scores.lex"<<std::endl;
	    exit(1);
	}

	int defaultScore = 0;
	if( options.hasOption( "defaultScore" ) ) {
	    defaultScore = atoi( options.getOption( "defaultScore" ).c_str() );
	}

	MinDic< int > mdic;
	if( options.hasOption( "defaultFreq" ) ) {
	    mdic.loadFromFile( "/mounts/data/proj/kristof/ARBEITSGRUPPE/ressourcen/freqListen/web_in_a_box/web_liste_gefiltert.frq.mdic" );
	}
	else {
	    mdic.loadFromFile( options.getArgument( 0 ).c_str() );
	}


	std::wstring query;
	int ann = 0;

	// this way of reading will SKIP THE LAST LINE if it is not properly terminated by \n
	while( std::getline( std::wcin, query ).good() ) {
	    ann = 0;
	    if( mdic.lookup( query.c_str(), &ann ) ) {
		std::wcout << query << "#" << ann << std::endl;
	    }
	    else {
		std::wcout << query << "#" << defaultScore << std::endl;
	    }
	}
	if( errno == EILSEQ ) {
	    throw exceptions::badInput( "csl::lookupMD: Input encodig error" );
	}

    } catch( exceptions::cslException ex ) {
	std::wcout<<"lookupMD: "<<ex.what()<<std::endl;
	exit( 1 );
    }
    return 0;
}
