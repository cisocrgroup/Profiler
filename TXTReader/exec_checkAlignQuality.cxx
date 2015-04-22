#include<csl/Getopt/Getopt.h>
#include<csl/LevDistance/LevDistance.h>
#include<DocXML/DocXMLReader.h>
    
int main( int argc, char const** argv ) {
    std::locale::global( std::locale( "" ) );

    csl::Getopt options;
    options.getOptionsAsSpecified( argc, argv );
    
    OCRCorrection::DocXMLReader reader;
    csl::LevDistance distComputer;
    OCRCorrection::Document doc;

    reader.parse( options.getArgument( 0 ), &doc);

    size_t countLev = 0;
    size_t countChars = 0;
    for( OCRCorrection::Document::iterator tok = doc.begin(); tok != doc.end(); ++tok ) {
	size_t dist = distComputer.getLevDistance( tok->getGroundtruth().getWOrig_lc(), tok->getWOCR_lc() );
	std::wcout << tok->getGroundtruth().getWOrig_lc() << "<-->" << tok->getWOCR_lc() << " = " << dist << std::endl;
	countLev += dist;
	countChars += tok->getGroundtruth().getWOrig_lc().size();
    }

    std::wcout << (float)countLev / countChars << " operations per GT character" << std::endl;

}
    
