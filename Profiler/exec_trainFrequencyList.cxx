#include <memory>

#include "./FrequencyList.h"
#include<Getopt/Getopt.h>
#include<Stopwatch.h>
#include<TXTReader/TXTReader.h>

void printHelp() {
    std::wcerr
	<< "This program uses a ground truth historical text to estimate an initial probabilistic model" << std::endl
	<< "for the Profiling tool." << std::endl
	<< std::endl
	<< "Use like: trainFrequencyList --config=<dictSearch.ini> --textFile <utf8_txtFile>" << std::endl
	<< "          trainFrequencyList --config=<dictSearch.ini> --typeFreqList <utf8_txtFile>" << std::endl
	<< std::endl
	<< "Type-frequency lists are expected in the format: <word>\t<freq\n> "
	<< std::endl
	<< "An alternative mode offers to treat all given words as modern words, so no dictionary and pattern file are needed:"  << std::endl
	<< "Use like: trainFrequencyList --allModern --textFile <utf8_txtFile>" << std::endl
	<< "          trainFrequencyList --allModern --typeFreqList <utf8_txtFile>" << std::endl
	<< std::endl
	<< std::endl
	<< "Options:" << std::endl
	<< "--frequencyThreshold=<N>                " << std::endl
	<< std::endl;
}

/**
 * This is to train a ferquency model and dump it to the hard disk.
 */
int main( int argc, char const** argv ) {

    csl::Stopwatch stopwatch;

    std::locale::global( std::locale( "" ) );

    csl::Getopt options;
    options.specifyOption( "help", csl::Getopt::VOID );

    options.specifyOption( "textFile", csl::Getopt::STRING );
    options.specifyOption( "typeFreqList", csl::Getopt::STRING );
    options.specifyOption( "frequencyThreshold", csl::Getopt::STRING );

    options.specifyOption( "allModern", csl::Getopt::VOID );
    options.specifyOption( "config", csl::Getopt::STRING );

    options.specifyOption( "out_freqlist", csl::Getopt::STRING, "./freqlist.binfrq" );
    options.specifyOption( "out_weights", csl::Getopt::STRING, "./weights.txt" );
    options.specifyOption( "out_corpusLexicon", csl::Getopt::STRING, "./corpusLexicon.lex" );

    try {
	options.getOptionsAsSpecified( argc, argv );
    } catch( csl::Getopt::Exception& exc ) {
	std::wcerr << "OCRC::trainFrequencyList: Incorrect command line call." << std::endl
		   << "Message from the command line parser:" << std::endl
               << OCRCorrection::Utils::utf8(exc.what()) << std::endl
		   << std::endl
		   << "Use: trainFrequencyList --help" << std::endl
	    ;
	exit( EXIT_FAILURE );
    } catch( std::exception& exc ) {
	std::wcerr << "HA?!" << std::endl;
    }


    if( options.hasOption( "help" ) ) {
	printHelp();
	exit( EXIT_SUCCESS );
    }
    if( ! ( options.hasOption( "textFile" ) || options.hasOption( "typeFreqList" ) ) ) {
	std::wcerr << "Give text file or type-frequency list for training. Use --help to learn more." << std::endl;
	exit( EXIT_FAILURE );
    }
    if( ! ( options.hasOption( "allModern" ) ||
	    ( options.hasOption( "config" ) ) ) ) {
	std::wcerr << "Specify obligatory option 'config'. Otherwise, use --allModern. Use --help to learn more." << std::endl;
	exit( EXIT_FAILURE );
    }


    OCRCorrection::FrequencyList freqlist;


    OCRCorrection::FrequencyList::Trainer* trainer = 0;

    try {
	if( options.hasOption( "allModern" ) ) {
            trainer = new OCRCorrection::FrequencyList::Trainer( freqlist );
	}
	else {
            trainer = new OCRCorrection::FrequencyList::Trainer( freqlist, options.getOption( "config" ).c_str() );
	}

	if( options.hasOption( "frequencyThreshold" ) ) { // for freqlist processing, ignored otherwise
            trainer->setFrequencyThreshold(OCRCorrection::Utils::toNum<size_t>(options.getOption( "frequencyThreshold")));
	}


	if( options.hasOption( "textFile" ) ) {
	    OCRCorrection::Document doc;
	    OCRCorrection::TXTReader reader;
	    reader.parse( options.getOption( "textFile" ).c_str(), &doc );

	    trainer->doTraining( doc );
	}
	else if( options.hasOption( "typeFreqList" ) ) {
	    trainer->doTrainingOnFreqlist( options.getOption( "typeFreqList" ).c_str() );
	}
	trainer->finishTraining();

	csl::INIConfig iniConf(options.getOption("config").c_str());
    const char *wpath = iniConf.getstring("language_model:patternWeightsFile");
    const char *fpath = iniConf.getstring("language_model:freqListFile");
    const char *cpath = iniConf.getstring("language_model:corpusLexicon");

	if ( not options.hasOption( "allModern" ) ) {
            trainer->writeCorpusLexicon( cpath );
	}
    freqlist.writeToFile(fpath, wpath);
	std::wcerr << "Training took " << stopwatch.readMilliseconds() << " ms" <<  std::endl;
    } catch( std::exception& exc ) {
	std::wcerr << "trainFrequencyList failed: " << exc.what() << std::endl;
	return EXIT_FAILURE;
    }


    return EXIT_SUCCESS;
}
