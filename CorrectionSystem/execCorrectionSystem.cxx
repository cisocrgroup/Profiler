#include<iostream>
#include<string>
#include "../CorrectionSystem/CorrectionSystem.h"

/**
 * go to dir cxx/
 * type: $> make bin/correctionSystem
 * $> ./bin/correctionSystem (ARGS)
 *
 * Output:
 * 1 Zeile pro Wort im Text
 * [wort] "normal tokens", nur aus Buchstaben bestehend.
 *     Dahinter in runden Klammern alle Kandidaten: (token,levenshtein-dist, frequency)
 * {bla} "not normal token" hierfür wurden keine Kandidaten berechnet
 *
 * normal/not normal ist übrigens für jedes Token mit Token::isNormal() abfragbar
 *
 * @todo Make this live again.
 */

int main( int argc, char** argv ) {

        std::locale::global(std::locale(""));
        //std::locale::global( CSL_UTF8_LOCALE ); // set the environment's default locale
//    std::locale::global( std::locale( "de_DE.utf8" ) );
//    setlocale(LC_CTYPE, "de_DE.utf8");  /*Setzt das Default Encoding für das Programm */



    size_t dlev = 0;
    if( argc < 4 ) {
	std::wcout<<L"Use like: "<<argv[0]<<" dicFile errDicFile textFile [levDist]"<<std::endl;
	std::wcout<<L"A dicFile might be: dictionaries/all_d_100.frq.fbdic"<<std::endl;
	std::wcout<<L"A textFile is any utf-8 text: e.g. exampleFiles/test.small.txt"<<std::endl;
	std::wcout<<L"levDist is the upper distance bound for candidate extraction. It defaults to 2"<<std::endl;
	exit( 1 );
    }
    else if( argc == 4 ) {
	dlev = 2; // default setting for distance bound
    }
    else { // argc is >= 5
	dlev = atoi( argv[4] );
    }
    const char* dicFile = argv[1];
    const char* errDicFile = argv[2];
    const char* txtFile = argv[3];



    OCRCorrection::CorrectionSystem corrSys;


    /**
     * Use Document's print method to dump doc to stdout
     */
    corrSys.getDocument().print( std::wcout );

}
