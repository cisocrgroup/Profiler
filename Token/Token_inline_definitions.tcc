#ifndef OCRCORRECTION_TOKEN_TCC__
#define OCRCORRECTION_TOKEN_TCC__

namespace OCRCorrection {

    inline Document& Token::getDocument() const {
	return myDocument_;
    }

    inline size_t Token::getIndexInDocument() const {
	return indexInDocument_;
    }


    const std::wstring& Token::getWOCR() const {
	return wOCR_;
    }

    const std::wstring& Token::getWOCR_lc() const {
	return wOCR_lc_;
    }

    std::wstring const& Token::getHyphenationMerged() const {
	return hyphenationMerged_;
    }


    const std::wstring& Token::getWCorr() const {
	return wCorr_;
    }

    const std::wstring& Token::getWDisplay() const {
	if( isCorrected() ) {
	    return wCorr_;
	}
	else {
	    return wOCR_;
	}
    }

    inline bool Token::hasTopCandidate() const {
	return ( candidates_ != 0 );
    }

    inline Candidate const& Token::getTopCandidate() const {
        assert(hasTopCandidate());
	return candidates_->getCandidate();
    }


    bool Token::hasProperty( TokenProperties property ) const {
	return ( ( static_cast< TokenProperties >( bitProperties_ ) & property ) == property );
    }

    bool Token::isNormal() const {
	return ( bitProperties_ & NORMAL );
    }

    bool Token::isSuspicious() const {
	if( getWOCR().length() <=3 ) { // rely on abbyy's judgment
	    return getAbbyySpecifics().isSuspicious();
	}
	else if( !hasTopCandidate() ) { // always suspicious
	    return true;
	}
	else {
	    return getTopCandidate().getLevDistance() > 0;
	}
    }

    bool Token::isCorrected() const {
	return ( bitProperties_ & CORRECTED );
    }

    bool Token::isCapitalized() const {
	return ( bitProperties_ & CAPITALIZED );
    }

    bool Token::isDontTouch() const {
	return ( bitProperties_ & DONT_TOUCH );
    }

    /*
      / returns a pointer to the tokenImageInfoBox or 0 if token has no coordinates
      /
    */
    TokenImageInfoBox const* Token::getTokenImageInfoBox() const {
      //    if( ! tokenImageInfoBox_ ) throw OCRCException( "OCRC::Token::getTokenImageInfoBox: no tokenImageInfoBox, 0-Pointer" );
      return tokenImageInfoBox_;
  }



    std::wstring const& Token::Groundtruth::getBaseWord() const {
	return baseWord_;
    }

    std::wstring const& Token::Groundtruth::getWOrig() const {
	return wOrig_;
    }

    std::wstring const& Token::Groundtruth::getWOrig_lc() const {
	return wOrig_lc_;
    }

    bool Token::Groundtruth::isNormal() const {
	return isNormal_;
    }

    std::wstring const& Token::Groundtruth::getOCRTrace() const {
	return ocrTrace_;
    }

    std::wstring const& Token::Groundtruth::getHistTrace() const {
	return histTrace_;
    }

    void Token::Groundtruth::setWOrig( std::wstring const& wOrig ) {
	wOrig_ = wOrig;
	wOrig_lc_ = wOrig_;
	for( std::wstring::iterator c = wOrig_lc_.begin(); c != wOrig_lc_.end(); ++c ) {
             *c = towlower(*c);
	}
    }

    void Token::Groundtruth::setNormal( bool n ) {
	isNormal_ = n;
    }

    void Token::Groundtruth::setOCRTrace( std::wstring const& instr ) {
	ocrTrace_ = instr;
    }

    void Token::Groundtruth::setHistTrace( std::wstring const& instr ) {
	histTrace_ = instr;
    }

    void Token::Groundtruth::setBaseWord( std::wstring const& w ) {
	baseWord_ = w;
    }

    void Token::Groundtruth::setVerified( VerifiedStatus v ) {
	verified_ = v;
    }

    void Token::Groundtruth::setClassified( std::wstring const& str ) {
	classified_ = str;
    }

    Token::VerifiedStatus Token::Groundtruth::getVerified() const {
	return verified_;
    }

    std::wstring const& Token::Groundtruth::getClassified() const {
	return classified_;
    }

    void Token::setWCorr( std::wstring const& w ) {
	wCorr_ = w;
	setCorrected( ! wCorr_.empty() );
    }

    void Token::setHyphenationMerged( std::wstring const& w ) {
	hyphenationMerged_ = w;
    }


    void Token::setNormal( bool b ) {
	if( b ) {
	    bitProperties_ |= NORMAL;
	}
	else {
	    bitProperties_ &= ~( NORMAL );
	}
    }

    void Token::setCorrected( bool b ) {
	if( b ) {
	    bitProperties_ |= CORRECTED;
	}
	else {
	    bitProperties_ &= ~( CORRECTED );
	}
    }

    void Token::setSplitMerge(  bool b ) {
	if( b ) {
	    bitProperties_ |= SPLIT_MERGE;
	}
	else {
	    bitProperties_ &= ~( SPLIT_MERGE );
	}
    }

    void Token::setSuspicious( bool b ) {
	if( b ) {
	    bitProperties_ |= SUSPICIOUS;
	}
	else {
	    bitProperties_ &= ~( SUSPICIOUS );
	}
    }

    void Token::setDontTouch( bool b ) {
	if( b ) {
	    bitProperties_ |= DONT_TOUCH;
	}
	else {
	    bitProperties_ &= ~( DONT_TOUCH );
	}
    }

    void Token::setCapitalized( bool b ) {
	if( b ) {
	    bitProperties_ |= CAPITALIZED;
	}
	else {
	    bitProperties_ &= ~( CAPITALIZED );
	}
    }

    void Token::setProperty( TokenProperties property,  bool b ) {
	if( b ) {
	    bitProperties_ |= property;
	}
	else {
	    bitProperties_ &= ~( property );
	}
    }



    size_t Token::getPageIndex() const {
	if( myPageIndex_ == (size_t)-1 ) throw OCRCException( "OCRC::Token::getPage: no such inforrmation available" );
	return myPageIndex_;
    }

    size_t Token::getNrOfCandidates() const {
	return nrOfCandidates_;
    }

    Token::AbbyySpecifics& Token::getAbbyySpecifics() {
	return abbyySpecifics_;
    }

    Token::AbbyySpecifics const& Token::getAbbyySpecifics() const {
	return abbyySpecifics_;
    }

    /**
     * @brief returns a const reference to the external id property
     */
    std::wstring const& Token::getExternalId() const {
	return externalId_;
    }

    /**
     * @brief sets the external id property
     */
    void Token::setExternalId( std::wstring const& id ) {
	externalId_ = id;
    }


}
#endif /* OCRCORRECTION_TOKEN_TCC__ */
