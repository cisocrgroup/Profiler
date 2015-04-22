#ifndef OCRCORRECTION_TOKEN_TCC
#define OCRCORRECTION_TOKEN_TCC OCRCORRECTION_TOKEN_TCC

#include "Token.h"



/******************************************************/
/***********  IMPORTANT *******************************/

/*   DO NOT INCLUDE THIS HEADER FILE ANYWHERE         */

/*   Include Document.h instead                       */
/******************************************************/

namespace OCRCorrection {

    Token::Token( Document& document, size_t indexInDocument, const wchar_t* wOCR, size_t length, bool isNormal ) :
	myDocument_( document ),
	wCorr_(),
	bitProperties_( 0 ),
	indexInDocument_( indexInDocument ),
	myPageIndex_( size_t( -1 ) ),
	groundtruth_( 0 ),
	tokenImageInfoBox_( 0 ),
	candidates_( 0 ),
	nrOfCandidates_( 0 )
    {
	setNormal( isNormal );
	setSuspicious( false );
	setCorrected( false );
	setDontTouch( false );
	setWOCR( std::wstring( wOCR, length ) );
    }

    Token::Token( Document& document, size_t indexInDocument, std::wstring const& wOCR, bool isNormal ) :
	myDocument_( document ),
	wCorr_(),
	bitProperties_( 0 ),
	indexInDocument_( indexInDocument ),
	myPageIndex_( size_t(-1) ),
	groundtruth_( 0 ),
	tokenImageInfoBox_( 0 ),
	candidates_( 0 ),
	nrOfCandidates_( 0 ) 


    {
	setSuspicious( false );
	setCorrected( false );
	setDontTouch( false );
	setWOCR( wOCR );
	setNormal( isNormal);
    }

    Token::Token( Document& document, size_t indexInDocument, bool isNormal ) :
	myDocument_( document ),
	bitProperties_( 0 ),
	indexInDocument_( indexInDocument ),
	myPageIndex_( size_t(-1) ),
	groundtruth_( 0 ),
	tokenImageInfoBox_( 0 ),
	candidates_( 0 ),
	nrOfCandidates_( 0 ) 


    {
	setNormal( isNormal);
	setSuspicious( false );
	setCorrected( false );
	setDontTouch( false );
    }

    Token::Token( Token const& other ) :
	myDocument_( other.getDocument() ),
	bitProperties_( 0 ),
	indexInDocument_( other.indexInDocument_ ),
	myPageIndex_( size_t(-1) ),
	groundtruth_( 0 ),
	tokenImageInfoBox_( 0 ),
	candidates_( 0 ),
	nrOfCandidates_( 0 ) 

    {
	setNormal( other.isNormal() );
	setSuspicious( other.isSuspicious() );
	setCorrected( other.isCorrected() );
	setDontTouch( false );
	setWOCR( other.getWOCR() );

	if( other.hasGroundtruth() ) {
	    groundtruth_ = new Groundtruth( other.getGroundtruth() );
	}
    }


    Token::~Token() {
	if( tokenImageInfoBox_ ) {
	    delete( tokenImageInfoBox_ );
	    tokenImageInfoBox_ = 0;
	}
	if( groundtruth_ ) {
	    delete( groundtruth_ );
	    groundtruth_ = 0;
	}

	removeCandidates();
    }

    
    // void Token::cutCandidates( size_t newSize ) {
    // 	if( candidates_ ) {
    // 	    CandidateChain* c = candidates_;
    // 	    CandidateChain* nextC = 0;

    // 	    size_t count = 0;
    // 	    // iterate newSize candidates, do NOT delete
    // 	    while( c != 0 && ( count+1 < newSize ) ) {
    // 		c = c->getNext(); 
    // 		++count;
    // 	    }
    // 	    // delete the rest of the candidates
    // 	    while( c != 0 ) {
    // 		nextC = c->getNext(); 
    // 		delete( c );
    // 		c = nextC;
    // 	    }
    // 	    candidates_ = 0;
    // 	    nrOfCandidates_ = newSize;
    // 	}
    // }

    void Token::removeCandidates() {
	if( candidates_ ) {
	    CandidateChain* c = candidates_;
	    CandidateChain* nextC = 0;
	    while( c != 0 ) {
		nextC = c->getNext(); 
		delete( c );
		c = nextC;
	    }
	    candidates_ = 0;
	    nrOfCandidates_ = 0;
	}
	
    }


    void Token::setCoordinates( size_t l, size_t t, size_t r, size_t b ) {
	if( tokenImageInfoBox_ == 0) {
	    tokenImageInfoBox_ = new TokenImageInfoBox();
	}
	tokenImageInfoBox_->setCoordinate_Left( l );
	tokenImageInfoBox_->setCoordinate_Top( t );
	tokenImageInfoBox_->setCoordinate_Right( r );
	tokenImageInfoBox_->setCoordinate_Bottom( b ); 
    }
    
    

    void Token::addCharacter( Character const& c ) {
	//std::wcout << c.getChar() << std::endl;

	if( c.getChar() == 0 ) throw OCRCException( "OCRC::Token::addChar: Do not insert \\0 char" );

	if( wOCR_.empty() && // first char of the Token
	    std::use_facet< std::ctype< wchar_t > >( csl::CSLLocale::Instance() ).is( std::ctype_base::upper, c.getChar() ) ) {
	    setCapitalized();
	}
	

	wOCR_ += c.getChar();
	wOCR_lc_ += std::tolower( c.getChar(), csl::CSLLocale::Instance() );
	
	// if the new character is suspicious, this makes the whole word suspicious
	if( c.isAbbyySuspicious() ) {
	  abbyySpecifics_.setSuspicious( true );
	}
	
 	size_t templeft = c.getLeft();
 	size_t tempright = c.getRight();
 	size_t temptop = c.getTop();
 	size_t tempbottom = c.getBottom();

	// character doesn't have 0 coordinates ( i.e. newline )
	if( tempright > 0 ) {
	    if( tokenImageInfoBox_ == 0) { // first character of word
		tokenImageInfoBox_ = new TokenImageInfoBox();
		tokenImageInfoBox_->setCoordinate_Right( tempright );
		tokenImageInfoBox_->setCoordinate_Left( templeft );
		tokenImageInfoBox_->setCoordinate_Top( temptop );
		tokenImageInfoBox_->setCoordinate_Bottom( tempbottom ); 
	    } else {
		if( temptop < tokenImageInfoBox_->getCoordinate_Top()) {
		    tokenImageInfoBox_->setCoordinate_Top( temptop );
		}
		if( tempbottom > tokenImageInfoBox_->getCoordinate_Bottom()) {
		    tokenImageInfoBox_->setCoordinate_Bottom( tempbottom );
		}
		if( tempright > tokenImageInfoBox_->getCoordinate_Right()) {
		    tokenImageInfoBox_->setCoordinate_Right( tempright );
		}
	    }
	}

	// copies the object, but who cares ...
	characters_.push_back( c );
    }



    void Token::setWOCR( std::wstring const& w ) {
	wOCR_ = w;
	
	if( ( !wOCR_.empty() ) && std::use_facet< std::ctype< wchar_t > >( csl::CSLLocale::Instance() ).is( std::ctype_base::upper, wOCR_.at( 0 ) ) ) {
	    setCapitalized();
	}
	
	wOCR_lc_ = wOCR_;
	for( std::wstring::iterator c = wOCR_lc_.begin(); c != wOCR_lc_.end(); ++c ) {
	    
            *c = std::tolower( *c, csl::CSLLocale::Instance() );
            
	}
    }
    

    size_t Token::mergeRight() {
	if( indexInDocument_ + 1 >= myDocument_.getNrOfTokens() ) {
	    return 0;
	}
	
//	Token* rightToken = &( myDocument_.at( indexInDocument_ + 1 ) );
	bool skipSpace = false;
	// decide if immediate neighbour should be skipped, 
	// e.g. if it contains just whitespace
	if( myDocument_.at( indexInDocument_ + 1 ).getWDisplay() == L" " ) {
	    if( indexInDocument_ + 2 >= myDocument_.getNrOfTokens() ) {
		return 0;
	    }
	    skipSpace = true;
	}

	
	size_t merged = mergeRight( (skipSpace ? 2 : 1 ) ); 

	return merged;
    }



    size_t Token::mergeRight( size_t n ) {
	Token* rightToken = 0;


	size_t i = 0;
	for( ; i < n; ++i ) {
	    rightToken = &( myDocument_.at( indexInDocument_ + (i + 1) ) );

	    // stop at newlines
	    if( rightToken->getWDisplay() == L"\n" ) {
		break;
	    }
	    
	    if( getTokenImageInfoBox() && rightToken->getTokenImageInfoBox() ) {
		// this might maybe not be the case for linebreaks 
		if( getTokenImageInfoBox()->getCoordinate_Right() < rightToken->getTokenImageInfoBox()->getCoordinate_Left() ) {
		    tokenImageInfoBox_->setCoordinate_Right( rightToken->getTokenImageInfoBox()->getCoordinate_Right() );
		}
	    }

	    // FUNZT NICHT !!!
	    if( rightToken->getWDisplay() != L" " ) {
		setWCorr( getWDisplay() + rightToken->getWDisplay() );
	    }
	    else {
		setWCorr( getWDisplay() );
	    }
	    

	    wOCR_ += rightToken->getWOCR();
	    wOCR_lc_ += rightToken->getWOCR_lc();
	    
	} // for all tokens to be merged

	/// @todo Find a way to decide if the merged token is normal, and to compute new candidates
	setNormal( true ); 

	myDocument_.eraseToken( indexInDocument_ + 1, indexInDocument_ + 1 + i );

	return i;
    }


    void Token::mergeHyphenation() {
        // first, check how many tokens are left in the current line
        size_t linebreakIndex = indexInDocument_ + 1;
        while( myDocument_.at( linebreakIndex ).getWOCR() != L"\n" ) {
            ++linebreakIndex;
        }
        if( linebreakIndex - indexInDocument_ > 3 ) {
            throw OCRCException( "OCRC::Token::mergeHyphenation:merge not possible");
        }

        // for all tokens of the line up to the newline
        for( size_t index = indexInDocument_ + 1; index != linebreakIndex; ++ index ) {
            //merge wOCR_
            wOCR_ += myDocument_.at( index ).getWOCR();

            //merge wOCR_lc_
            wOCR_lc_ += myDocument_.at( index ).getWOCR_lc();

            //merge coordinates
            if( getTokenImageInfoBox() && myDocument_.at( index ).getTokenImageInfoBox() ) {
                // Here, this might be paranoia. This should always be true!
                if( getTokenImageInfoBox()->getCoordinate_Right() < myDocument_.at( index ).getTokenImageInfoBox()->getCoordinate_Left() ) {
                    tokenImageInfoBox_->setCoordinate_Right( myDocument_.at( index ).getTokenImageInfoBox()->getCoordinate_Right() );
                }
            }
        }

        myDocument_.eraseToken(indexInDocument_+1, linebreakIndex );

        // the first token of the next line. Calculate new, because eraseToken made a mess of old indices.
        // It's the *this token, then the newline, then the first token of the next line.
        size_t index = getIndexInDocument() + 2;
        wOCR_ += myDocument_.at( index ).getWOCR();
        wOCR_lc_ += myDocument_.at( index ).getWOCR_lc();
        myDocument_.at( index ).setSplitMerge( true );
        myDocument_.at( index ).setCorrected( true );
        myDocument_.at( index ).setWCorr( L"" );

    }


    size_t Token::handleSplit( std::wstring const& str ) {
        bool isNormal = false;
        // find 1st chunk
        size_t start = 0;
        size_t size = myDocument_.getBorder( str.c_str(), &isNormal );
	setWCorr( str.substr( start, size ) ); // start is always 0 here.
        start += size;
	
        size_t addedWords = 0;
        while( ( size = Document::getBorder( str.c_str() + start, &isNormal ) ) != 0 ) {
            /// @todo Find a way to decide if the split tokens are normal, and to compute new candidates
            std::wstring newStr = str.substr( start, size );
            //std::wcerr << "Add token: '" << newStr << "'" << std::endl;
            ++addedWords;
            myDocument_.insertToken( indexInDocument_ + addedWords, L"", 0, isNormal, getPageIndex());
            myDocument_.at( indexInDocument_ + addedWords ).setWCorr( newStr.c_str() );
	    myDocument_.at( indexInDocument_ + addedWords ).setCorrected();
	    myDocument_.at( indexInDocument_ + addedWords ).setPageIndex( getPageIndex() );
            start += size;
        }

	return addedWords;
    }

    void Token::addCandidate( Candidate const& initCand ) {
	CandidateChain* candItem = new CandidateChain( initCand );
	
	// if token is capitalized, also capitalize the candidate
	if( this->isCapitalized() && !candItem->getCandidate().getString().empty() ) {
	    candItem->getCandidate().getString().at( 0 ) =
		std::use_facet< std::ctype< wchar_t > >( csl::CSLLocale::Instance() ).toupper( candItem->getCandidate().getString().at( 0 ));
	}
	if( candidates_ == 0 ) {
	    candidates_ = candItem;
	}
	else {
	    CandidateChain* c = candidates_;
	    while( c->getNext() != 0 ) c = c->getNext();
	    c->setNext( candItem );
	}
	++nrOfCandidates_;
	
    }

    void Token::print( std::wostream& stream ) {
	if( isNormal() ) 
	    stream<< "("<<getIndexInDocument()<<")["<< wOCR_.c_str()<<"]";
	else
	    stream<< "("<<getIndexInDocument()<<"){"<< wOCR_.c_str()<<"}";

	if( isDontTouch() ) {
	    stream << "\tDONTTOUCH";
	}

	stream<<std::endl;

    }


} // ns OCRCorrection

#endif
