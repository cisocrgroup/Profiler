#ifndef OCRC_ALIGNED_TXT_READER_CXX
#define OCRC_ALIGNED_TXT_READER_CXX OCRC_ALIGNED_TXT_READER_CXX


#include "./AlignedTXTReader.h"

namespace OCRCorrection {

    AlignedTXTReader::AlignedTXTReader() :
	locale_( CSL_UTF8_LOCALE ) {

    }

    void AlignedTXTReader::parse( std::string const& inFile, Document* document ) {

	std::wifstream fi;
	fi.open( inFile.c_str() );
	
	if( ! fi ) {
	    throw OCRCException( "OCRC::Document::parseAlignedTXT::Could not open input file" );
	}
	
	std::wstring line;

	std::wstring wOrig;
	std::wstring wOCR;
	std::wstring preOrig, postOrig, preOCR, postOCR;

	size_t lineCount = 0;
	while( getline( fi, line).good() ) {
	    
	    if( lineCount == 0 ) { // search for utf8 byte-order marks!
		size_t pos = 0;
		while( ( pos = line.find_first_of( 65279, pos ) ) != std::wstring::npos ) {
		    line.erase( pos, 1 );
		    ++pos;
		}
	    }
	    size_t delimPos = line.find( L'\t' );
	    if( delimPos == std::wstring::npos ) {
		throw OCRCException( "OCRC::AlignedTXTReader::parse::No separator found in input file" );
	    }
	    
	    wOrig = line.substr( 0, delimPos );
	    wOCR  = line.substr( delimPos + 1 );

	    //std::wcout << wOrig << "\t" << wOCR << std::endl;

	    if( ( wOrig == L"###NEWLINE###" ) && ( wOCR == L"###NEWLINE###" ) ) {
		Token* tok = new Token( *document, document->getNrOfTokens(), std::wstring( L"\n" ), false );
		tok->initGroundtruth();
		tok->getGroundtruth().setWOrig( L"\n" );
		document->pushBackToken( tok );
	    }
	    else { // regular lines

		size_t pos = wOrig.find( L"###NEWLINE###" );
		if( pos != wOrig.npos ) {
		    std::wcerr << "OCRC::AlignedTXTReader: WARNING: Found wOrig with '###NEWLINE##' wOCR=" << wOrig << std::endl;
		    wOrig.erase( pos, 13 );
		}

		pos = wOCR.find( L"###NEWLINE###" );
		if( pos != wOCR.npos ) {
		    std::wcerr << "OCRC::AlignedTXTReader: WARNING: Found wOCR with '###NEWLINE##' wOCR=" << wOCR << std::endl;
		    wOCR.erase( pos, 13 );
		}

		preOrig = postOrig = preOCR =  postOCR = L"";

//	    std::wcout << "line=" << line <<  std::endl; // DEBUG
//	    std::wcout << "Worig='" << wOrig <<"', wOCR='" << wOCR <<  "'" << std::endl; // DEBUG
	    
	    
		document->cleanupWord( &wOrig, &preOrig, &postOrig );
		document->cleanupWord( &wOCR, &preOCR, &postOCR );
	    
		// std::wcout << "Worig='" << wOrig <<"', wOCR='" << wOCR <<  "'" << std::endl; // DEBUG
	    
		// NOTE: not-normal prefixes of wOrig are thrown away if there's no such prefix in wOCR
		if( ! preOCR.empty()  ) {
		    Token* tok = new Token( *document, document->getNrOfTokens(), preOCR, false );
		    tok->initGroundtruth();
		    tok->getGroundtruth().setWOrig( preOrig );
		    document->pushBackToken( tok );
		
		}

	    
		Token* tok = 0;
	    
		size_t length_ocr = 0;
		wchar_t const* pos_ocr = wOCR.c_str();
		bool isNormal_ocr = false;
		size_t length_orig = 0;
		wchar_t const* pos_orig = wOrig.c_str();
		bool isNormal_orig = false;
		std::wstring subOCR, subOrig;

		while( ( *pos_ocr != 0 ) || ( *pos_orig != 0 )  ) {
		    //std::wcout << "ocr='" << pos_ocr << "'" << std::endl;
		    //std::wcout << "orig='" << pos_orig << "'" << std::endl;
		    if( *pos_ocr != 0 ) {
			length_ocr = Document::getBorder( pos_ocr, &isNormal_ocr );
			subOCR = wOCR.substr( pos_ocr - wOCR.c_str(), length_ocr );
			pos_ocr += length_ocr;
		    }
		    else {
			subOCR = L"";
		    }
		    
		    if( *pos_orig != 0 ) {
			length_orig = Document::getBorder( pos_orig, &isNormal_orig );
			subOrig = wOrig.substr( pos_orig - wOrig.c_str(), length_orig );
			pos_orig += length_orig;
		    }
		    else {
			subOrig = L"";
		    }
		    
		    tok = new Token( *document, document->getNrOfTokens(), subOCR, isNormal_ocr );
		    tok->initGroundtruth();
		    tok->getGroundtruth().setWOrig( subOrig );
		    //tok->setSplitMerge( true );
		    document->pushBackToken( tok );
		    
		}

		if( postOCR.empty() ) postOCR = L" ";
		if( postOrig.empty() ) postOrig = L" ";
	    
		tok = new Token( *document, document->getNrOfTokens(), postOCR, false );
		tok->initGroundtruth();
		tok->getGroundtruth().setWOrig( postOrig );
		document->pushBackToken( tok );
	    } // not a ###NEWLINE###	    

	    ++lineCount;

	} // while( getline )
	document->pushBackToken( L"\n", 1, false );
	
	std::wcerr<<"OCRC::AlignedTXTReader::parse: Finished, "<< document->getNrOfTokens() << " tokens." << std::endl;
    }



    void AlignedTXTReader::parseSingleFile( std::string const& inFile, Document* document ) {
	document->newPage( "_NO_IMAGE_DIR_" );
	parse( inFile, document );
    }


    void AlignedTXTReader::parseDir( std::string const& init_txtDir, std::string const& imageDir, Document* doc ) {
	std::string xmlDir( init_txtDir );
	if( xmlDir.at( 0 ) == '~' ) {
	    xmlDir.replace( 0, 1, getenv( "HOME" ) );
	}
	
	std::wstring wide_xmlDir;
	csl::CSLLocale::string2wstring( xmlDir, wide_xmlDir );


	DIR *pDIR = opendir( xmlDir.c_str() );
	struct dirent *pDirEnt;
	
	std::vector< std::string > dirEntries;
	
	/* Get each directory entry */
	pDirEnt = readdir( pDIR );
	while ( pDirEnt != NULL ) {
	    dirEntries.push_back( pDirEnt->d_name );
	    pDirEnt = readdir( pDIR );
	}
	closedir( pDIR );

	// sort entries
	std::sort( dirEntries.begin(), dirEntries.end() );

	// process all entries
	for( std::vector< std::string >::iterator entry = dirEntries.begin(); entry != dirEntries.end(); ++entry ) {
	    if( *entry == "." || *entry == ".." ) {
		continue;
	    }
	    size_t pos = entry->rfind( ".txt" );
	    // only .xml files
	    if( ( pos != std::string::npos ) && ( pos == (entry->size() - 4 ) )  ) {
		
		std::string inFile = xmlDir + std::string( "/" ) +  *entry;
		
		// create the image path
		std::string imageFile = imageDir + std::string( "/" ) +  entry->substr( 0, (entry->size() - 4 ) ) + std::string( ".tif" );
		
		doc->newPage( imageFile );
		parse( inFile.c_str(), doc );
	    }
	}

	std::wcerr << "OCRC::AlignedTXTReader::parseDir parsed directory " << wide_xmlDir << ", " 
		   << doc->getNrOfTokens() << " tokens, " 
		   << doc->getNrOfPages() << " pages."
		   << std::endl;
        
    
    }










}


#endif
