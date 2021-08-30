#ifndef OCRCORRECTION_PATTERN_COUNTER_H__
#define OCRCORRECTION_PATTERN_COUNTER_H__

#include <map>
#include "../Pattern/Pattern.h"

namespace OCRCorrection {


    /**
     * @brief This class is used during the profiling process to add up
     *        frequencies for patterns and character n-grams.
     */
    class PatternCounter {
    public:


	typedef std::map< csl::Pattern, float >::iterator PatternIterator;
	typedef std::map< csl::Pattern, float >::const_iterator ConstPatternIterator;

	/**
	 * @brief standard constructor
	 */
	PatternCounter() :
	    maxNGramSize_( 5 ) {

	}

	void clear() {
	    patternCount_.clear();
	    nGramCount_.clear();
	}


	void registerPattern( const csl::Pattern& pattern, float addValue = 1 ) {
	    patternCount_[pattern] += addValue;
	}


	/**
	 * @brief
	 */
	void registerNGrams(const std::wstring& correct, float addValue = 1 ) {

	    // also register char-n-grams
	    for( size_t offset = 0; offset < correct.length(); ++offset ) {
		for( size_t length = 1; length <= maxNGramSize_ ; ++length ) {
		    if( ( offset + length ) > correct.length() ) {
			break;
		    }
		    nGramCount_[ correct.substr( offset, length ) ] += addValue;
		}
	    }
	}

	float getPatternCount( csl::Pattern const& pattern ) const {
	    std::map< csl::Pattern, float >::const_iterator it = patternCount_.find( pattern );
	    if( it != patternCount_.end() )
		return it->second;
	    else return 0;
	}

	float getNGramCount( std::wstring const& nGram ) const {
	    std::map< std::wstring, float >::const_iterator it = nGramCount_.find( nGram );
	    if( it != nGramCount_.end() )
		return it->second;
	    else return 0;
	}

	PatternIterator patternsBegin() {
	    return patternCount_.begin();
	}

	PatternIterator patternsEnd() {
	    return patternCount_.end();
	}
	ConstPatternIterator begin() const {
	    return patternCount_.begin();
	}
	ConstPatternIterator end() const {
	    return patternCount_.end();
	}


	/////////  P R I V A T E  /////////////////
    private:
	typedef float count_t;

	size_t maxNGramSize_;

	/**
	 * @brief This data structure maps the frequencies of the known patterns
	 */
	std::map< csl::Pattern, count_t > patternCount_;

	/**
	 * @brief This data structure maps the frequencies of the correct words
	 */
	std::map< std::wstring, count_t > nGramCount_;


    };

} // eon
#endif // OCRCORRECTION_PATTERN_COUNTER_H__
