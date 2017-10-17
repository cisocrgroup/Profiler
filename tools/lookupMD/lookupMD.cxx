#include <cstdlib>
#include <errno.h>
#include "MinDic/MinDic.h"
#include "FBDic/FBDic.h"
#include "Getopt/Getopt.h"
#include "LevFilter/LevFilter.h"
#include "MSMatch/MSMatch.h"

using namespace csl;

class MyCandidateReceiver: public csl::LevFilter::CandidateReceiver {
public:
	MyCandidateReceiver(): found_(false) {}
	virtual ~MyCandidateReceiver() override = default;
	virtual void receive(const wchar_t*, int, int) override {
		found_ = true;
	}
	virtual void reset() override {
		found_ = false;
	}
	bool found() const noexcept {
		return found_;
	}
private:
	bool found_;
};

int main( int argc, char const** argv ) {
    std::locale::global( std::locale("") ); // set the environment's default locale

    try {

        csl::Getopt options;
	options.specifyOption("k", csl::Getopt::STRING);
        options.getOptionsAsSpecified( argc, argv );

	if(argc < 2) {
	    std::cerr<<"Use like: " << argv[0] << " <binDic> [--k n]"<<std::endl;
	    exit(1);
	}


	int k = 0;
	csl::MinDic<> const* minDic = 0;
	csl::FBDic<>* fbdic = 0; // this one is loaded in case a fbdic is passed to the program
	csl::MSMatch<>* msmatch = 0;

	if (options.hasOption("k")) {
		k = std::stoi(options.getOption("k"));
	}
	if (k > 0) {
		msmatch = new csl::MSMatch<>(k, options.getArgument(0).c_str());
	// In case a .fbdic file is passed, open it and use the FWDic
	} else if ((options.getArgument(0).size() >= 5) &&
			options.getArgument(0).substr(options.getArgument(0).size() - 5) == "fbdic") {
	    fbdic= new csl::FBDic<>( options.getArgument( 0 ).c_str() );
	    minDic = &( fbdic->getFWDic() );
	} else {
	    minDic = new csl::MinDic<>( options.getArgument( 0 ).c_str() );
	}

	MyCandidateReceiver r;

	std::wstring query;
	while( std::getline( std::wcin, query ).good() ) {
	    // is this really necessary ??
	    if ( query.length() > Global::lengthOfLongStr ) {
		throw exceptions::badInput("csl::lookupMD: Maximum length of input line violated (set by Global::lengthOfLongStr)" );
	    }

	    int ann = 0;
	    if (k <= 0) {
		    if( minDic->lookup( query.c_str(), &ann ) ) {
			std::wcout << ann << std::endl;
		    }
		    else std::wcout <<std::endl;
	    } else {
		    msmatch->query(query.c_str(), r);
		    if (r.found())
			    std::wcout << ann << std::endl;
		    else
			    std::wcout << std::endl;
		    r.reset();
	    }
	}
	if( errno == EILSEQ ) {
	    throw exceptions::badInput( "csl::lookupMD: Input encoding error" );
	}

    } catch( exceptions::cslException ex ) {
	std::wcout<<"lookupMD: "<<ex.what()<<std::endl;
	exit( 1 );
    }
    return 0;
}

