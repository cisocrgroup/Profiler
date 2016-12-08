#ifndef OCRC_PROFILER_PROFILER_TOKEN_CXX
#define OCRC_PROFILER_PROFILER_TOKEN_CXX OCRC_PROFILER_PROFILER_TOKEN_CXX

namespace OCRCorrection {

    // Profiler::Profiler_Token::Profiler_Token() :
    // 	originalToken_( originalToken ),
    // 	bitProperties_( 0 ),
    // 	candidateSet_( 0 ),
    // 	probNormalizationFactor_( 0 ),
    // 	tokenNr_( 0 )
    // {

    // }

    Profiler::Profiler_Token::Profiler_Token( Token& originalToken ) :
	originalToken_( &originalToken ),
	bitProperties_( 0 ),
	candidateSet_( 0 ),
	probNormalizationFactor_( 0 ),
	tokenNr_( 0 ),
	isSuspicious_( false )
    {
        // All this is done in Profiler::preparedocument

	// setWOCR( originalToken_->getWOCR() );
	// setWOCR_lc( originalToken_->getWOCR_lc() );
	// setDontTouch( originalToken_->isDontTouch() );
	// getAbbyySpecifics().setSuspicious( originalToken_->getAbbyySpecifics().isSuspicious() );

	// if( originalToken_->hasGroundtruth() ) {
	//     groundtruth_ = originalToken_->getGroundtruth();
	// }
    }



    Profiler::Profiler_Token::~Profiler_Token() {
    }

    const Token& Profiler::Profiler_Token::getOriginalToken() const {
	if( ! originalToken_ ) {
	    throw OCRCException( "OCRC::Profiler_Token::getOriginalToken: originalToken is NULL - this should not happen." );
	}
	return *originalToken_;
    }

    Token& Profiler::Profiler_Token::getOriginalToken() {
	if( ! originalToken_ ) {
	    throw OCRCException( "OCRC::Profiler_Token::getOriginalToken: originalToken is NULL - this should not happen." );
	}
	return *originalToken_;
    }


    const std::wstring& Profiler::Profiler_Token::getWOCR() const {
	return wOCR_;
    }

    const std::wstring& Profiler::Profiler_Token::getWOCR_lc() const {
	return wOCR_lc_;
    }

    const std::wstring& Profiler::Profiler_Token::getWCorr() const {
	return wCorr_;
    }

    const std::wstring& Profiler::Profiler_Token::getWCorr_lc() const {
	return wCorr_lc_;
    }

    bool Profiler::Profiler_Token::isNormal() const {
	return ( bitProperties_ && NORMAL );
    }

    void Profiler::Profiler_Token::setNormal( bool b ) {
	if( b ) {
	    bitProperties_ |= NORMAL;
	}
	else {
	    bitProperties_ &= ~( NORMAL );
	}
    }


    bool Profiler::Profiler_Token::isDontTouch() const {
	return ( bitProperties_ & DONT_TOUCH );
    }

    void Profiler::Profiler_Token::setDontTouch( bool b ) {
	if( b ) {
	    bitProperties_ |= DONT_TOUCH;
	}
	else {
	    bitProperties_ &= ~( DONT_TOUCH );
	}
    }


    void Profiler::Profiler_Token::setWOCR( std::wstring const& w ) {
	wOCR_ = w;
    }

    void Profiler::Profiler_Token::setWOCR_lc( std::wstring const& w ) {
	wOCR_lc_ = w;

	// find out if wOCR is normal
	if( wOCR_lc_.empty() ) {
	    setNormal( false );
	}
	else {
	    /**
	     * THIS IS WRONG !!!!!!!!!!!
	     * @todo FIX THIS!!!
	     */
	    setNormal( true );
	    for( std::wstring::const_iterator c = wOCR_lc_.begin(); c != wOCR_lc_.end(); ++c ) {
		if(!Document::isWord(*c)) {
		    setNormal( false );
                    break;
		}

	    }
	}
    }

    void Profiler::Profiler_Token::setWCorr( const wchar_t* w ) {
	wCorr_ = w;
	wCorr_lc_ = wCorr_;
	using std::begin;
	using std::end;
	std::transform(begin(wCorr_), end(wCorr_), begin(wCorr_lc_), towlower);
    }


    void Profiler::Profiler_Token::setProbNormalizationFactor( double f ) {
	probNormalizationFactor_ = f;
    }

    double Profiler::Profiler_Token::getProbNormalizationFactor() const {
	return probNormalizationFactor_;
    }

    void Profiler::Profiler_Token::setWOCRFreq( size_t f ) {
	wOCRFreq_ = f;
    }

    size_t Profiler::Profiler_Token::getWOCRFreq() const {
	return wOCRFreq_;
    }





    std::wstring const& Profiler::Profiler_Token::Groundtruth::getWOrig() const {
	return wOrig_;
    }

    std::wstring const& Profiler::Profiler_Token::Groundtruth::getWOrig_lc() const {
	return wOrig_lc_;
    }

    bool Profiler::Profiler_Token::Groundtruth::isNormal() const {
	return isNormal_;
    }

    std::wstring const& Profiler::Profiler_Token::Groundtruth::getOCRTrace() const {
	return ocrTrace_;
    }

    std::wstring const& Profiler::Profiler_Token::Groundtruth::getHistTrace() const {
	return histTrace_;
    }

    std::wstring const& Profiler::Profiler_Token::Groundtruth::getBaseWord() const {
	return baseWord_;
    }

    void Profiler::Profiler_Token::Groundtruth::setWOrig( std::wstring const& wOrig ) {
	wOrig_ = wOrig;
    }

    void Profiler::Profiler_Token::Groundtruth::setWOrig_lc( std::wstring const& wOrig_lc ) {
	wOrig_lc_ = wOrig_lc;
    }

    void Profiler::Profiler_Token::Groundtruth::setOCRTrace( std::wstring const& instr ) {
	ocrTrace_ = instr;
    }

    void Profiler::Profiler_Token::Groundtruth::setHistTrace( std::wstring const& instr ) {
	histTrace_ = instr;
    }

    void Profiler::Profiler_Token::Groundtruth::setBaseWord( std::wstring const& w ) {
	baseWord_ = w;
    }

    void Profiler::Profiler_Token::Groundtruth::setNormal( bool n ) {
	isNormal_ = n;
    }

    void Profiler::Profiler_Token::Groundtruth::setVerified( VerifiedStatus v ) {
	verified_ = v;
    }

    void Profiler::Profiler_Token::Groundtruth::setVerified( Token::VerifiedStatus v ) {
	switch( v ) {
	    case Token::VERIFIED_FALSE: verified_ = VERIFIED_FALSE; break;
	    case Token::VERIFIED_TRUE: verified_ = VERIFIED_TRUE; break;
	    case Token::VERIFIED_GUESSED: verified_ = VERIFIED_GUESSED; break;
	}
    }

    void Profiler::Profiler_Token::Groundtruth::setClassified( std::wstring const& str ) {
	classified_ = str;
    }

    Profiler::Profiler_Token::VerifiedStatus Profiler::Profiler_Token::Groundtruth::getVerified() const {
	return verified_;
    }

    std::wstring const& Profiler::Profiler_Token::Groundtruth::getClassified() const {
	return classified_;
    }



}

#endif
