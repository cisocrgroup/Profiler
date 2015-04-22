#include<csl/INIConfig/INIConfig.h>
#include<csl/Getopt/Getopt.h>
#include<Document/Document.h>
#include<AbbyyXmlParser/AbbyyXmlParser.h>
#include<DocXML/DocXMLReader.h>
#include<DocXML/DocXMLWriter.h>
#include<TXTReader/AlignedTXTReader.h>
#include<DocXML/MergedGroundtruthReader.h>

#include "./GuessTraces.h"


void printHelp() {
    std::wcout << "Use like: guessTraces --config <config.ini> document|dir " << std::endl
	       << " --inFormat DocXML | AlignTXT | AlignTXTDir"
	       << " --merge  AbbyyXMLDir"
	       << std::endl;
}

int main( int argc, char const** argv ) {
    std::locale::global( std::locale( "" ) );

//    try {

	csl::Getopt options;
	options.specifyOption( "help", csl::Getopt::VOID );
	options.specifyOption( "config", csl::Getopt::STRING );
	options.specifyOption( "inFormat", csl::Getopt::STRING, "AlignTXT" );
	options.specifyOption( "merge", csl::Getopt::STRING );
	options.getOptionsAsSpecified( argc, argv );


	if( options.hasOption( "help" ) ) {
	    printHelp();
	    return EXIT_SUCCESS;
	}

	if( ! options.hasOption( "config" ) || options.getArgumentCount() < 1 ) {
	    printHelp();
	    return EXIT_FAILURE;
	}
    
	csl::INIConfig iniConf( options.getOption( "config" ) );

	OCRCorrection::GuessTraces gt;

	gt.getDictSearch().readConfiguration( iniConf );

	OCRCorrection::Document doc;
    
	if( options.getOption( "inFormat" ) == "DocXML" ) {
	    OCRCorrection::DocXMLReader reader;
	    reader.parse( options.getArgument( 0 ), &doc );
	}
	else if( options.getOption( "inFormat" ) == "AlignTXTDir" ) {
	    OCRCorrection::AlignedTXTReader reader;
	    reader.parseDir( options.getArgument( 0 ), "_NO_IMAGE_DIR_", &doc );
	}
	else if( options.getOption( "inFormat" ) == "AlignTXT" ) {
	    OCRCorrection::AlignedTXTReader reader;
	    reader.parseSingleFile( options.getArgument( 0 ), &doc );
	}

	gt.processDocument( &doc );

	OCRCorrection::DocXMLWriter writer;

	if( options.hasOption( "merge" ) ) {
	    OCRCorrection::AbbyyXmlParser abbyyReader;
	    OCRCorrection::MergedGroundtruthReader merger;
	    OCRCorrection::Document abbyyDoc;
	    abbyyReader.parseDirToDocument( options.getOption( "merge" ), "_NO_IMAGE_DIR_", &abbyyDoc );
	    
	    merger.mergeDocuments( &abbyyDoc, doc );

	    writer.writeXML( abbyyDoc, std::wcout);

	}
	else {
	    writer.writeXML( doc, std::wcout);
	}
	



    // } catch( std::exception const& exc ) {
    // 	std::wcerr << "OCRC::guessTraces: caught exception: " << exc.what() << std::endl;
    // 	return EXIT_FAILURE;
    // }

    return EXIT_SUCCESS;
}
