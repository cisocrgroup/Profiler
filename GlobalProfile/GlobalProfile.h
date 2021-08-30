#ifndef OCRC_GLOBALPROFILE_H
#define OCRC_GLOBALPROFILE_H OCRC_GLOBALPROFILE_H

#include "../GlobalProfile/PatternContainer.h"

namespace OCRCorrection {

    /**
     * @brief represents a global text and error profile
     */
    class GlobalProfile {
    public:
	class DictDistributionPair; // forward declaration

	/**
	 * @brief This is where the Profiler reads the probabilities for historical variant patterns. At the end
	 *        of each iteration, the probabilities are updated based on values in {@link histCounter_}
	 */
	PatternContainer histPatternProbabilities_;

	/**
	 * @brief This is where the Profiler reads the probabilities for ocr error patterns. At the end
	 *        of each iteration, the probabilities are updated based on values in {@link ocrCounter_}
	 */
	PatternContainer ocrPatternProbabilities_;

	/**
	 * @brief This counter keeps track of how many of the tokens were assigned to
	 *        which dictionary
	 *
	 */
	std::map< std::wstring, DictDistributionPair > dictDistribution_;

	std::wstring createdAt_;

    public:

	/**
	 * @brief This is a simple datatype for that part of the profile describing the distribution of tokens
	 *        among the active dictionaries. For each dictionary a structure of this type provides two values:
	 *        the absolute and relative proportion as described below.
	 */
	class DictDistributionPair {
	public:
	    /**
	     * @brief Holds the absolute number of interpreted tokens that were assigned to the respective dictionary.
	     */
	    float frequency;
	    /**
	     * @brief Holds the proportion of interpreted tokens that were assigned to the respective dictionary.
	     */
	    float proportion;

	    DictDistributionPair() :
		frequency( 0 ),
		proportion( 0 ) {
	    }
	}; // class DictDistributionPair
    }; // class GlobalProfile

} // eon

#endif
