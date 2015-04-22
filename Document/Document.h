#ifndef OCRCORRECTION_DOCUMENT_H
#define OCRCORRECTION_DOCUMENT_H OCRCORRECTION_DOCUMENT_H

#include<vector>
#include<set>
#include<iostream>
#include<fstream>
#include<exception>
#include<wctype.h>
#include<csl/CSLLocale/CSLLocale.h>

#include "../Exceptions.h"

namespace OCRCorrection {
    // forward declarations /////////////
    class Token;
    class Document;
}

#include "../Token/Token.h"

namespace OCRCorrection {


    /**
     * @brief
     * Represents a document. It is basically a vector of tokens, and some
     * extra information
     *
     * @author Uli Reffle
     */
    class Document {
    public:

	/**
	 * @brief A very basic constructor.
	 */
	Document();
	
	/**
	 * As Tokens are created on the heap, they have to be destroyed here.
	 */
	~Document();

	/**
	 * @brief Create a new Token object with the parameters given and add it to the end
	 * of the array of Tokens.
	 *
	 * As new token the first @c length characters of @c token are taken.
	 */
	void pushBackToken( const wchar_t* token, size_t length, bool isNormal );

	/**
	 * @brief Create a new Token object with the parameters given and add it to the end
	 * of the array of Tokens.
	 *
	 */
	void pushBackToken( std::wstring const& token, bool isNormal );

	/**
	 * @brief Add token object referenced by @c newToken to the end of the array of tokens. NOTE THAT the object itself
	 *        not a copy is added. So @c newToken must be created on the heap. class Document takes care of the destruction.
	 */
	void pushBackToken( Token* newToken );

	/**
	 * @brief Create a new Token object with the parameters given and add it in front of
	 * the Token with index %position
	 */
	inline void insertToken( size_t position, const wchar_t* token, size_t length, bool isNormal, size_t pageIndex );
	
	/**
	 * @brief Erase a sequence of tokens as specified by the two positions. These positions are to be
	 *        understood similar to iterators: the tokens at position @c begin is the first to be deleted,
	 *        the token at position @c end is the one AFTER the last token to be deleted.
	 */
	void eraseToken( size_t begin, size_t end );

	/**
	 * @brief Remove all tokens to obtain a new virgin document
	 *
	 * @todo This method crashes if Document is created with AbbyyXmlParser. That's why it is not called
	 *       in the destructor, although it really should be.
	 */
	void clear();

	
	/**
	 * @brief Set LINE_BEGIN and LINE_END token properties
	 *
	 */
	void findLineBorders();


	/**
	 * @brief analyze all tokens of the document and set all token properties related to hyphenation.
	 */
	void findHyphenation();


	/**
	 * @brief Returns a reference of the Token at index i
	 * @param i an index into the Token array
	 */
	inline Token& at( size_t i );

	/**
	 * const version of getter function at().
	 */
	inline const Token& at( size_t i ) const;
	
	/**
	 * @brief Dumps a plaintext version of the ocr result. (Not necessarily the original version! See deatils of the description!)
	 *
	 * There's one pitfall with this method, and it concerns deleted tokens. According to the current implementation, These tokens
	 * will not be printed by this method.
	 */
	void dumpOCRToPlaintext( std::wostream& fo );

	/**
	 * @todo Dumps a plaintext version of the document, taking corrections into account.
	 */
	void dumpToPlaintext( std::wostream& fo );

	/**
	 * @see dumpToPlaintext( std::wostream& fo )
	 */
	void dumpToPlaintext( const char* filename );

	/**
	 * @see dumpToPlaintext( std::wostream& fo )
	 */
	void dumpToPageSeparatedPlaintext( const char* directory_to );

	/**
	 * print a debug representation of the text
	 */
	void print( std::wostream& stream = std::wcout );

	/**
	 * returns the nr of tokens in the document
	 */
	inline size_t getNrOfTokens() const;

	/**
	 * @brief returns the nr of pages in the document
	 */
	inline size_t getNrOfPages() const;

	/**
	 * @brief returns true iff the document contains 0 tokens.
	 */
	inline bool empty() const;


	/**
	 * @brief clears all special characters from beginning or end of a string, 
         * and converts it to lower case.
	 */
	inline static void cleanupWord( std::wstring* word, std::wstring* pre, std::wstring* post );

	/**
	 * returns the number of chars from pos with identical word/non-word status
	 * @param[in] pos
	 * @param[out] isNormal
         * @return the number of chars from pos with identical word/non-word status, or 0 if the empty string is passed
	 *
	 * @deprecated Use findBorder() instead, which offers the same functionality in a more STL-like way.
	 */
	inline static size_t getBorder( wchar_t const* pos, bool* isNormal );

	/**
	 *
	 * Note that the word end is always treated as one last border.
	 */
	inline static size_t findBorder( std::wstring const& str, size_t offset, bool* isNormal );


	/**
	 * @brief This iterator class is used to iterate through all Token s of the Document
	 *
	 * It wraps a std::vector< Token* >::iterator: the underlying vector contains not Tokens but pointers
	 * to Tokens.
	 */
	class iterator {
	public:
	    // Document may use the private constructor
	    friend class Document;

	    /**
	     * @brief This calls the default constructor for the wrapped iterator
	     */
	    iterator() {
	    }

	
	    bool operator==( const iterator& other ) const {
		return it_ == other.it_;
	    }
	    bool operator!=( const iterator& other ) const {
		return ! ( *this == other );
	    }

	    Token& operator*() {
		return **it_;
	    }
	    
	    Token* operator->() {
		return *it_;
	    }

	    iterator& operator++() {
		++it_;
		return *this;
	    }
	    
	    iterator& operator+( size_t s ) {
		it_ += s;
		return *this;
	    }

	    iterator& operator-( size_t s ) {
		it_ -= s;
		return *this;
	    }

	private:
	    explicit iterator( std::vector< Token* >::iterator it ) :
		it_( it) {
	    } 

	    std::vector< Token* >::iterator it_;
	};

	/**
	 * @brief the const version of iterator
	 * @see iterator
	 */
	class const_iterator {
	public:
	    const_iterator( std::vector< Token* >::const_iterator it ) :
		it_( it ) {
	    } 
	
	    bool operator==( const_iterator const& other ) const {
		return it_ == other.it_;
	    }
	    bool operator!=( const_iterator const& other ) const {
		return ! ( *this == other );
	    }

	    Token const& operator*() {
		return **it_;
	    }
	    
	    Token const* operator->() {
		return *it_;
	    }

	    const_iterator& operator++() {
		++it_;
		return *this;
	    }
	    
	    const_iterator& operator+( size_t s ) {
		it_ += s;
		return *this;
	    }

	private:
	    std::vector< Token* >::const_iterator it_;
	};





	iterator begin() {
	    return iterator( tokens_.begin() );
	}

	const_iterator begin() const {
	    return const_iterator( tokens_.begin() );
	}

	iterator end() {
	    return iterator( tokens_.end() );
	}

	const_iterator end() const {
	    return const_iterator( tokens_.end() );
	}



	/**
	 * @brief This class introduces a page structure to the Document.
	 *
	 * The Page objects do not contain Token s themselves - those are always stored
	 * in a vector of Token s in the Document object. Instead each Page just knows offsets
	 * into this vector of Token s. 
	 */
	class Page {

	private:
	    friend class Document;

	    /**
	     * @brief Initializes a Page object with the mother document, an image filename
	     *        and an offset into the vector of Token s.
	     *
	     * The constructor is private, so only the friend class Document can create objects -
	     * it does so in the method Document::newPage().
	     *
	     * The Page is initialized empty, so offsetEnd_ = offsetBegin_.
	     *
	     * The iterators show the typical container-like behaviour.
	     *
	     * @deprecated The preferred way of constructing is to use Document::newPage()
	     *             like: d.newPage().setImageFile( "foo" ).setSourceFile( "bar" );
	     */
	    Page( Document& doc, std::string const& imageFile, size_t offsetBegin ) :
		myDocument_( doc ),
		imageFile_( imageFile ),
		offsetBegin_( offsetBegin ),
		offsetEnd_( offsetBegin ) {
		
	    }

	    Page( Document& doc, size_t offsetBegin ) :
		myDocument_( doc ),
		offsetBegin_( offsetBegin ),
		offsetEnd_( offsetBegin ) {
		
	    }

	public:
	    

	    /**
	     * @brief returns the number of tokens on the Page
	     */
	    size_t size() const {
		return ( offsetEnd_ - offsetBegin_ );
	    }

	    /**
	     * @brief return the filename of the image for this page.
	     *
	     */
	    std::string& getImageFile() {
		return imageFile_;
	    }

	    /**
	     * @brief return the filename of the image for this page.
	     *
	     */
	    std::string const& getImageFile() const {
		return imageFile_;
	    }
	    
	    
	    /**
	     *
	     * @return this Page (for method chaining)
	     */
	    Page& setImageFile( std::string const& f ) {
		imageFile_ = f;
		return *this;
	    }


	    /**
	     * @brief return the filename of the OCR source file for this page.
	     *
	     * This might as remain empty.
	     */
	    std::string const& getSourceFile() const {
		return sourceFile_;
	    }

	    /**
	     * @brief set the sourceFile attribute for the Page
	     *
	     * @return this Page (for method chaining)
	     */
	    Page& setSourceFile( std::string const& f ) {
		sourceFile_ = f;
		return *this;
	    }

	    /**
	     * @brief returns a Document::iterator to the first Token of the Page.
	     *
	     * The iterators show the typical container-like behaviour.
	     */
	    Document::iterator begin() {
		return Document::iterator( myDocument_.tokens_.begin() + offsetBegin_ );
	    }

	    /**
	     * @brief returns a Document::iterator to the first Token of the Page.
	     *
	     * The iterators show the typical container-like behaviour.
	     */
	    Document::const_iterator begin() const {
		return Document::const_iterator( myDocument_.tokens_.begin() + offsetBegin_ );
	    }

	    /**
	     * @brief returns a Document::iterator referring to the past-the-end Token of the Page.
	     *
	     * The iterators show the typical container-like behaviour.
	     */
	    Document::iterator end() {
		return Document::iterator( myDocument_.tokens_.begin() + offsetEnd_ );
	    }

	    /**
	     * @brief returns a Document::iterator referring to the past-the-end Token of the Page.
	     *
	     * The iterators show the typical container-like behaviour.
	     */
	    Document::const_iterator end() const {
		return Document::const_iterator( myDocument_.tokens_.begin() + offsetEnd_ );
	    }

	    /**
	     * @brief returns the (document-wide) token index of the first token of the page
	     * 
	     */
	    size_t getOffsetBegin() const {
		return offsetBegin_;
	    }
	    
	    /**
	     * @brief returns the (document-wide) token index of the after-the-last token of the page
	     *
	     */
	    size_t getOffsetEnd() const {
		return offsetEnd_;
	    }
	    
	private:

            void setOffsetBegin( size_t x ) {
                offsetBegin_ = x;
            }

            void setOffsetEnd( size_t x ) {
                offsetEnd_ = x;
            }

            /**
             * @brief A reference to the parent document
             */
            Document& myDocument_;

            /**
             * @brief The path to the image file belonging to the page
             */
	    std::string imageFile_;

            /**
             * @brief The path to the OCR source file belonging to the page.
             */
	    std::string sourceFile_;

            /**
             * @brief the (document-wide) index of the first token of the page
             */
	    size_t offsetBegin_;

            /**
             * @brief the (document-wide) index of the after-the-last token of the page
             */
            size_t offsetEnd_;
	}; // class Page
	

	/**
	 * @brief This iterator class is used to iterate through all Page s of the Document
	 *
	 * It wraps a std::vector::iterator, but the underlying vector contains not Pages but pointers
	 * to Pages.
	 */
	class PageIterator {
	public:
	    PageIterator() {
	    }

	    PageIterator( std::vector< Page* >::iterator const& it ) :
		it_( it) {
	    } 
	    
	    bool operator==( const PageIterator& other ) const {
		return it_ == other.it_;
	    }
	    bool operator!=( const PageIterator& other ) const {
		return ! ( *this == other );
	    }
	    
	    Page& operator*() {
		return **it_;
	    }
	    
	    Page* operator->() {
		return *it_;
	    }
	    
	    PageIterator& operator++() {
		++it_;
		return *this;
	    }
	    
	    PageIterator& operator+( size_t s ) {
		it_ += s;
		return *this;
	    }
	    
	private:
	    std::vector< Page* >::iterator it_;
	};
	
	/**
	 * @brief the const version of PageIterator
	 * @see PageIterator
	 */
	class const_PageIterator {
	public:
	    const_PageIterator( std::vector< Page* >::const_iterator it ) :
		it_( it ) {
	    } 

	    bool operator==( const_PageIterator const& other ) const {
		return it_ == other.it_;
	    }
	    bool operator!=( const_PageIterator const& other ) const {
		return ! ( *this == other );
	    }

	    Page const& operator*() {
		return **it_;
	    }
	    
	    Page const* operator->() {
		return *it_;
	    }
	
	    const_PageIterator& operator++() {
		++it_;
		return *this;
	    }
	    
	    const_PageIterator& operator+( size_t s ) {
		it_ += s;
		return *this;
	    }

	private:
	    std::vector< Page* >::const_iterator it_;
	};


	/**
	 * @brief Returns true iff the Document has page information.
	 */
	bool hasPages() const {
	    return ! pages_.empty();
	}

        /**
         * @brief returns a PageIterator pointing to the first page of the document
         * @return a PageIterator pointing to the first page of the document
         */
	PageIterator pagesBegin() {
	    return PageIterator( pages_.begin() );
	}
	
        /**
         * @brief returns a const_PageIterator pointing to the first page of the document
         * @return a const_PageIterator pointing to the first page of the document
         */
	const_PageIterator pagesBegin() const {
	    return const_PageIterator( pages_.begin() );
	}

        /**
         * @brief returns a PageIterator pointing to the after-the-last page of the document
         * @return a PageIterator pointing to the  after-the-last page of the document
         */
     	PageIterator pagesEnd() {
	    return PageIterator( pages_.end() );
	}

        /**
         * @brief returns a const_PageIterator pointing to the after-the-last page of the document
         * @return a const_PageIterator pointing to the  after-the-last page of the document
         */
	const_PageIterator pagesEnd() const {
	    return const_PageIterator( pages_.end() );
	}

        /**
         * @brief returns a reference to the page specified by index
         * @param index
         * @return a reference to the page specified by index
         */
	Page& pagesAt( size_t index ) {
	    return *( pages_.at( index ) );
	}

        /**
         * @brief returns a const reference to the page specified by index
         * @param index
         * @return a const reference to the page specified by index
         */
	Page const& pagesAt( size_t index ) const {
	    return *( pages_.at( index ) );
	}

        /**
         * @brief Introduces a new empty page at the end of the document
         * @param imageFile
	 *
	 * @returns  A reference to the new Page object.
         */
	inline Page& newPage( std::string imageFile = "" ) {
	    if( ( pages_.size() == 0 ) && ( tokens_.size() > 0 ) ) {
		throw OCRCException( "OCRC::Document::newPage: First page must be initialized before first token is added." );
	    }
	    Page* newPage = new Page( *this, imageFile, tokens_.size() ); 
	    pages_.push_back( newPage );

	    return *newPage;
	}

	/**
	 * @brief Change the directory where image files of the pages can be found.
	 * 
	 * This presumes that all images are in the same directory.
	 */
	inline void changeImageDir( std::string const& dir ) {
	    for( PageIterator page = pagesBegin(); page != pagesEnd(); ++page ) {
		size_t pos = page->getImageFile().rfind( "/" );
		if( pos != std::string::npos ) {
     		    page->getImageFile().replace( 0, pos, dir );
		}

	    }
	}

	// THIS METHOD IS NOT NEEDED AS LONG AS THE TOKENS HAVE THEIR PAGINDEX AS ATTRIBUTE ANYWAY
	/**
	 * @brief For a given token index, finds the index of its page.
	 */
	//inline size_t findPageIndexByTokenIndex( size_t tokenIndex, size_t first = -1, size_t last = -1  ) const;


	/**
	 * @brief Does a run through the document and does some checks if the state of the object is still ok.
	 */
	inline bool integrityCheck() const;

	// std::locale const& getLocale() const {
	//     return locale_;
	// }

    private: 	/////  PRIVATE  of class Document ///////////////////////////

	
	friend class IBMGTReader;

	/**
	 * The vector containing all tokens. The Token-objects are created on the heap, so
	 * that pointers won't get invalid if tokens are inserted somewhere or if the vector reallocates.
	 */
	std::vector< Token* > tokens_;

        std::set< std::wstring > stringPool_;

	/**
	 * @brief A vector of Page s.
	 *
	 * This vector being empty indicates that the Document has no page information. 
	 */
	std::vector< Page* > pages_;

	
//	std::locale locale_;
	
    }; // class Document

    
    
} // ns OCRCorrection

#include "./Document_inline_definitions.tcc"
#include "../Token/Token_inline_definitions.tcc"
#include "../Candidate/Candidate_inline_definitions.tcc"

#endif
