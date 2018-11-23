#ifndef OCRCORRECTION_CANDIDATE_H
#define OCRCORRECTION_CANDIDATE_H OCRCORRECTION_CANDIDATE_H

#include<string>
#include<DictSearch/DictSearch.h>
#include<Pattern/Trace.h>

/******************************************************/
/***********  IMPORTANT *******************************/

/*   DO NOT INCLUDE THIS HEADER FILE ANYWHERE         */

/*   Include Document.h instead                       */
/******************************************************/

namespace OCRCorrection {

    /**
     * @brief Represents one candidate for a token.
     *
     * In the 2010 version it inherits from csl::DictSearch::Interpretation.
     */
    class Candidate :	public csl::DictSearch::Interpretation
    {
    public:

	/******************** CONSTRUCTORS/ DESTRUCTORS ************************/
	/**
	 * @namem Constructors/ Destructor
	 */
	// @{

	inline Candidate();

	inline Candidate( csl::DictSearch::Interpretation const& interpretation );

	inline Candidate( csl::Interpretation const& interpretation, csl::DictSearch::iDictModule const& dm );

	// @}

	/******************** GETTERS ************************/
	/**
	 * @name Getters
	 */
	//@{

        /**
         * @brief returns a read/write reference of the candidate string
         * @return
         */
        inline std::wstring&  getString();

        inline const std::wstring&  getString() const;

        inline size_t getFrequency() const;

        inline size_t getDlev() const;

        inline float getVoteWeight() const;

	csl::Trace const& getOCRTrace() const {
	    return ocrTrace_;
	}

	/**
	 * @todo Make this method protected
	 */
	csl::Trace& getOCRTrace() {
	    return ocrTrace_;
	}

	inline bool isUnknown() const {
		return getOCRTrace().isUnknown() or
			getHistInstruction().isUnknown();
	}
	//@}

	/******************** SETTERS ************************/
	/**
	 * @name Setters
	 */
	//@{
	inline void setOCRTrace( csl::Trace const& ocri );

	inline void setVoteWeight( float w );
	//@}

	/******************** Serialize ************************/
	/**
	 * @name Serialize
	 *
	 * theil:{teil+[(t_th,0)]}+ocr[(i_j,3)],voteWeight=0.99
	 */
	//@{

	void print( std::wostream& os = std::wcout ) const;

	inline std::wstring toString() const;

	size_t parseFromString( std::wstring const& str, size_t offset );

	//@}

    protected:


    private:
	csl::Trace ocrTrace_;
	float voteWeight_;

    }; // class Candidate
    static inline std::wostream&
    operator<<(std::wostream& os, const Candidate& cand) {
	    cand.print(os);
	    return os;
    }

} // ns OCRCorrection

#endif
