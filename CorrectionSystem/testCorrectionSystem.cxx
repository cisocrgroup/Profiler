#include<iostream>
#include<string>
#include "../CorrectionSystem/CorrectionSystem.h"
#include<DocXML/DocXMLWriter.h>
#include<Getopt/Getopt.h>
#include<Stopwatch.h>


int main( int argc, char const** argv ) {


    std::wcerr << "sizeof(std::wstring)=" << sizeof(std::wstring)<<  std::endl;
    std::wcerr << "sizeof(Token)=" << sizeof(OCRCorrection::Token)<<  std::endl;
    std::wcerr << "sizeof(Token::Groundtruth)=" << sizeof(OCRCorrection::Token::Groundtruth)<<  std::endl;
    std::wcerr << "sizeof(std::vector< Character >)=" << sizeof(std::vector< OCRCorrection::Character >)<<  std::endl;
    std::wcerr << "sizeof(Token::AbbyySpecifics)=" << sizeof(OCRCorrection::Token::AbbyySpecifics)<<  std::endl;
    // return 1;

    std::locale::global( std::locale( "" ) );

    csl::Getopt options;
    options.specifyOption( "config", csl::Getopt::STRING );
    options.specifyOption( "file", csl::Getopt::STRING );
    options.specifyOption( "dir", csl::Getopt::STRING );
    options.specifyOption( "imageDir", csl::Getopt::STRING );
    options.specifyOption( "xmlOut", csl::Getopt::STRING );
    options.getOptionsAsSpecified( argc, argv );


    OCRCorrection::CorrectionSystem corrSys;

    if( ! options.hasOption( "config" ) ) {
	std::wcout << L"Use like: testCorrectionSystem --config=<configFile> --dir=<xmlDir>" << std::endl;
	std::wcout << L"xmlDir contains abbyy xml output files" << std::endl;
	exit( EXIT_FAILURE );
    }

    corrSys.readConfiguration( options.getOption( "config" ).c_str() );

    if( options.hasOption( "file" ) ) {
	//corrSys.newDocument( options.getOption( "file" ).c_str(), options.getOption( "imageDir" ).c_str(), OCRCorrection::CorrectionSystem::ABBYY_XML );
    }
    else if( options.hasOption( "dir" ) ) {
	corrSys.newDocument( options.getOption( "dir" ).c_str(), options.getOption( "imageDir" ).c_str(), OCRCorrection::CorrectionSystem::ABBYY_XML_DIR );
    }
    else {
	std::wcout<<L"Use like: testCorrectionSystem --dict=<dicFile> --dir=<xmlDir>"<<std::endl;
	std::wcout<<L"xmlDir contains abbyy xml output files"<<std::endl;
	exit( EXIT_FAILURE );
    }


    if( options.hasOption( "xmlOut" ) ) {
	OCRCorrection::DocXMLWriter writer;
	std::wofstream of( options.getOption( "xmlOut" ).c_str() );
	// make sure that stream writes utf8 and that no thousands separators are printed
	//of.imbue( std::locale( CSL_UTF8_LOCALE, new std::numpunct< wchar_t >() ) );
	if( ! of.good() ) {
	    throw OCRCorrection::OCRCException( "testCorrectionSystem: Could not open xmlOut file" );
	}
	writer.writeXML( corrSys.getDocument(), of );
    }

    /**
     * Use Document's print method to dump doc to stdout
     */
    //corrSys.getDocument().print( std::wcout );

    // for( OCRCorrection::Document::iterator it = corrSys.getDocument().begin(); it != corrSys.getDocument().end(); ++it ) {
    // 	if( it->getNrOfCandidates() )
    // 	    std::wcout << it->getWOCR() << " - " << it->candidateAt( 0 ).getBaseWord() << std::endl;
    // }

    return( EXIT_SUCCESS );
}
