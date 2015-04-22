#ifndef OCRCORRECTION_DOCUMENT_TCC
#define OCRCORRECTION_DOCUMENT_TCC OCRCORRECTION_DOCUMENT_TCC

#include "Document.h"
#include <csl/CSLLocale/CSLLocale.h>



namespace OCRCorrection {
 
    Document::Document() 
    //locale_( std::locale( "" ), new Ctype_OldGerman( std::locale( "" ) ) )
	//compareOperator_
    {
	
	tokens_.reserve( 500 );

    }


    Document::~Document() {
	clear();
    }

    
    void Document::pushBackToken( const wchar_t* token, size_t length, bool isNormal ) {
	pushBackToken( new Token( *this, tokens_.size(), token, length, isNormal ) );
    }
    
    void Document::pushBackToken( std::wstring const& token, bool isNormal ) {
	pushBackToken( new Token( *this, tokens_.size(), token, isNormal ) );
    }


    void Document::pushBackToken( Token* newToken ) {
	if( newToken == 0 ) {
	    throw OCRCException( "OCRC::Document::pushBackToken: Tried to add 0-Pointer as new token." );
	}
	// std::wcout << "Document::pushBackToken: " << newToken->getWOCR() << std::endl;
	
	if( tokens_.capacity() == tokens_.size() ) {
	    // anything to take care of before vector is going to realloc???
	}

	

	tokens_.push_back( newToken );
	if( hasPages() ) {
	    tokens_.back()->setPageIndex( pages_.size() - 1 );
	    pages_.back()->offsetEnd_ += 1; 
	    assert( pages_.back()->offsetEnd_ == tokens_.size() );
	}
    }

    void Document::insertToken( size_t position, const wchar_t* token, size_t length, bool isNormal, size_t page ) {
	if( tokens_.capacity() == tokens_.size() ) {
	    // anything to take care of before vector is going to realloc???
	}
	tokens_.insert( tokens_.begin() + position, new Token( *this, position, token, length, isNormal ) );



	////////// thorsten 12.2.2011 /////////////////////////////////////////////////////////////////

	// current page
        pages_.at( page )->setOffsetEnd( pages_.at( page )->getOffsetEnd() + 1 );
        ++page;

	// all subsequent pages
        for( ; page < getNrOfPages(); ++page ) {
            pages_.at( page )->setOffsetBegin( pages_.at( page )->getOffsetBegin() + 1 );
            pages_.at( page )->setOffsetEnd( pages_.at( page )->getOffsetEnd() + 1 );
        }
	///////////////////////////////////////////////////////////////////////////////////////////////



	// the tokens themselves know their index in the document. These values are updated here.
	size_t count = position + 1;
	for( iterator it = begin() + position + 1; it != end(); ++it ) {
	    it->setIndexInDocument( count );
	    ++count;
	}
    }
    

    void Document::eraseToken( size_t beg, size_t end  ) {
	if( (end < beg) || (end == 0) ||  (end > getNrOfTokens()) ) {
	    throw OCRCException( "OCRC::Document::eraseToken: invalid range" );
	}

	if( beg == end ) {
	    return;
	}

       
        // make sure all erased tokens are on one page
        if( this->at( beg ).getPageIndex() != this->at( end - 1 ).getPageIndex() ) {
            throw OCRCException( "OCRC::Document::eraseToken: cannot erase across page borders" );
        }

        size_t currentPage = this->at(beg ).getPageIndex();
	
        // delete the distinct token objects on the heap
	for( std::vector< Token* >::iterator it = tokens_.begin() + beg; it != tokens_.begin() + end; ++it ) {
	    delete( *it );
	    *it = 0;
	}

        // remove pointers from document vector
	tokens_.erase( tokens_.begin() + beg, tokens_.begin() + end );

        // update begin-of-page and end-of-page index for all subsequent pages
        // Two assumptions are made here which are checked for above:
        // - all erased tokens are on the same page
        // - begin is smaller than or equal to end ;

        size_t page = currentPage;
        size_t diff = ( end -beg );

	// current page
        pages_.at( page )->setOffsetEnd( pages_.at( page )->getOffsetEnd() - diff );
        ++page;

	// all subsequent pages
        for( ; page < getNrOfPages(); ++page ) {
            pages_.at( page )->setOffsetBegin( pages_.at( page )->getOffsetBegin() - diff );
            pages_.at( page )->setOffsetEnd( pages_.at( page )->getOffsetEnd() - diff );
        }

	// the tokens themselves know their index in the document. These values are updated here.
	size_t count = beg;
	for( iterator it = begin() + beg; it != this->end(); ++it ) {
	    it->setIndexInDocument( count );
	    ++count;
	}
	
    }
    
    void Document::clear() {
        // delete Token-objects on heap
        // std::wcerr << "OCRC::Document::clear: tokens_.size() == " << tokens_.size() << std::endl;
        for( std::vector< Token* >::iterator it = tokens_.begin(); it != tokens_.end(); ++it ) {
	    // std::wcerr << "Token nr " << ( it-tokens_.begin() ) << ", address=" << *it <<  std::endl;
	    delete( *it );
	}
	tokens_.clear();

        // std::wcerr << "OCRC::Document::clear: Delete pages" << std::endl;
 	for( std::vector< Page* >::iterator it = pages_.begin(); it != pages_.end(); ++it ) {
 	    delete( *it );
 	}
 	pages_.clear();

        // std::wcerr << "OCRC::Document::clear: Done" << std::endl;

    }


    void Document::findLineBorders() {

	this->at( 0 ).setProperty( Token::LINE_BEGIN, true );
        for( size_t i = 1; i < getNrOfTokens() - 1 ; ++i ) {
	    if( this->at( i ).getWOCR() == L"\n" ) {
		this->at( i - 1 ).setProperty( Token::LINE_END, true );
		this->at( i + 1 ).setProperty( Token::LINE_BEGIN, true );
	    }
	}
	this->at( getNrOfTokens() - 1 ).setProperty( Token::LINE_END, true );
    }


    void Document::findHyphenation() {
	std::wstring hyphenationMarks = std::wstring( L"-\u00AC" );
	std::wstring mergedWord;
	
	
	if( getNrOfTokens() < 4 ) return;
	
	/*
	 *  hyphe       ¬          \n         nation
	 *   i -2      i-1         i           i+1
	 *
	 */
        for( size_t i = 2; i < getNrOfTokens()-1 ; ++i ) {
	    if( this->at( i ).getWOCR() == L"\n" ) {
		if( ( this->at( i - 1 ).getWOCR().size() == 1 ) && 
		    ( hyphenationMarks.find( this->at( i - 1 ).getWOCR().at( 0 ) ) != std::wstring::npos ) ) {

		    mergedWord = this->at( i - 2 ).getWOCR() + this->at( i + 1 ).getWOCR();
		
		    this->at( i - 2 ).setProperty( Token::HYPHENATION_1ST, true );
		    this->at( i - 2 ).setHyphenationMerged( mergedWord );
		    this->at( i - 1 ).setProperty( Token::HYPHENATION_MARK, true );
		    this->at( i + 1 ).setProperty( Token::HYPHENATION_2ND, true );
		    this->at( i + 1 ).setHyphenationMerged( mergedWord );

		}
	    }
	    
	}
    }

    

    void Document::dumpOCRToPlaintext( std::wostream& fo ) {
	for( iterator it = begin(); it != end(); ++it ) {
	    fo << (*it).getWOCR();
	}
    }

    void Document::dumpToPlaintext( std::wostream& fo ) {
	for( iterator it = begin(); it != end(); ++it ) {
	    fo << (*it).getWDisplay();
	}
    }

    void Document::dumpToPlaintext( const char* filename ) {
	std::cerr << "Dump to:" <<filename << std::endl;
	std::wofstream fo( filename );
	fo.imbue( csl::CSLLocale::Instance() );
	dumpToPlaintext( fo );
	fo.close();
    }

    void Document::dumpToPageSeparatedPlaintext( const char* directory_to) {
      for( PageIterator pageIt = pagesBegin(); pageIt != pagesEnd(); ++pageIt ) {

	 int from = pageIt->imageFile_.find_last_of("/") + 1;
	 int to   = pageIt->imageFile_.find_last_of(".") ;

	 int length = to - from;

	 std::string filename = pageIt->imageFile_.substr(from , length );
	 filename = "/" + filename + ".txt";
	 
	 char* buffer = new char[strlen(directory_to)+strlen(filename.c_str())];
	 strcpy(buffer, directory_to);
	 strcat(buffer, filename.c_str());

	 std::wofstream fo( buffer );
	 fo.imbue( csl::CSLLocale::Instance() );
	 for( iterator it = pageIt->begin(); it != pageIt->end(); ++it) {
	   fo << (*it).getWDisplay();
	 }
	 fo.close();

	 free( buffer );
      }
    }

    void Document::print( std::wostream& stream ) {
	if( ! hasPages() ) {
	    for( iterator it = begin(); it != end(); ++it ) {
		(*it).print( stream );
	    }
	}
	else {
	    for( PageIterator pageIt= pagesBegin(); pageIt != pagesEnd(); ++pageIt ) {
		std::wstring wide_imageFile;
		csl::CSLLocale::string2wstring( pageIt->imageFile_, wide_imageFile );
		stream << "PAGE: image=" << wide_imageFile << std::endl;
		
		for( iterator it = pageIt->begin(); it != pageIt->end(); ++it ) {
		    (*it).print( stream );
		}
		stream << "PAGE END" << std::endl;
	    }
	}
    }

    
    bool Document::integrityCheck() const {

	// // check for consistency of page assignment
	// size_t pageCount = 0;
	// for( PageIterator pageIt= pagesBegin(); pageIt != pagesEnd(); ++pageIt ) {
	//     for( iterator it = pageIt->begin(); it != pageIt->end(); ++it ) {
	// 	if( pageCount != it->getPageIndex() ) {
	// 	    std::wcout << "Token: '" << it->getWOCR() << "' has pageIndex " << it->getPageIndex() 
	// 		       << ", pageCount says " << pageCount << std::endl;
	// 	}
	//     }
	//     ++pageCount;
	// }

	return true;
    }





} // ns OCRCorrection

#endif
