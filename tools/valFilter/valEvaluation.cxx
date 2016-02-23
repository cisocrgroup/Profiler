#include<iostream>
#include "Val/Val.h"
#include "Getopt/Getopt.h"
#include "Stopwatch.h"
#include "FBDic/FBDic.h"

#define OCRCORRECTION_VAAMFILTER_PRINTNONE true

/**
 * Vaam
 *
 * @file
 *
 * @see csl::Vaam
 * @author Ulrich Reffle, <uli@cis.uni-muenchen.de>
 *
 */



int main(int argc, const char** argv ) {

    try {

    csl::Stopwatch watch;
    watch.start();

    std::locale::global( std::locale("") ); // set the environment's default locale

    csl::Getopt opt( argc, argv );

    if( opt.getArgumentCount() < 2 ) {
	std::wcerr<< "Use like: vaamFilter [options] <dictionary> <pattern-file>"<<std::endl
		  << std::endl
		  << "Options:" << std::endl
		  << "--minNrOfPatterns=N       Allow only interpretations with N or more pattern applications. Defaults to 0." << std::endl
		  << "--maxNrOfPatterns=N       Allow only interpretations with at most N pattern applications. Defaults to INFINITE." << std::endl
		  << "--machineReadable=1       Print (even more) machine-readable output, i.e. all answers in one line, separated by '|'" << std::endl;
	exit( 1 );
    }


    csl::MinDic<> const* baseDic = 0;
    csl::FBDic<> const* fbdic = 0;

    // In case a .fbdic file is passed, open it and use the FWDic
    if( opt.getArgument( 0 ).substr( opt.getArgument( 0 ).size() - 5 ) == "fbdic" ) {
	fbdic= new csl::FBDic<>( opt.getArgument( 0 ).c_str() );
	baseDic = &( fbdic->getFWDic() );
    }
    else {
	csl::MinDic<>* tmp = 0;

	// this tmp-hack is because of the const-ness of baseDic
	tmp = new csl::MinDic<>();
	tmp->loadFromFile( opt.getArgument( 0 ).c_str() );
	baseDic = tmp;

    }


    csl::Val val( *baseDic, opt.getArgument( 1 ).c_str() );

    if( opt.hasOption( "maxNrOfPatterns" ) ) {
	val.setMaxNrOfPatterns( atoi( opt.getOption( "maxNrOfPatterns" ).c_str() ) );
    }

    if( opt.hasOption( "minNrOfPatterns" ) ) {
	val.setMinNrOfPatterns( atoi( opt.getOption( "minNrOfPatterns" ).c_str() ) );
    }

    bool machineReadable = false;
    if( opt.hasOption( "machineReadable" ) ) {
	machineReadable = true;
    }


    csl::Val::CandidateReceiver answers;

    std::wstring query;

    size_t nrOfQueries = 0;
    size_t sumOfCandidates = 0;

    if( machineReadable ) {
	std::wcout << "csl::Vaam: READY [machineReadable=true]" << std::endl;
    }
    else {
	std::wcout << "csl::Vaam: READY [machineReadable=false]" << std::endl;
    }

    std::wcerr << "vaamFilter startup time: " << watch.readMilliseconds() << "ms" << std::endl;
    watch.start();

    while( std::getline( std::wcin, query ).good() ) {
	++nrOfQueries;
	answers.clear();
	val.query( query, &answers );

#ifndef OCRCORRECTION_VAAMFILTER_PRINTNONE
	std::sort( answers.begin(), answers.end() );
#endif

	sumOfCandidates += answers.size();

	if( answers.empty() ) {
#ifndef OCRCORRECTION_VAAMFILTER_PRINTNONE
	    std::wcout<<query<<":NONE"<<std::endl;
#endif
	}
	else if( machineReadable ) {
	    // all interpretations of the query in one line
#ifndef OCRCORRECTION_VAAMFILTER_PRINTNONE
	    for( std::vector< csl::Interpretation >::const_iterator it = answers.begin(); it!= answers.end(); ++it ) {
		it->print();
		if( it + 1  != answers.end() ) std::wcout<<"|";
	    }
	    std::wcout<<std::endl;
#endif
	}
	else {
	    // new line for each interpretation of the query
#ifndef OCRCORRECTION_VAAMFILTER_PRINTNONE
	    for( std::vector< csl::Interpretation >::const_iterator it = answers.begin(); it!= answers.end(); ++it ) {
		it->print();
		std::wcout<<std::endl;
	    }
#endif
	}

//	std::wcout<<watch.readMilliseconds()<<" ms"<<std::endl;
    } // for all input

    if( errno == EILSEQ ) {
	throw csl::exceptions::badInput( "csl::vaamFilter: Input encodig error" );
    }


    std::wcout<<watch.readMilliseconds()<<" ms for "<< nrOfQueries << " queries. AVG: "
	      << (double)watch.readMilliseconds() / (double)nrOfQueries << "ms" << std::endl;
    std::wcout<<sumOfCandidates << " answers for "<< nrOfQueries << " queries. AVG: "
	      << (double)sumOfCandidates / (double)nrOfQueries << std::endl;


    if( fbdic ) {
	delete( fbdic );
	fbdic = 0;
	baseDic = 0;
    }

    if( baseDic ) {
	delete( baseDic );
	baseDic = 0;
    }

    } catch( csl::exceptions::cslException ex ) {
	std::wcout<<"Caught exception: "<<ex.what()<< std::endl;
    }



    return 0;
}
