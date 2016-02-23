#include<DocXML/DocXMLReader.h>
#include<DocXML/DocXMLWriter.h>
#include<AltoXML/AltoXMLReader.h>
#include<AbbyyXmlParser/AbbyyXmlParser.h>
#include<TXTReader/TXTReader.h>
#include<TXTReader/AlignedTXTReader.h>
#include<IBMGroundtruth/IBMGTReader.h>
#include<Document/Document.h>
#include<Getopt/Getopt.h>
#include "Utils/Utils.h"

void printHelp() {
    std::wcerr << "Use like: ocrc_transform " << std::endl
	       << "" << std::endl
	       << "Options:" << std::endl
	       << "--source <file/dir>  (obligatory)" << std::endl
	       << "--outFile   output file" << std::endl
	       << "--inFormat  DocXML | AltoXML | ABBYY_XML | AlignTXT  (default: DocXML)" << std::endl
	       << "--outFormat DocXML | TXT | OCRTXT | PAGES_TXT                   (default: TXT)" << std::endl
	       << "--dirMode" << std::endl
	       << "--imageDir" << std::endl
	       << "" << std::endl
	       << "" << std::endl
	;
}



int main( int argc, char const** argv ) {
    std::locale::global( std::locale( "" ) );


    csl::Getopt options;
    options.specifyOption( "help",      csl::Getopt::VOID                       );
    options.specifyOption( "inFormat",  csl::Getopt::STRING, "DocXML"           );
    options.specifyOption( "outFormat", csl::Getopt::STRING, "TXT"              );
    options.specifyOption( "source",    csl::Getopt::STRING                     );
    options.specifyOption( "outFile",   csl::Getopt::STRING                     );
    options.specifyOption( "dirMode",   csl::Getopt::VOID                       );
    options.specifyOption( "imageDir",  csl::Getopt::STRING, "_NO_IMAGE_DIR_"   );
    options.specifyOption( "maxCandidates", csl::Getopt::STRING );
    options.getOptionsAsSpecified( argc, argv );

   if( options.hasOption( "help" ) ) {
	printHelp();
	return EXIT_SUCCESS;
    }

    if( ! options.hasOption( "source" ) ) {
	printHelp();
	return EXIT_FAILURE;
    }

    OCRCorrection::Document document;


    if( options.hasOption( "inFormat" ) ) {
	if( options.getOption( "inFormat" ) == "TXT" ) {
            OCRCorrection::TXTReader reader;
            reader.parse( options.getOption( "source" ).c_str(), &document );
	}
	else if( options.getOption( "inFormat" ) == "DocXML" ) {
	    try {
		OCRCorrection::DocXMLReader reader;
		reader.parse( options.getOption( "source" ), &document );
	    }
	    catch( std::exception& exc ) {
                std::wcerr << "ocrc_transform: Reading DocXML failed: " << OCRCorrection::Utils::utf8(exc.what()) << std::endl;
		return EXIT_FAILURE;
	    }
	}
	else if( options.getOption( "inFormat" ) == "AltoXML" ) {
            OCRCorrection::AltoXMLReader reader;
	    if( options.hasOption( "dirMode" ) ) {
		reader.parseDir( options.getOption( "source" ), options.getOption( "imageDir" ), &document );
	    }
	    else {
		reader.parse( options.getOption( "source" ), &document );
	    }
	}
	else if( options.getOption( "inFormat" ) == "ABBYY_XML" ) {
            OCRCorrection::AbbyyXmlParser reader;
	    try {
	    if( options.hasOption( "dirMode" ) ) {
		reader.parseDirToDocument( options.getOption( "source" ), options.getOption( "imageDir" ), &document );
	      }
	      else {
		reader.parsePageToDocument( options.getOption( "source" ), options.getOption( "imageDir" ), &document );
	      }
	    } catch( std::exception& exc ) {
                std::wcerr << "ocrc_transform: Reading AbbyyXML failed: " << OCRCorrection::Utils::utf8(exc.what()) << std::endl;
	      return EXIT_FAILURE;
	    }

	}
	else if( options.getOption( "inFormat" ) == "IBM_GROUNDTRUTH" ) {
	    OCRCorrection::IBMGTReader r;
	    r.parse( options.getOption( "source" ).c_str(), &document );
	}
	else if( options.getOption( "inFormat" ) == "AlignTXT" ) {
	    OCRCorrection::AlignedTXTReader p;
	    std::wcerr << "Parsing AlignTXT ... " << std::flush;
	    try {
		if( options.hasOption( "dirMode" ) ) {
		    p.parseDir( options.getOption( "source" ), options.getOption( "imageDir" ), &document );
		}
		else {
		    p.parseSingleFile( options.getOption( "source" ), &document );
		}
	    }
	    catch( std::exception& exc ) {
                std::wcerr << "ocrc_transform: Reading AlignTXT failed: " << OCRCorrection::Utils::utf8(exc.what()) << std::endl;
		return EXIT_FAILURE;
	    }
	}
	else {
	    std::wcerr << "Unknown inFormat! Use: ocrc_transform --help" << std::endl;
	    return EXIT_FAILURE;
	}
    }
    else {
 	printHelp();
 	return EXIT_FAILURE;
    }


    if( options.hasOption( "maxCandidates" ) ) {
	// not yet implemented
    }


    std::wostream* os = &std::wcout;
    if( options.hasOption( "outFile" ) ) {
      os = new std::wofstream( options.getOption( "outFile" ).c_str() ); // is not deleted at the moment, as the program ends anyway
      //os->imbue( csl::CSLLocale::Instance() );
    }


    if( options.getOption( "outFormat" ) == "DocXML" ) {
      try {
	  // this is possibly redundant if the image dir was set already while parsing the input document (TXT, Alto etc.)
	  if( options.hasOption( "imageDir" ) ) {
	      document.changeImageDir( options.getOption( "imageDir" ) );
	  }
	  OCRCorrection::DocXMLWriter writer;
	  writer.writeXML( document, *os );
      } catch( std::exception const& exc ) {
              std::wstring wideWhat(OCRCorrection::Utils::utf8(exc.what()));
              //csl::CSLLocale::string2wstring( exc.what(), wideWhat );
	  std::wcerr << "transform_docXML: error while writing: " << wideWhat;
	  return EXIT_FAILURE;
      }
    }
    else if( options.getOption( "outFormat" ) == "TXT" ) {
	document.dumpToPlaintext( *os );
    }
    else if( options.getOption( "outFormat" ) == "OCRTXT" ) {
	document.dumpOCRToPlaintext( *os );
    }
    else if( options.getOption( "outFormat" ) == "PAGES_TXT" ) {
      document.dumpToPageSeparatedPlaintext( options.getOption( "outFile" ).c_str() );
    }
    else {
	printHelp();
	return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
