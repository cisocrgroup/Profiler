#include<iostream>
#include "Vaam/Vam.h"
#include "Getopt/Getopt.h"
#include "Stopwatch.h"

//#define CSL_VAMFILTER_PRINTNONE true

/**
 * Vam
 *
 * @file
 * @brief vamFilter is a command-line tool for the usage of the class Vam.
 * It is invoked with a distance bound \c k, a compiled minimized dictionary \c dic
 * and a file containing a set of patterns \c P.
 *
 * Please consult the documentation of class csl::Vam for details.
 *
 * @see csl::Vam
 * @author Ulrich Reffle, <uli@cis.uni-muenchen.de>
 *
 */
int main(int argc, const char** argv ) {

	try {

    std::locale::global( CSL_UTF8_LOCALE );
    Getopt opt( argc, argv );

    if( opt.getArgumentCount() < 2 ) {
	std::wcerr<<"Use like: vamFilter <dictionary> <pattern-file>"<<std::endl;
	exit( 1 );
    }

    csl::MinDic<> baseDic;
    baseDic.loadFromFile( opt.getArgument( 0 ).c_str() );

    csl::Vam vam( baseDic, opt.getArgument( 1 ).c_str() );

    if( opt.hasOption( "maxNrOfPatterns" ) ) {
	vam.setMaxNrOfPatterns( atoi( opt.getOption( "maxNrOfPatterns" ).c_str() ) );
    }


    std::vector< csl::Interpretation > answers;

    std::wstring query;

    csl::Stopwatch watch;

    while( std::wcin >> query ) {
	watch.start();
	answers.clear();
	vam.query( query, &answers );

	if( answers.empty() ) {
#ifndef CSL_VAMFILTER_PRINTNONE
	    std::wcout<<query<<":NONE"<<std::endl;
#endif
	}
	else {
	    // all interpretations of the query in one line

#ifndef CSL_VAMFILTER_PRINTNONE
	    for( std::vector< csl::Interpretation >::const_iterator it = answers.begin(); it!= answers.end(); ++it ) {
		it->print();
		if( it + 1  != answers.end() ) std::wcout<<"|";
	    }
	    std::wcout<<std::endl;
#endif
	}

//	std::wcout<<watch.readMilliseconds()<<" ms"<<std::endl;
    }

	} catch( csl::exceptions::cslException ex ) {
		std::wcout<<"Caught exception: "<<ex.what()<< std::endl;
		return 0;
	}

    return 0;
}
