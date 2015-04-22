#ifndef OCRC_MERGEDGROUNDTRUTHREADER_CXX
#define OCRC_MERGEDGROUNDTRUTHREADER_CXX OCRC_MERGEDGROUNDTRUTHREADER_CXX

#include<Exceptions.h>
#include "./MergedGroundtruthReader.h"

namespace OCRCorrection {

    bool MergedGroundtruthReader::bsbIDCheck( std::wstring const& abbyyFilename, std::wstring const& alignedTXTFilename ) const {
	//                         01234567890123456
	// bsb ids look like this: bsb00001830_00521 
	std::wstring abbyyBase = abbyyFilename.substr( abbyyFilename.find_last_of( '/' ) + 1 , 17 );
	std::wstring alignedTXTBase = alignedTXTFilename.substr( alignedTXTFilename.find_last_of( '/' ) + 1, 17 );
	if( ! (
		abbyyBase.find( L"bsb" )  == 0 &&
		abbyyBase.at( 11 ) == '_' &&
		alignedTXTBase == abbyyBase 
		)
	    )  {
	    // std::wcerr << alignedTXTBase  << L"!=" << abbyyBase  << std::endl;
	    return false;
	}
	return true;
    }
    

    void MergedGroundtruthReader::mergeDirectories( char const* abbyyXMLDir, char const* alignedTXTDir, Document* abbyyDoc ) {
	if( ! abbyyDoc->empty() ) {
	    throw OCRCException( "MergedGroundtruthReader::mergeDirectories: doc should be empty for the beginning." );
	}
	AbbyyXmlParser abbyyXMLParser;
	
	abbyyXMLParser.parseDirToDocument( abbyyXMLDir, "_NO_IMAGE_DIR_", abbyyDoc );
	
	AlignedTXTReader alignedTXTReader;
	Document alignedTXTDoc;
	alignedTXTReader.parseDir( alignedTXTDir, "_NO_IMAGE_DIR_", &alignedTXTDoc );

	mergeDocuments( abbyyDoc, alignedTXTDoc );
	
	mergeStatistic_.print( std::wcerr );

	DocXMLWriter writer;
	writer.writeXML( *abbyyDoc, std::wcout );
	exit( 0 ); 
    }


    void MergedGroundtruthReader::mergeDocuments( Document* abbyyDoc, Document& alignedTXTDoc ) {
	Document::PageIterator abbyyPage = abbyyDoc->pagesBegin();
	Document::PageIterator alignedTXTPage = alignedTXTDoc.pagesBegin();
	size_t pageCount = 0;
	for( ; abbyyPage != abbyyDoc->pagesEnd(); ++abbyyPage, ++alignedTXTPage ) {
	    if( alignedTXTPage == alignedTXTDoc.pagesEnd() ) {
		throw OCRCException( 
		    std::string ( "OCRC::MergedGroundtruthReader::mergeDirectories: Too few files in alignedTXT-directory." )
		    + "Next abbyy file is for image " + abbyyPage->getImageFile() 
		);
	    }
	    std::wstring wideAbbyyFilename;
	    std::wstring wideAlignedTXTFilename;
	    csl::CSLLocale::string2wstring( abbyyPage->getImageFile(), wideAbbyyFilename );
	    csl::CSLLocale::string2wstring( alignedTXTPage->getImageFile(), wideAlignedTXTFilename );

	    std::wcerr << "New page: " << pageCount << ", abbyyFile=" << wideAbbyyFilename << ", alignedTXTFile=" << wideAlignedTXTFilename << std::endl;

	    if( ! bsbIDCheck( wideAbbyyFilename, wideAlignedTXTFilename ) ) {
		throw OCRCException( 
		    std::string( "OCRC::MergedGroundtruthReader::mergeDirectories: bsbIDCheck says: annotated image files seem not to correspond: abbyy=" )
		    + abbyyPage->getImageFile() + " <-> align_txt=" + alignedTXTPage->getImageFile() );
	    }

	    //std::wcerr << "Abbyy:" << abbyyPage->getImageFile() << " != AlignedTXT:" << alignedTXTPage->getImageFile() << std::endl;
// 	    if( abbyyPage->getImageFile() != alignedTXTPage->getImageFile() ) {
// 		std::string message = 
// 		    std::string( "MergedGroundtruthReader::mergeDirectories: files are not properly aligned" )
// 		    + "Abbyy:" + abbyyPage->getImageFile() + " != AlignedTXT:" + alignedTXTPage->getImageFile();
// 		throw OCRCException( message );
// 	    }
	    mergeAreas( abbyyPage->begin(),abbyyPage->end(), alignedTXTPage->begin(), alignedTXTPage->end() );
	    ++pageCount;
	}
	if( alignedTXTPage != alignedTXTDoc.pagesEnd() ) {
	    throw OCRCException( 
		std::string ( "OCRC::MergedGroundtruthReader::mergeDirectories: Too few files in abbyy-directory. Next alignedTXT file is for image " ) 
		+ alignedTXTPage->getImageFile()
		);
	}

	mergeStatistic_.print( std::wcerr );

    }


    void MergedGroundtruthReader::mergeAreas( Document::iterator abbyyBegin, Document::iterator abbyyEnd, 
					      Document::iterator alignedTXTBegin, Document::iterator alignedTXTEnd ) {

	std::locale loc_( CSL_UTF8_LOCALE );


	// make sure all abbyyTokens have Groundtruth space allocated
	for( Document::iterator t = abbyyBegin; t != abbyyEnd; ++t ) {
	    if( ! t->hasGroundtruth() ) t->initGroundtruth();
	}
	
	Document::iterator abbyyToken = abbyyBegin;
	Document::iterator alignedToken = alignedTXTBegin;
	

	while( abbyyToken != abbyyEnd ) {

	    if( alignedToken == alignedTXTEnd ) {
		return;
	    }
	    
	    while( abbyyToken->getWOCR_lc() == L"\n" ) {
		++abbyyToken;
		if( abbyyToken == abbyyEnd ) {
		    return;
		}
	    }

	    while( abbyyToken->getWOCR_lc().empty() || ! std::use_facet < std::ctype<wchar_t > > (loc_).is ( std::ctype_base::alnum, abbyyToken->getWOCR_lc().at( 0 ) ) ) {
		std::wcerr << "skip abbyy=" << abbyyToken->getWOCR_lc() << std::endl;
		++abbyyToken;
		if( abbyyToken == abbyyEnd ) {
		    return;
		}
	    }
	    
	    while( alignedToken->getWOCR_lc().empty() || 
		   ( abbyyToken->getWOCR_lc().at( 0 ) != alignedToken->getWOCR_lc().at( 0 ) ) ) {
		std::wcerr << "skip doc=" << alignedToken->getWOCR_lc() << ", abby=" << abbyyToken->getWOCR_lc() <<  std::endl;
		++alignedToken;
		if( alignedToken == alignedTXTEnd ) {
		    return;
		}
	    }
	    
	    std::wcerr << "a=" << abbyyToken->getWOCR_lc() << ",d=" << alignedToken->getWOCR_lc() << std::endl;
	    mergeStatistic_.registerPair( *abbyyToken, *alignedToken );


	    abbyyToken->getGroundtruth() = alignedToken->getGroundtruth();
	    ++abbyyToken; ++alignedToken;
	}
    }



} // eon

#endif
