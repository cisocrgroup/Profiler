

#include<SimpleEnrich/SimpleEnrich.h>
#include<Exceptions.h>
#include<DocXML/DocXMLReader.h>
#include<DocXML/DocXMLWriter.h>
#include<csl/Getopt/Getopt.h>

int main( int argc, char const** argv ) {
    std::locale::global( std::locale( "" ) );

    csl::Getopt options;
    options.specifyOption( "config", csl::Getopt::STRING );
    options.specifyOption( "out", csl::Getopt::STRING );

    options.getOptionsAsSpecified( argc, argv );


    if( ! options.hasOption( "config" ) ) {
	throw OCRCorrection::OCRCException( "Error: Please specify a configuration file using --config" );
    }

    if( options.getArgumentCount() != 1 ) {
	throw OCRCorrection::OCRCException( "Error: Please specify an input document as command line argument." );
    }

    OCRCorrection::Document document;
    OCRCorrection::DocXMLReader docReader;
    docReader.parse( options.getArgument( 0 ), &document );

    
    OCRCorrection::SimpleEnrich enricher( options.getOption( "config" ) );
    
    enricher.enrichDocument( &document );
    
    OCRCorrection::DocXMLWriter writer;

    if( options.hasOption( "out" ) ) {
	std::wofstream outstream;
	outstream.imbue( csl::CSLLocale::Instance() );
	outstream.open( options.getOption( "out" ).c_str() );
	writer.writeXML( document, outstream );
    }
    else writer.writeXML( document, std::wcout );
}
