#ifndef OCRCORRECTION_LEVENSHTEINWEIGHTS_H
#define OCRCORRECTION_LEVENSHTEINWEIGHTS_H OCRCORRECTION_LEVENSHTEINWEIGHTS_H

#include<string>
#include<map>
#include<math.h>
#include<csl/Pattern/Pattern.h>
#include<csl/Pattern/PatternWeights.h>
#include<Document/Document.h>

namespace OCRCorrection {
    
    /**
     * This class holds the weights for the computation of a symbol-dependent weighted 
     * Levenshtein distance.
     *
     * @author Ulrich Reffle <uli@cis.uni-muenchen.de>
     * @date Jan 2008
     */
    class LevenshteinWeights : public csl::PatternWeights {
    public:
	
	/**
	 * @brief initialize function
	 */
	void init();
	
	/**
	 * @brief computes the Levenshtein weights and stores them in the freqAndWeightsPattern_ member
	 */
	void finish();
	
	/**
	 * @brief registers an error pattern
	 */
	void registerPattern( const csl::Pattern& pattern, float probability = 1 );
 
	/**
	 * @brief registers a correct word
	 */
	void registerCorrect( const std::wstring& correct, float probability = 1 );
	
	
	float getFreq( csl::Pattern const& pattern ) const;
	float getFreq( std::wstring const& substring ) const;
	
	
	/**
	 * @brief prints the pattern frequencies
	 */
	void printFreqPattern() const;
	
	/**
	 * @brief prints the frequencies of bigrams and unigrams
	 */
	void printFreqCorrect() const;
	
	
	/////////  P R I V A T E  /////////////////
	private:
	typedef float count_t;

	/**
	 * @brief This data structure maps the frequencies of the known patterns 
	 */
	std::map< csl::Pattern, count_t > freqPattern_;

	/**
	 * @brief This data structure maps the frequencies of the correct words
	 */
	std::map< std::wstring, count_t > freqCorrWord_;
	
   }; // class LevenshteinWeights



} // ns OCRCorrection

#endif
