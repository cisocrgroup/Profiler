#ifndef OCRCORRECTION_CREATEXML_CXX
#define OCRCORRECTION_CREATEXML_CXX OCRCORRECTION_CREATEXML_CXX


#include "./CreateXML.h"

namespace OCRCorrection {

    CreateXML::CreateXML() :
	msMatch_( 0 ),
	dictionary_( 0 )
    {
    }

    CreateXML::~CreateXML() {
	if( msMatch_ ) delete msMatch_;
    }

    Document& CreateXML::getDocument() {
	return document_;
    }

    Document* CreateXML::getDocumentPointer() {
	return &document_;
    }

    void CreateXML::initMSMatch( size_t msMatchDistance, const char* FBDic ) {
	std::wcout<<"c++::CreateXML: config MSMatch with distance="<<msMatchDistance<<", FBdic="<<FBDic<<std::endl;

	msMatch_ = new csl::MSMatch< csl::FW_BW >( msMatchDistance, FBDic );

	msMatch_->setCaseMode( csl::MSMatch< csl::FW_BW >::restoreCase );

	dictionary_ = &( msMatch_->getFWDic() );
	std::wcout<<"c++::CreateXML: exit initMSMatch"<<std::endl;

    }

    void CreateXML::newDocument( const char* filename ) {
	std::cout<<"c++::CorrSys::newDocument"<<std::endl;

	if( ! msMatch_ ) {
	    throw std::logic_error( "c++::CorrSys::call initMSMatch before generating a new document." );
	}

	document_.clear();

	Stopwatch watch;
	watch.start();

  	document_.parseTXT( filename );

	for( Document::iterator tokIter = document_.begin(); tokIter != document_.end(); ++tokIter ) {

	    // check if token is lexical
	    const wchar_t* lowercased;
	    std::wstring tmp;
	    if( iswupper( tokIter->getWOCR().at( 0 ) ) ) {
		tmp = tokIter->getWOCR();
		tmp.replace( 0, 1, 1, towctrans( tmp.at( 0 ), wctrans( "tolower" ) ) );
		lowercased = tmp.c_str();
	    }
	    else lowercased = tokIter->getWOCR().c_str();
	    if( dictionary_->lookup( lowercased, 0 ) ) {
		tokIter->setLexical( true );
	    }

	    // use MSMatch to add candidates to the token (if it is a normal token)
	    if( tokIter->isNormal() 
		// && ! tokIter->isLexical() // add this line if you DON'T want to produce cands for lexical tokens
		) {
		
		msMatch_->query( tokIter->getWOCR().c_str(), *tokIter );
	    }
	}

	AlphaScore alphaScore;
	alphaScore.setAlpha( 0.45 );
	alphaScore.addScoreToDocument( document_ );

	Stopwatch sortWatch;
	sortWatch.start();
	// this is yet another iteration and might be avoided, somehow
	for( Document::iterator tokIter = document_.begin(); tokIter != document_.end(); ++tokIter ) {
	    tokIter->sortCandidates();
	}
	std::wcout<<"OCRCorrection::CreateXML: Sorted "<<document_.getNrOfTokens()<<" tokens in "<<sortWatch.readMilliseconds()<<" milliseconds"<<std::endl;

	size_t time_all = watch.readMilliseconds();
	
	
	std::wcout<<"OCRCorrection::CreateXML: Analyzed "<<document_.getNrOfTokens()<<" tokens in "<<time_all<<" milliseconds"<<std::endl;;
    }

} // eon


#endif
