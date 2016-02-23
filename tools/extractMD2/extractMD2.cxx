#include<iostream>
#include "Getopt/Getopt.h"
#include "Alphabet/Alphabet.h"
#include "MinDic2/MinDic2.h"

using namespace csl;

int main( int argc, const char** argv ) {
    std::locale::global( std::locale("") ); // set the environment's default locale

    Getopt options( argc, argv );


    if( options.getArgumentCount() != 1 ) {
	std::wcerr<<std::endl
		  <<"Use like: extractMD2 [options] <binDic> "<<std::endl
		  <<"Options:"<<std::endl
		  <<"--stat=1\tto print statistics of the MinDic"<<std::endl
		  <<"--dot=1\tto print dotcode for the MinDic"
		  <<std::endl<<std::endl;

	exit( 1 );
    }
    try {
	MinDic2 t;
	t.loadFromFile( options.getArgument( 0 ).c_str() );

	if( options.hasOption( "dot" ) ) {
	    t.toDot();
	}
	else if( options.hasOption( "stat" ) ) {
	    t.doAnalysis();
	}
	else t.printDic();

	return 0;
    } catch ( exceptions::cslException ex ) {
	std::wcerr << "Dictionary extraction failed: " << ex.what() << std::endl;
	return 1;
    }
}
