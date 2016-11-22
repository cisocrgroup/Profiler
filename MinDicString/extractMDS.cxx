#include<iostream>
#include "../Alphabet/Alphabet.h"
#include "./MinDicString.h"

using namespace fsdict;

int main( int argc, char** argv ) {
    setlocale(LC_CTYPE, "de_DE.UTF-8");  /*Setzt das Default Encoding für das Programm */

    if( argc < 2 ) {
	std::cerr<<"Use like: "<<argv[0]<<" <binDic> [DOT]"<<std::endl;
	exit(1);
    }
    try {
	MinDicString t;
	t.loadFromFile( argv[1] );
	if( argc == 3 && ! strcmp( argv[2], "DOT" ) ) {
	    t.toDot();
	}
	else t.printDic();


	return 0;
    } catch ( exceptions::fsdictException ex ) {
	std::cerr << "Dictionary extraction failed: " << ex.what() << std::endl;
	return 1;
    }
}
