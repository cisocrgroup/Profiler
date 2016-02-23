#include<cstdlib>
#include "csl/MinDic2/MinDic2.h"
#include<errno.h>

using namespace csl;

int main( int argc, char** argv ) {
    std::locale::global( std::locale("") ); // set the environment's default locale

    try {

	if(argc != 2) {
	    std::cerr<<"Use like: "<<argv[0]<<" <binDic>"<<std::endl;
	    exit(1);
	}

	MinDic2 mdic;
	mdic.loadFromFile( argv[1] );


	std::wstring query;
	while( std::getline( std::wcin, query ).good() ) {
	    // is this really necessary ??
	    if ( query.length() > Global::lengthOfLongStr ) {
		throw exceptions::badInput( "csl::lookupMD2: Maximum length of input line violated (set by Global::lengthOfLongStr)" );
	    }

	    int ann = 0;
	    if( mdic.lookup( query.c_str(), &ann ) ) {
		std::wcout<<ann<<std::endl;
	    }
	    else std::wcout<<std::endl;
	}
	if( errno == EILSEQ ) {
	    throw exceptions::badInput( "csl::lookupMD2: Input encodig error" );
	}

    } catch( exceptions::cslException ex ) {
	std::wcout<<"lookupMD: "<<ex.what()<<std::endl;
	exit( 1 );
    }
    return 0;
}
