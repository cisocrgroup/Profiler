#ifndef OCRCORRECTION_CANDIDATE_TCC__
#define OCRCORRECTION_CANDIDATE_TCC__

namespace OCRCorrection {

    inline Candidate::Candidate() :
	voteWeight_( 0 ) {
    }

    /**
     * @brief This constructor is equivalent to its version in csl::DictSearch
     *
     */
    inline Candidate::Candidate( csl::Interpretation const& interpretation, csl::DictSearch::iDictModule const& dm ) :
	csl::DictSearch::Interpretation( interpretation, dm ),
	voteWeight_( 0 ) {
    }

    inline Candidate::Candidate( csl::DictSearch::Interpretation const& interpretation ) :
	csl::DictSearch::Interpretation( interpretation ),
	voteWeight_( 0 ) {

    }

    inline std::wstring&  Candidate::getString() {
	return getWord();
    }

    inline const std::wstring&  Candidate::getString() const {
	return getWord();
    }

    inline size_t Candidate::getFrequency() const {
	return getBaseWordScore();
    }

    inline size_t Candidate::getDlev() const {
	return getLevDistance();
    }

    inline float Candidate::getVoteWeight() const {
	return voteWeight_;
    }


    inline void Candidate::setOCRTrace( csl::Trace const& ocrt ) {
	ocrTrace_ = ocrt;
    }

    inline void Candidate::setVoteWeight( float w ) {
	voteWeight_ = w;
    }

    inline std::wstring Candidate::toString() const {
	std::wostringstream oss;
	print( oss );
	oss.flush();
	return oss.str();
    }


}
#endif /* OCRCORRECTION_CANDIDATE_TCC__ */
