
#include<DictSearch/DictSearch.h>
#include<INIConfig/INIConfig.h>

#include<Document/Document.h>
#include<Stopwatch.h>

void printHelp() {
    std::wcout << "use like: markup --config <dictSearch_config.ini>" << std::endl;
}

void printHead() {
    std::wcout << "<html>" << std::endl
	       << "<head>"<<std::endl
	       << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl
	       << "<title>MARKUP</title>" << std::endl
	       << "<body>" << std::endl
	;
}

int main( int argc, char const** argv ) {
    std::locale::global( std::locale( "" ) );

    csl::Getopt options;
    options.specifyOption( "help", csl::Getopt::VOID );
    options.specifyOption( "config", csl::Getopt::STRING );

    options.getOptionsAsSpecified( argc, argv );


    if( ! options.hasOption( "config" ) ) {
	printHelp();
	return EXIT_FAILURE;
    }

    try {
	csl::INIConfig iniConfig( options.getOption( "config" ) );

	csl::DictSearch dictSearch;
	dictSearch.readConfiguration( iniConfig );
	dictSearch.initHypothetic( iniConfig.getstring( "language_model:patternFile" ) );


	csl::DictSearch::CandidateSet candSet;

	OCRCorrection::Document document; // this is created only to make use of getBorder!!!

	printHead();

	std::wstring line;
	size_t lineCount = 0;
	size_t wordCount = 0;
	csl::Stopwatch timeAll;
	csl::Stopwatch timeInterval;
	while( std::getline( std::wcin, line ).good() ) {
	    if( lineCount == 0 ) { // search for utf8 byte-order marks!
		if( !line.empty() && line.at( 0 ) == 65279 ) {
		    line.erase( 0, 1 );
		}
	    }

	    const wchar_t* pos = line.c_str();
	    size_t length = 0;
	    bool isNormal;
	    std::wstring w;
	    std::wstring style;
	    while( ( length = document.getBorder( pos, &isNormal ) ) != 0 ) {
		w = line.substr( pos - line.c_str(), length );
		pos += length;

		if( isNormal ) {
		    style.clear();
		    candSet.clear();
		    dictSearch.query( w, &candSet );
		    std::sort( candSet.begin(), candSet.end() );

		    if( !candSet.empty() ) {
			csl::DictSearch::Interpretation topcand = candSet.at( 0 );
			if( ! topcand.getHistInstruction().empty() ) style += L"color:red;";
			if( topcand.getLevDistance() > 0 ) style += L"background-color:#bbbbbb;";
			std::wcout << "<span style=\"" << style << "\" title=\"" << topcand/*.getDictModule().getName()*/ << "\">" << w << "</span>";
			//std::wcout << w;
		    }
		    else {
			//std::wcout << "<span style=\"color:purple;\" title=\"unknown\">" << w << "</span>";
		    }
		    ++wordCount;
		    if( wordCount % 100000 == 0 ) {
			std::wcerr << wordCount / 1000 << "k words processed. 100k words in "
				   << timeInterval.readMilliseconds() << " ms." << std::endl;
			timeInterval.start();
		    }
		}
		else { // not normal
		    std::wcout << w;
		}

	    }
	    std::wcout << "<br>" << std::endl;

	    ++lineCount;

	}
	if( errno == EILSEQ ) {
	    throw OCRCorrection::OCRCException( "OCRC markup: Input encoding error" );
	}

	std::wcerr << wordCount << " words processed in " << timeAll.readMilliseconds() << " ms." << std::endl
		   << "(Avg: " << (float)timeAll.readMilliseconds() / (float)wordCount <<  " ms / word)" << std::endl
		   << "      " << (float)wordCount / (float)timeAll.readMilliseconds()  << " words / ms)" << std::endl;


    } catch( std::exception const& exc ) {
	std::wcerr << "OCRC markup: caught exception:" << exc.what() << std::endl;
	return EXIT_FAILURE;
    }




}
