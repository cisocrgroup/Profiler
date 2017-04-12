#include <DictSearch/DictSearch.h>
#include <Getopt/Getopt.h>
#include <INIConfig/INIConfig.h>
#include <Pattern/ComputeInstruction.h>
#include <Pattern/PatternWeights.h>

// uncomment this if you want so suppress output for the words
//#define CSL_DICTSEARCH_PRINTNONE 1;
#define CSL_QUERY 1

void printHelp() {
    std::wcerr<< std::endl
	      << "Use like: runDictSearch"<<std::endl
	      << "--config=<configFile>         configuration file (obligatory)" << std::endl
	      << "--machineReadable             Print (even more) machine-readable output, that is, all answers in one line, separated by '|'" << std::endl
#ifdef CSL_QUERY
	      << "--query=query                 specify query token" << std::endl
#endif // CSL_QUERY
	      << std::endl<<std::endl;
}


int main( int argc, char const** argv ) {
    std::locale::global( std::locale( "" ) ); // set a default locale

    csl::Getopt options;
    options.specifyOption( "help", csl::Getopt::VOID );
    options.specifyOption( "config", csl::Getopt::STRING );
    options.specifyOption( "machineReadable", csl::Getopt::VOID );
#ifdef CSL_QUERY
    options.specifyOption( "query", csl::Getopt::STRING );
#endif // CSL_QUERY

    options.getOptionsAsSpecified( argc, argv );


    if( options.hasOption( "help" ) ) {
	printHelp();
	return EXIT_SUCCESS;
    }
    if( !options.hasOption( "config" ) ) {
	printHelp();
	return EXIT_FAILURE;
    }

    // create a DictSearch-object
    csl::DictSearch dictSearch;

    csl::INIConfig config( options.getOption( "config" ) );
    dictSearch.readConfiguration( config );



    std::wstring query;
    csl::DictSearch::CandidateSet candSet;

    bool machineReadable = false;
    if( options.hasOption( "machineReadable" ) ) {
	machineReadable = true;
    }

#ifdef CSL_QUERY
    if (options.hasOption("query")) {
	    query = OCRCorrection::Utils::utf8(options.getOption("query"));
#else // CSL_QUERY
    while( std::getline( std::wcin, query ).good() ) {
#endif // CLS_QUERY
	candSet.clear(); // empty the CandidateSet
	dictSearch.query( query, &candSet ); // execute lookup

#ifndef CSL_DICTSEARCH_PRINTNONE
	std::sort( candSet.begin(), candSet.end() ); // sort candidates following a very coarse order relation
	// std::wcout << query << "\n";
	if( candSet.empty() ) {
	     std::wcout<<"--NONE--"<<std::endl;
	}
	else if( machineReadable ) {
	    // all interpretations of the query in one line
	    for( csl::DictSearch::CandidateSet::const_iterator it = candSet.begin(); it != candSet.end(); ++it ) {
		std::wcout << *it;
		if( it + 1  != candSet.end() ) std::wcout<<"|";
	    }
	    std::wcout<<std::endl;
	}
	else {
	// globalProfile_.ocrPatternProbabilities_.setDefault( csl::PatternWeights::PatternType( 1, 1 ), config_.ocrPatternSmoothingProb_ );
		csl::PatternProbabilities probs;
		probs.setDefault(csl::PatternWeights::PatternType(1, 1), 1e-5);
		probs.setDefault(csl::PatternWeights::PatternType(1, 0), 1e-5);
		probs.setDefault(csl::PatternWeights::PatternType(0, 1), 1e-5);
		probs.setDefault(csl::PatternWeights::PatternType(1, 2), 1e-5);
		probs.setDefault(csl::PatternWeights::PatternType(2, 1), 1e-5);
		    std::vector< csl::Instruction > instr;
		csl::ComputeInstruction computer;
		computer.connectPatternProbabilities(probs);
	    for( csl::DictSearch::CandidateSet::const_iterator it = candSet.begin();
			    it != candSet.end(); ++it ) {
		    instr.clear();
		    // std::wcout << "SIZE:  " << instr.size() << "\n";
		    // std::wcout << "WORD:  " << it->getWord() << "\n";
		    // std::wcout << "QUERY: " << query << "\n";
		    // std::wcout << "LEV:   " << it->getLevDistance() << "\n";
		    computer.computeInstruction(it->getWord(), query, &instr);
		    // std::wcout << "SIZE: " << instr.size() << "\n";
		    if (instr.empty()) {
			std::wcout << *it << std::endl;
		    } else {
			for (std::vector<csl::Instruction>::const_iterator j = instr.begin();
					j != instr.end(); ++j) {
				std::wcout << *it << ",ocr=" << *j << std::endl;
			}
		    }
	    }
	}
#endif
    }

}
