#ifndef OCRCORRECTION_ALPHASCORE_H
#define OCRCORRECTION_ALPHASCORE_H OCRCORRECTION_ALPHASCORE_H

namespace OCRCorrection {

    /**
     * @brief 
     * This class adds a new score to all Tokens of a Document-object: The score is a value combined 
     * of a distance measure and a frequency score
     *
     * The method is described in:
     * Christian Strohmaier, Christoph Ringlstetter, Klaus U. Schulz and Stoyan Mihov:
     * A Visual and Interactive Tool for Optimizing Lexical Postcorrection of OCR-Results.
     * Proceedings of the Workshop on Document Image Analysis and Retrieval DIAR'03, 2003. 
     * Available at: http://www.cis.uni-muenchen.de/people/Schulz/pubs.html
     *
     */
    class AlphaScore {
    public:
	/********************** Constructors/ Destructors ************************/
      inline AlphaScore();
	

	/**************************** Apply weights ******************************/

	/**
	 * @brief add combined score to complete document
	 *
	 * iterate through all tokens and candidates of a Document-object and add 
	 * the score to all  tokens
	 */
	template< typename Document_t >
	inline void addScoreToDocument( Document_t& document ) const;
	
	/**
	 * @brief get the current value for alpha
	 * @return the current value for alpha
	 */
	inline double getAlpha() const;
	/**
	 * @brief set a new value for alpha
	 *
	 * @param alpha a new value for alpha
	 */
	inline void setAlpha( double alpha );

    private:
	/**
	 * The current balance value
	 */
	double alpha_;
    };



} // ns OCRCorrection
#include "math.h"
#include "./AlphaScore.tcc"

#endif
