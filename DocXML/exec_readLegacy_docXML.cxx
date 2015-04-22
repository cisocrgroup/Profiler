#include<DocXML/LegacyDocXMLReader.h>
#include<DocXML/DocXMLWriter.cxx>
#include<Document/Document.h>

#include<csl/Getopt/Getopt.h>


void printHelp() {
    std::wcout << "Use like: readLegacy_docXML <xml_file>" << std::endl
	       << "          --dirMode            parse a directory of xml files into a multi-page document" << std::endl
	       << "          --alignedList        print a tab-separated list <wOCR>\t<GT_baseword>\t<GT_baseword> " << std::endl
	       << "                                  (this is for the IR experiments Nov 2011)" << std::endl;
}


int main( int argc, char const** argv ) {
    std::locale::global( std::locale( "" ) );

    csl::Getopt options;
    options.specifyOption( "help", csl::Getopt::VOID );
    options.specifyOption( "dirMode", csl::Getopt::VOID );
    options.specifyOption( "alignedList", csl::Getopt::VOID );
    options.getOptionsAsSpecified( argc, argv );

    if( options.hasOption( "help" ) ) {
	printHelp();
	return EXIT_SUCCESS;
    }

    if( options.getArgumentCount() < 1 ) {
	printHelp();
	return EXIT_FAILURE;
    }




    OCRCorrection::LegacyDocXMLReader reader;
    OCRCorrection::Document doc;
    
    if( options.hasOption( "dirMode" ) ) {
	reader.parseDir( options.getArgument( 0 ), "NO_IMAGE_DIR", &doc );
    }
    else {
	reader.parse( options.getArgument( 0 ), &doc );
    }
    
    
    if( ! options.hasOption( "alignedList" ) ) {
	// print new xml format to stdout
	OCRCorrection::DocXMLWriter writer;
	writer.writeXML( doc, std::wcout );
    }
    else  {
	// print aligned list to stdout
	
	// doc.findHyphenation();
	
	for( OCRCorrection::Document::iterator tok = doc.begin(); tok != doc.end(); ++tok ) {
	    if( tok->getWOCR_lc() == L"\n" ) {
		std::wcout << "**NL**\t**NL**\t**NL**\t**NL**" << std::endl;
	    }
	    else if( tok->getWOCR_lc() == L" " ) {
		// print nothing
	    }
	    else {
		std::wstring suggest;
		if( ! tok->isNormal() ) suggest = L"*****"; 
		else if( tok->getGroundtruth().getClassified() == L"modern" )                     suggest = L"*****";
		else if( tok->getGroundtruth().getClassified() == L"hypothetic" )                 suggest = tok->getGroundtruth().getBaseWord();
		else if( tok->getGroundtruth().getClassified() == L"simplex_other_historic" )     suggest = L"";
		else if( tok->getGroundtruth().getClassified() == L"other_trash" )                suggest = L"*****";
		else if( tok->getGroundtruth().getClassified() == L"other_dont_know" )            suggest = L"";
		else if( tok->getGroundtruth().getClassified() == L"name_lex_gap" )               suggest = L"";
		else if( tok->getGroundtruth().getClassified() == L"other_latin" )                suggest = L"*****";
		else if( tok->getGroundtruth().getClassified() == L"simplex_lex_gap" )            suggest = L"*****";
		else if( tok->getGroundtruth().getClassified() == L"compound_other_historic" )    suggest = L"";
		else if( tok->getGroundtruth().getClassified() == L"simplex_lex_gap_pattern" )    suggest = L"";
		else if( tok->getGroundtruth().getClassified() == L"other_ocr_error" )            suggest = L"*****";
		else if( tok->getGroundtruth().getClassified() == L"compound_lex_gap_pattern" )   suggest = L"";
		else if( tok->getGroundtruth().getClassified() == L"geo_other_historic" )         suggest = L"";
		else if( tok->getGroundtruth().getClassified() == L"geo_lex_gap_pattern" )        suggest = L"";
		else if( tok->getGroundtruth().getClassified() == L"compound_lex_gap" )           suggest = L"";
		else if( tok->getGroundtruth().getClassified() == L"unknown" )                    suggest = L"";
		else if( tok->getGroundtruth().getClassified() == L"geo_lex_gap" )                suggest = L"";
		else {
		    std::string narrow_class;
		    csl::CSLLocale::wstring2string( tok->getGroundtruth().getClassified(), narrow_class );
		    
		    throw OCRCorrection::OCRCException( std::string( "OCRC::readLegacy: unknown token classification" ) + narrow_class  );
		}
		
		std::wcout << tok->getWOCR_lc() << "\t"
			   << tok->getGroundtruth().getBaseWord() << "\t" 
			   << suggest << "\t" 
			   << "(" << tok->getGroundtruth().getClassified() << ")"
			   << std::endl;
	    }
	}

    }


}
