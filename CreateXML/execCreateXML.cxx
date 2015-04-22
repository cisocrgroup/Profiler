#include<iostream>
#include<string>
#include<csl/ErrDic/ErrDic.h>
#include "../CorrectionSystem/CorrectionSystem.h"
#include "../ErrDicApplier/ErrDicApplier.h"
#include "../LevDistance/LevDistanceWeight.h"



int main( int argc, char** argv ) {

    std::locale::global( CSL_UTF8_LOCALE ); // set the environment's default locale
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
    corrSys.initMSMatch( dlev, dicFile );
    corrSys.newDocument( txtFile );

    csl::ErrDic eD;
    csl::MinDic<int> mD;
    eD.loadFromFile( errDicFile );
	
    /**get filename from errdic-filename, load MinDic with this name
     */
    std::string tmp;
    const char* binFile;
    tmp = tmp + errDicFile;
    std::size_t pos = tmp.find_last_of(".");
    tmp.replace(pos, (tmp.size()-(pos)), "."); 

    tmp = tmp + "positive.mdic";
	
    binFile = tmp.c_str(); 
    mD.loadFromFile(binFile);

    OCRCorrection::LevenshteinWeights LevWeights;
	
    OCRCorrection::ErrDicApplier EDApplier( mD, eD );
    EDApplier.connectLevenshteinWeights(&LevWeights);

    EDApplier.scanDocument( corrSys.getDocument() );
    LevWeights.finish();

    csl::LevDistanceWeight levDistWeight;
    levDistWeight.connectLevenshteinWeights(&LevWeights);
    levDistWeight.addScoreToDocument(&(corrSys.getDocument()));	

    std::wofstream s( "out" );
    corrSys.getDocument().print( std::wcout );

	
}
