#ifndef OCRCORRECTION_TOKEN_H
#define OCRCORRECTION_TOKEN_H OCRCORRECTION_TOKEN_H

#include<vector>
#include<string>
#include<algorithm>

#include <csl/LevFilter/LevFilter.h>
#include <csl/Vaam/Vaam.h>
#include <csl/CSLLocale/CSLLocale.h>
#include <Exceptions.h>
#include <Candidate/Candidate.h>
#include "./Character.h"
#include "./TokenImageInfoBox.h"

/******************************************************/
/***********  IMPORTANT *******************************/
/*                                                    */
/*   DO NOT INCLUDE THIS HEADER FILE ANYWHERE         */
/*                                                    */
/*   Include Document.h instead                       */
/******************************************************/


namespace OCRCorrection {


    /**
     * @brief Represents one token of a document and holds an array of correction candidates for the token.
     * 
     *
     * @author Uli Reffle
     */
    class Token {
	
    public:
	/**
	 * @brief An enum type that encodes a number of properties for Tokens. Each property is assigned a bit in the 
	 *        variable bitProperties_ of class Token.
	 *
	 * NORMAL       Token consists of alphabetical symbols and should be processed. NOT normal are e.g. numbers, punctuation marks etc.
	 * SUSPICIOUS   Token was found suspicious
	 * CORRECTED    Token was corrected manually by the user
	 * SPLIT_MERGE  Token is considered to be part of some word-level split/merge mess.
	 * DONT_TOUCH   This property was added mainly for the profiler. If this flag is set, the profiler should not process this token.
	 * 
	 */
	enum TokenProperties { 
	    NORMAL               = 0x1, 
	    SUSPICIOUS           = 0x2, // obsolete
	    CORRECTED            = 0x4, 
	    SPLIT_MERGE          = 0x8, // obsolete
	    DONT_TOUCH           = 0x10,
	    CAPITALIZED          = 0x20,
	    HYPHENATION_1ST      = 0x40,
	    HYPHENATION_MARK     = 0x80,
	    HYPHENATION_2ND      = 0x100,
	    LINE_BEGIN           = 0x200,
	    LINE_END             = 0x400
	};  


	enum VerifiedStatus {VERIFIED_FALSE, VERIFIED_TRUE, VERIFIED_GUESSED};

	/******************** CONSTRUCTORS/ DESTRUCTORS ************************/
	/**
	 * @name Constructors/ Destructor
	 */
	//@{

	/**
	 * @brief Create a new Token with the values as given
	 *
	 * @param wOCR points to the beginning of the token as recognized by the ocr-engine.
	 * @param length take the first [length] characters of [wOCR] as string [wOCR_]
	 *
	 * @todo Remove this constructor, it is nonsense.
	 *
	 * @deprecated
	 */
	Token( Document& document, size_t indexInDocument, const wchar_t* wOCR, size_t length, bool isNormal = true );

	Token( Document& document, size_t indexInDocument, std::wstring const& wOCR, bool isNormal = true );

	Token( Document& document, size_t indexInDocument, bool isNormal = true );


	/**
	 * @brief Copy constructor: ATTENTION! This does, at the moment, NOT copy the candidate and the character arrays
	 *        
	 */
	Token( Token const& other );

	/**
	 * @brief Destructor
	 *
	 * Candidates are created on the heap, so they have to be destroyed here.
	 */
	~Token();

	//@}



	/****************************   GETTERS ***********************************/
	/**
	 * @name Getters
	 */
	//@{

	/**
	 * @brief returns a reference to the Document which contains this Token
	 * @return a reference to the Document which contains this Token
	 */
	inline Document& getDocument() const;

	/**
	 * @brief returns the position this Token holds in the Token-array in the Document
	 * @return the position this Token holds in the Token-array in the Document
	 */
	inline size_t getIndexInDocument() const;

        /**
         * @brief returns the index of the page which the thoken belongs to
         * @return the index of the page which the thoken belongs to
         */
	inline size_t getPageIndex() const;



	/**
	 * @brief returns a reference to wOCR, the token as recognized by the ocr engine
	 * @return a reference to wOCR, the token as recognized by the ocr engine
	 */
	inline const std::wstring& getWOCR() const;

	inline const std::wstring& getWOCR_lc() const;


	inline std::wstring const& getHyphenationMerged() const;

	/**
	 * @brief returns a reference to tokenImageInfoBox
	 */
	inline TokenImageInfoBox const* getTokenImageInfoBox() const;

	/**
	 * @brief returns a reference to wCorr, the current correction suggestion for the token
	 * @return a reference to wCorr, the current correction suggestion for the token
	 */
	inline const std::wstring& getWCorr() const;
	
	inline const std::wstring& getWDisplay() const;


	inline bool hasProperty( TokenProperties prop ) const;

	/**
	 * @return true iff token is marked as normal
	 */
	inline bool isNormal() const;

	/**
	 * @brief returns true iff token was found suspicious.
	 * @return true iff token was found suspicious
	 *
	 * At the moment, this getter does NOT rely on the token property SUSPICIOUS.
	 *
	 */
	inline bool isSuspicious() const;

	/**
	 * @return true iff token was corrected manually
	 */
	inline bool isCorrected() const;

	/**
	 * @return true iff the first character of wOCR_ is uppercase.
	 *
	 * For empty wOCR_, return false
	 */
	inline bool isCapitalized() const;


	/**
	 * @brief returns true iff token is marked as "Don't Touch"
	 * @return true iff token is marked as "Don't Touch"
	 * @see TokenProperties
	 */
	inline bool isDontTouch() const;

	inline bool hasTopCandidate() const;
	inline Candidate const& getTopCandidate() const;
	
	//@}

	/****************************** SETTERS ************************************/
	/**
	 * @name Setters
	 */
	//@{

	/**
	 * @brief set a new value for the position inside the Document-object
	 */
	inline void setIndexInDocument( size_t index ) {
	    indexInDocument_ = index;
	}
	
	/**
	 * This also set wOCR_lc_
	 */
	void setWOCR( std::wstring const& w );


	/**
	 * @brief set a new correction for the Token
         *
         * This version only offers the very basic functionality of a setter.
         * See also handleSplit(): This method decides if the given string should be
         * split up into several tokens.
         *
         * @see handleSplit
	 */
	inline void setWCorr( std::wstring const& w );

	/**
	 * @see hyphenationMerged_
	 */
	inline void setHyphenationMerged( std::wstring const& w );


	/**
	 * @brief set this flag to mark a normal token
	 */
	inline void setNormal( bool b = true );

	/**
	 * @brief set this flag if the token was already corected manually
	 */
	inline void setCorrected( bool b = true );

	/**
	 * @brief set this flag if the token is part of split/merge trouble
	 */
	inline void setSplitMerge( bool b = true );

	/**
	 * @brief set this flag to indicate that this flag should not be touched by post-processing
	 */
	inline void setDontTouch( bool b = true );

	/**
	 * @brief Add a Character -object to the Token.
	 * @todo might involve a lot of copying of objects
	 */
	void addCharacter( Character const& c );

	/**
	 * @brief This flag indicates that wOCR was found in the dictionary 
	 */
	inline void setSuspicious( bool b = true );

	/**
	 * @brief This flag indicates that wOCR has a capitalized first char
	 */
	inline void setCapitalized( bool b = true );

	inline void setProperty( TokenProperties property, bool b );

	/**
	 * @brief sets the page for this token.
	 * 
	 * @param page 
	 *
	 */
	inline void setPageIndex( size_t pageIndex ) {
	    myPageIndex_ = pageIndex;
	}

	void setCoordinates( size_t l, size_t t, size_t r, size_t b );

	//inline void setTopCandidate( Candidate const& cand );

	//@} // END SETTERS

	/*********************** SPLIT/ MERGE ***********************************/
	/**
	 * @name Split/ Merge -related methods
	 */
	//@{

	/**
	 * @brief Merge token with the one to the right.
	 *
	 * In contrast to mergeRight(int), this method shows some intelligence in that it
	 * checks if the following token is a space - in that case, the token after that is
	 * merged as well.
	 *
	 * @return the number of following tokens that were swallowed by the merge 
	 */
	size_t mergeRight();

	/**
	 * @brief Merge token with the given number of following tokens.
	 *
	 * This method merges this token with the following n ones. Only spaces are swallowed.
	 *
	 * @return the number of following tokens that were swallowed by the merge 
	 */
	size_t mergeRight( size_t n );



        /**
         * @brief Perform merge with the first token of the next line.
         * @return the number of following tokens that were swallowed by the merge
         */
        void mergeHyphenation();

	/**
	 * @brief split the token according to the string
	 * @return the number of new tokens which were inserted to the right
	 */
	size_t handleSplit( std::wstring const& string );


	typedef std::vector< Character >::const_iterator CharIterator;

	CharIterator charBegin() {
	    return characters_.begin();
	}

	CharIterator charEnd() {
	    return characters_.end();
	}

	//@}

	/************************* CONVENIENCE *********************************/
	/**
	 * @name Convenience methods
	 */
	//@{

	void print( std::wostream& stream = std::wcout );

	//@}


	class Groundtruth {
	public:
	    /**
	     * @brief copy constructor
	     * @param token the Token-object the Groundtruth object belongs to.
	     */
	    Groundtruth( Token& token ) :
		myToken_( &token ),
		wOrig_(),
		wOrig_lc_(),
		histTrace_(),
		ocrTrace_(),
		baseWord_(),
		classified_( L"unknown" ),
		verified_( VERIFIED_FALSE ),
		isNormal_( false ) {
	    }

	    /**
	     * @brief copy constructor
	     */
	    Groundtruth( Groundtruth const& other ) :
	    	myToken_( other.myToken_ ),
	    	wOrig_( other.getWOrig() ),
	    	wOrig_lc_( other.getWOrig_lc() ),
	    	histTrace_( other.getHistTrace() ),
	    	ocrTrace_( other.getOCRTrace() ),
	    	baseWord_( other.getBaseWord() ),
	    	classified_( other.getClassified() ),
	    	verified_( other.getVerified() ),
	    	isNormal_( other.isNormal() )
		{
		}


	    Groundtruth& operator=( Groundtruth const& other ) {
		// myToken_ remains what it was!!!!!!!!!
		setWOrig( other.getWOrig() );
		setNormal( other.isNormal() );
		setHistTrace( other.getHistTrace() );
		setOCRTrace( other.getOCRTrace() );
		setBaseWord( other.getBaseWord() );
		setClassified( other.getClassified() );
		setVerified( other.getVerified() );

		return *this;
	    }
	    

	    inline std::wstring const& getWOrig() const;
	    inline std::wstring const& getWOrig_lc() const;
	    inline bool isNormal() const;
	    inline std::wstring const& getOCRTrace() const;
	    inline std::wstring const& getHistTrace() const;
	    inline std::wstring const& getBaseWord() const;
	    inline VerifiedStatus getVerified() const;
	    inline std::wstring const& getClassified() const;
	    
	    inline void setWOrig( std::wstring const& );
	    inline void setNormal( bool n );
	    inline void setOCRTrace( std::wstring const& );
	    inline void setHistTrace( std::wstring const& );
	    inline void setBaseWord( std::wstring const& );
	    inline void setVerified( VerifiedStatus v );
	    inline void setClassified( std::wstring const& str );
	    
	private:
	    Token* myToken_;
	    std::wstring wOrig_;
	    std::wstring wOrig_lc_;
	    std::wstring histTrace_;
	    std::wstring ocrTrace_;
	    std::wstring baseWord_;

	    /**
	     * @brief 
	     */
	    std::wstring classified_;

	    VerifiedStatus verified_;

	    /**
	     * @brief Indicates if wOrig_ is a normal token
	     */
	    bool isNormal_;


	}; // class Groundtruth

	bool hasGroundtruth() const {
	    return groundtruth_ != 0;
	}

	void initGroundtruth() {
	    if( groundtruth_ ) throw OCRCException( "OCRC::Token::initGroundtruth: Groundtruth already initialized for this token." );
	    groundtruth_ = new Groundtruth( *this );
	}

	Groundtruth& getGroundtruth() {
	    return *groundtruth_;
	}

	Groundtruth const& getGroundtruth() const {
	    return *groundtruth_;
	}
	

	/**
	 * @brief A group of data members that are specific to the Abbyy ocr output
	 */
	class AbbyySpecifics {
	public:
	    AbbyySpecifics() :
		isSuspicious_( false ) {
	    }
	    
	    inline void setSuspicious( bool b ) {
		isSuspicious_ = b;
	    }
	    
	    inline bool isSuspicious() const {
		return isSuspicious_;
	    }
	    
	private:
	    /**
	     * @brief A token is suspicious if it contains at least one suspicous char in the ABBYY sense.
	     */
	    bool isSuspicious_;

	    
	}; // class AbbyySpecifics

	/**
	 * @brief returns a reference to a data-structure containing information specificalyy provided by abbyy xml 
	 *        output
	 */
	inline AbbyySpecifics& getAbbyySpecifics();

	/**
	 * @brief returns a const reference to a data-structure containing information specifically 
	 *        provided by abbyy xml output
	 */
	inline AbbyySpecifics const& getAbbyySpecifics() const;

	class CandidateChain {
	public:
	    CandidateChain( Candidate const& cand ) :
		candidate_( cand ), 
		next_( 0 ) {
	    }
	    Candidate& getCandidate() {
		return candidate_;
	    }
	    CandidateChain* getNext() {
		return next_;
	    }
	    void setNext( CandidateChain* n ) {
		next_ = n;
	    }
	private:
	    Candidate candidate_;
	    CandidateChain* next_;

	};

	class CandidateIterator {
	public:
	    CandidateIterator( CandidateChain* pos ) :
		pos_( pos ) {
		
	    }

	    CandidateIterator& operator++() {
		pos_ = pos_->getNext();
		return *this;
	    }

	    bool operator !=( CandidateIterator const& other ) {
		return pos_ != other.pos_;
	    }

	    Candidate* operator ->() {
		return &( pos_->getCandidate() );
	    }

	    Candidate& operator *() {
		return pos_->getCandidate();
	    }

	    
	    


	private:
	    CandidateChain* pos_;
	}; // class CandidateIterator

	CandidateIterator candidatesBegin() const {
	    return CandidateIterator( candidates_ );
	}
	
	CandidateIterator candidatesEnd() const {
	    return CandidateIterator( 0 );
	}

	/**
	 * This method is public because of jni issues. Please don't use it!
	 * Instead, access the candidates using iterators!
	 */
	CandidateChain* getCandItem() {
	    return candidates_;
	}
	

	void addCandidate( Candidate const& initCand );
	
	inline size_t getNrOfCandidates() const;

	/**
	 * @brief remove all candidates of the Token
	 */
	void removeCandidates();


	/**
	 * @brief returns a const reference to the external id property
	 */
	inline std::wstring const& getExternalId() const;
	
	/**
	 * @brief sets the external id property
	 */
	inline void setExternalId( std::wstring const& id );



	inline Document& getMyDocument() {
	    return myDocument_;
	}
	
    protected:
	/************************** PROTECTED **********************************/

	/**
	 * @brief A reference to the Document object which the Token is part of.
	 */
	Document& myDocument_;
	
	/**
	 * @brief The token as it appears in the ocr-ed document
	 */
	std::wstring wOCR_;

	/**
	 * @brief the lower-cased version of wOCR_
	 */
	std::wstring wOCR_lc_;


	/**
	 * @brief In case the token is part (1st or 2nd) of a hyphenation, this string
	 *        holds the merged string. The token properties specify if the current 
	 *        token is part of such a hyphenation.
	 *
	 * If the Token is not part of a hyphenation, this string remains empty.
	 *
	 * @see TokenProperties
	 */
	std::wstring hyphenationMerged_;

	
	/**
	 * @brief 
	 */
	std::wstring wCorr_;
	
	std::vector< Character > characters_;
	

	/**
	 * @brief This is a bit-vector which handles a number of boolean properties
	 */
	size_t bitProperties_;


	/**
	 * @brief A data structure holding token information that is specific to Abbyy output.
	 */
	AbbyySpecifics abbyySpecifics_;

	/**
	 * @brief This id string can be set to whatever you need to find the token in your original document.
	 */
	std::wstring externalId_;



	/**
	 * @brief Holds the index of the Token in its document.
	 */
	size_t indexInDocument_;

	/**
	 * @brief Tells on which page of the document the token can be found.
	 *
	 * We need this e.g. when we want to retrieve the image for the Token.
	 */
	size_t myPageIndex_;

	/**
	 * @brief Pointer to a Groundtruth object holding groundtruth information
	 *
	 * The pointer is initialised with 0. If the respective document parser can provide
	 * the necessary information, a Groundtruth object is allocated.
	 *
	 */
	Groundtruth* groundtruth_;

	/**
	 * @brief Pointer to a TokenImageInfoBox holding picture coordinates.
	 *
	 * The pointer is initialised with 0. If the respective document parser can provide
	 * the necessary information, a TokenImageInfoBox object is allocated.
	 */
	TokenImageInfoBox* tokenImageInfoBox_;

	/**
	 * @brief This is needed for the DictSearch::ResultReceiver interface
	 */
	csl::DictSearch::iDictModule const* currentDictModule_;



	/**
	 * @brief The list of candidates is provided by the Profiler
	 *
	 */
	CandidateChain* candidates_;
	
	/**
	 * @brief specifies the nr of cands stored in candidates_
	 */
	size_t nrOfCandidates_;

    }; // class Token



} // ns OCRCorrection

// NOTE THAT the implementations of this class are stored in Token.tcc
// To manage the circular dependencies, Token.tcc is included in Document.h
#endif
