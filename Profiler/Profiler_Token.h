

class Profiler_Token  {


	public:
	enum VerifiedStatus {VERIFIED_FALSE, VERIFIED_TRUE, VERIFIED_GUESSED};

	/******************** CONSTRUCTORS/ DESTRUCTORS ************************/

//	Profiler_Token();
	Profiler_Token( Token& orginalToken );


	/**
	 * @brief Candidates are created on the heap, so they have to be destroyed here.
	 */
	~Profiler_Token();




	/****************************   GETTERS ***********************************/

    Token& getOriginalToken();
    const Token& getOriginalToken() const;

	/**
	 * @brief returns a reference to wOCR, the token as recognized by the ocr engine
	 * @return a reference to wOCR, the token as recognized by te ocr engine
	 */
	std::wstring const& getWOCR() const;

	std::wstring const& getWOCR_lc() const;

	/**
	 * @brief returns a reference to wCorr, the current correction suggestion for the token
	 * @return a reference to wCorr, the current correction suggestion for the token
	 */
	std::wstring const& getWCorr() const;
	const std::wstring& getWCorr_lc() const;

	/**
	 * @brief Returns a reference of the Candidate at index i
	 * @param index an index into the Candidate array
	 */
//	Candidate& candidateAt( size_t index );

	/**
	 * @brief const version of the getter candidateAt()
	 */
//	const Candidate& candidateAt( size_t index ) const;

	/**
	 * @return true iff token is marked as normal
	 */
	bool isNormal() const;
	void setNormal( bool b = true );

	bool isDontTouch() const;
	void setDontTouch( bool b = true );


	size_t getTokenNr() const {
	    return tokenNr_;
	}


	inline bool isSuspicious() const {
	    return isSuspicious_;
	}


	/****************************** SETTERS ************************************/


	void setWOCR( std::wstring const& w );

	void setWOCR_lc( std::wstring const& w );

	/**
	 * @brief set a new correction for the Token
	 */
	void setWCorr( const wchar_t* w );


	/**
	 * @brief Add a Character -object to the Token.
	 * @todo might involve a lot of copying of objects
	 */
	// void addCharacter( Character const& c );// NOT IMPLEMENTED HERE!

	void setLevDistance( float stdDist, float weightLev);

	bool isShort() const {
	    return ( getWOCR().size() <= 3 );
	}

	inline void setSuspicious( bool b ) {
	    isSuspicious_ = b;
	}



	void setTokenNr( size_t tn ) {
	    tokenNr_ = tn;
	}

	void setProbNormalizationFactor( double f );

	double getProbNormalizationFactor() const;

	void setWOCRFreq( size_t f );

	size_t getWOCRFreq() const;


	/************************* CONVENIENCE *********************************/
	void print( std::wostream& stream = std::wcout );


	/**
	 * @brief Contains different levels of groundtruth information for a Profiler_Token.
	 *
	 */
	class Groundtruth {
	public:
	    Groundtruth() :
		isNormal_( false ),
		verified_( VERIFIED_FALSE ),
		classified_( L"unknown" ) {

	    }

	    /**
	     * @brief This lets us assign a Token::Groundtruth object to this Profiler_Token::Groundtruth object.
	     */
	    Groundtruth& operator=( Token::Groundtruth const& other ) {
		setWOrig( other.getWOrig() );
		setWOrig_lc( other.getWOrig_lc() );
		setNormal( other.isNormal() );
		setHistTrace( other.getHistTrace() );
		setOCRTrace( other.getOCRTrace() );
		setBaseWord( other.getBaseWord() );
		setVerified( other.getVerified() );
		setClassified( other.getClassified() );

		return *this;
	    }


	    std::wstring const& getWOrig() const;
	    std::wstring const& getWOrig_lc() const;
	    bool isNormal() const;
	    std::wstring const& getOCRTrace() const;
	    std::wstring const& getHistTrace() const;
	    std::wstring const& getBaseWord() const;
	    VerifiedStatus getVerified() const;
	    std::wstring const& getClassified() const;

	    void setWOrig( std::wstring const& );
	    void setWOrig_lc( std::wstring const& );
	    void setNormal( bool n );
	    void setOCRTrace( std::wstring const& );
	    void setHistTrace( std::wstring const& );
	    void setBaseWord( std::wstring const& );
	    void setVerified( VerifiedStatus v );
	    /**
	     * @brief This method sets the verified-status according to the respective attribute in the
	     *        "normal" Token obejct.
	     */
	    void setVerified( Token::VerifiedStatus v );
	    void setClassified( std::wstring const& str );

	private:
	    /**
	     * @brief The groundtruth token
	     */
	    std::wstring wOrig_;

	    /**
	     * @brief The lower-cased groundtruth token
	     */
	    std::wstring wOrig_lc_;

	    /**
	     * @brief Indicates if wOrig_ is a normal token
	     */
	    bool isNormal_;

	    std::wstring ocrTrace_;
	    std::wstring histTrace_;
	    std::wstring baseWord_;
	    VerifiedStatus verified_;

	    /**
	     * @brief
	     */
	    std::wstring classified_;

	}; // class Groundtruth

	Groundtruth& getGroundtruth() {
	    return groundtruth_;
	}

	Groundtruth const& getGroundtruth() const {
	    return groundtruth_;
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

	AbbyySpecifics& getAbbyySpecifics() {
	    return abbyySpecifics_;
	}

	AbbyySpecifics const& getAbbyySpecifics() const {
	    return abbyySpecifics_;
	}




	void setCandidateSet( std::vector< Profiler_Interpretation > const* cs ) {
	    candidateSet_ = cs;
	}

	std::vector< Profiler_Interpretation > const* getCandidateSet() const {
	    return candidateSet_;
	}

    protected:
	/************************** PROTECTED **********************************/

	Token* originalToken_;

	std::wstring wOCR_;
	std::wstring wOCR_lc_;

	std::wstring wCorr_, wCorr_lc_;
	Groundtruth groundtruth_;
	AbbyySpecifics abbyySpecifics_;


	enum { NORMAL=0x1, LEXICAL=0x2, CORRECTED=0x4, SPLIT_MERGE=0x8, DONT_TOUCH=0x10 }; // 0x20, 0x40, 0x80, 0x100, ...

	/**
	 * @brief This is a bit-vector which handles a number of boolean properties
	 */
	unsigned int bitProperties_;

	/**
	 * @brief *DANGEROUS*  A pointer to the candidate list. This structure is created by the
	 *        Profiler ON THE STACK and is re-used for the next token, once the processing of
	 *        the current one is finished.
	 *
	 */
	std::vector< Profiler_Interpretation > const* candidateSet_;


	/**
	 * @brief This is the factor that was used to normalize the sum of probabilites of all
	 *        candidates to 1. In theory this should be sth like 1/freq(wOCR)
	 */
	double probNormalizationFactor_;

	/**
	 * @brief This is the actual wOCR (for comparison with probNormalizationFactor_ )
	 */
	size_t wOCRFreq_;

	size_t tokenNr_;
	bool isShort_;
	bool isSuspicious_;


    }; // class Token



