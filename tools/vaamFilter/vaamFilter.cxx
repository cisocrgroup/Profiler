#include<iostream>
#include<Global.h>
#include<Vaam/Vaam.h>
#include<Vaam/Vaam.h>
#include<Getopt/Getopt.h>
#include<FBDic/FBDic.h>
//#include<MinDicString/MinDicString.h>

/**
 * Vaam
 *
 * @file
 * @brief vaamFilter is a command-line tool for the usage of the class Vaam.
 * It is invoked with a distance bound \c k, a compiled minimized dictionary \c dic
 * and a file containing a set of patterns \c P.
 *
 * Please consult the documentation of class csl::Vaam for details.
 *
 * @see csl::Vaam
 * @author Ulrich Reffle, <uli@cis.uni-muenchen.de>
 *
 */


#ifdef CSL_VAAMFILTER_USE_TRANSTABLE
typedef csl::Vaam< csl::TransTable< csl::TT_PERFHASH, uint16_t, uint32_t > > Vaam_t;
#else
typedef csl::Vaam< csl::MinDic<> > Vaam_t;
#endif


//forward declarations
int main(int argc, const char** argv );
void printAnswer( csl::Interpretation const& i, std::wostream& os = std::wcout );
void printAnswer_machineReadable( csl::Interpretation const& i, std::wostream& os = std::wcout );



void printHelp() {
    std::wcerr<< "Use like: vaamFilter [options] <distance> <dictionary> <pattern-file>"<<std::endl
	      << std::endl
	      << "Options:" << std::endl
	      << "--minNrOfPatterns=N       Allow only interpretations with N or more pattern applications. Defaults to 0." << std::endl
	      << "--maxNrOfPatterns=N       Allow only interpretations with at most N pattern applications. Defaults to INFINITE." << std::endl
	      << std::endl
	      << "--patternDelimiter=c      Lets symbol c appear as the delimiter between left and right pattern side in the output." << std::endl
	      << "                          Note that this will NOT affect the expected form of the pattern-file." << std::endl
	      << "--machineReadable         Print (even more) machine-readable output, i.e. all answers in one line, separated by '|'" << std::endl
	      << "--noStatusMessage         In machineReadable mode, vaamFilter prints a status message at the very beginning."  << std::endl
	      << "                          Use this optionto suppress this status message." << std::endl
	      << "--systemlocale            Use the system locale. This is the default STILL." << std::endl
	      << "                          But the default is likely to change to the csl utf8 locale." << std::endl
	      << std::endl
	      << "--printQuery              Print the query word to STDOUT. (Mostly for debug purposes.) " << std::endl

	      << std::endl;
}

int main(int argc, const char** argv ) {

    try {
	// retrieve command line options
	csl::Getopt opt;
	opt.specifyOption( "help", csl::Getopt::VOID );
	opt.specifyOption( "minNrOfPatterns", csl::Getopt::STRING );
	opt.specifyOption( "maxNrOfPatterns", csl::Getopt::STRING );
	opt.specifyOption( "machineReadable", csl::Getopt::VOID );
	opt.specifyOption( "noStatusMessage", csl::Getopt::VOID );
	opt.specifyOption( "patternDelimiter", csl::Getopt::STRING );
	opt.specifyOption( "systemlocale", csl::Getopt::VOID );
	opt.specifyOption( "printQuery", csl::Getopt::VOID );
	opt.getOptionsAsSpecified( argc, argv );


	std::locale::global( std::locale("") ); // set the environment's default locale

	if( opt.hasOption( "help" ) ) {
	    printHelp();
	    return EXIT_SUCCESS;
	}

	if( opt.getArgumentCount() < 3 ) {
	    printHelp();
	    return EXIT_FAILURE;
	}


	// this one is actually used for the matching process
	Vaam_t::MinDic_t const* baseDic = 0;

	// one of those is loaded from file; baseDic is (a part of) this dictionary
	csl::MinDic<>* minDic = 0;
	csl::FBDic<>* fbdic = 0;
	csl::MinDicString* minDicString = 0;

	// In case a .fbdic file is passed, open it and use the FWDic
	if( ( opt.getArgument( 1 ).size() >= 5 ) && opt.getArgument( 1 ).substr( opt.getArgument( 1 ).size() - 5 ) == "fbdic" ) {
	    fbdic= new csl::FBDic<>( opt.getArgument( 1 ).c_str() );
	    baseDic = &( fbdic->getFWDic() );
	}
	else if( ( opt.getArgument( 1 ).size() >= 3 ) && opt.getArgument( 1 ).substr( opt.getArgument( 1 ).size() - 3 ) == "mds" ) {
	    minDicString = new csl::MinDicString( opt.getArgument( 1 ).c_str() );
	    baseDic = minDicString;
	}
	else {
	    // this tmp-hack is because of the const-ness of baseDic
	    minDic = new csl::MinDic<>( opt.getArgument( 1 ).c_str() );
	    baseDic = minDic;
	}


	Vaam_t vaam( *baseDic, opt.getArgument( 2 ).c_str() );

	if( opt.hasOption( "maxNrOfPatterns" ) ) {
	    vaam.setMaxNrOfPatterns( atoi( opt.getOption( "maxNrOfPatterns" ).c_str() ) );
	}

	if( opt.hasOption( "minNrOfPatterns" ) ) {
	    vaam.setMinNrOfPatterns( atoi( opt.getOption( "minNrOfPatterns" ).c_str() ) );
	}

	bool machineReadable = false;
	if( opt.hasOption( "machineReadable" ) ) {
	    machineReadable = true;
	}

	if( opt.hasOption( "patternDelimiter" ) ) {
	    if( opt.getOption( "patternDelimiter" ).length() != 1 ) {
		throw csl::exceptions::cslException( "csl::vaamFilter: option patternDelimiter must specify exactly 1 symbol" );
	    }
	    csl::Pattern::setLeftRightDelimiter( opt.getOption( "patternDelimiter" ).at( 0 ) );
	}

	Vaam_t::CandidateReceiver answers;

	std::wstring query;

	size_t maxDistance = atoi( opt.getArgument( 0 ).c_str() );
	vaam.setDistance( maxDistance );

	size_t nrOfQueries = 0;
	size_t sumOfCandidates = 0;

	std::wstring wideAnnotation;

	if( machineReadable && ( ! opt.hasOption( "noStatusMessage" ) ) ) {
	    std::wcout << "csl::Vaam: READY [machineReadable=true]" << std::endl;
	}

	while( std::getline( std::wcin, query ).good() ) {
	    ++nrOfQueries;
	    answers.clear();

	    if( opt.hasOption( "printQuery" ) ) {
		std::wcout << ">>>" << query << "<<<" << std::endl;
	    }

	    vaam.query( query, &answers );

	    std::sort( answers.begin(), answers.end() );

	    sumOfCandidates += answers.size();

	    if( answers.empty() ) {
		std::wcout<<query<<":NONE"<<std::endl;
	    }
	    else if( machineReadable ) {
		// all interpretations of the query in one line
		for( std::vector< csl::Interpretation >::const_iterator it = answers.begin(); it!= answers.end(); ++it ) {
		    it->print();
		    if( it + 1  != answers.end() ) std::wcout<<"|";
		}
		std::wcout<<std::endl;
	    }
	    else {
		// new line for each interpretation of the query
		for( std::vector< csl::Interpretation >::const_iterator it = answers.begin(); it!= answers.end(); ++it ) {
		    std::wcout << it->getWord() << ":"
			       << it->getBaseWord()
			       << "+" << it->getInstruction()
			       << ",dist=" << it->getLevDistance();

		    if( minDicString ) {
			// in a MinDicString the annotation ("score") is the offset of the word's respective annotation
			csl::CSLLocale::string2wstring( (char const*)minDicString->getAnnByOffset( it->getBaseWordScore() ), wideAnnotation );
			std::wcout << ",annotation=" << wideAnnotation;
		    }
		    std::wcout<<std::endl;
		}
	    }

	} // for all input

	if( errno == EILSEQ ) {
	    throw csl::exceptions::badInput( "csl::vaamFilter: Input encodig error" );
	}


	if( fbdic ) {
	    delete( fbdic );
	    fbdic = 0;
	}

	if( minDic ) {
	    delete( minDic );
	    minDic = 0;
	}
	if( minDicString ) {
	    delete( minDicString );
	    minDicString = 0;
	}

    } catch( csl::exceptions::cslException ex ) {
	std::wcerr << "Caught exception: " << ex.what() << std::endl;
	return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
