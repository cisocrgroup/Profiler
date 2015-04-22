#include "./LevenshteinWeights.h"

namespace OCRCorrection{
    void LevenshteinWeights::init() {
    }
	
    void LevenshteinWeights::registerPattern( const csl::Pattern& pattern, float probability ){
	freqPattern_[pattern] += probability;
    }

    void LevenshteinWeights::registerCorrect(const std::wstring& correct, float probability ) {
	std::wstring index;
	for( size_t i = 0; i < correct.length(); i++ ) {
	    index = correct.at( i );
	    freqCorrWord_[index] += probability;
	    
	    // also register char-n-grams
	    for( size_t n = 2; n <= 5 ; ++n ) {
		if( ( i + n - 1 ) < correct.length() ) {
		    break;
		}
		index += correct.at( i + n - 1 );
		freqCorrWord_[index] += probability;
	    }
	}
    }
    
    float LevenshteinWeights::getFreq( csl::Pattern const& pattern ) const {
	std::map< csl::Pattern, count_t >::const_iterator it = freqPattern_.find( pattern );
	if( it != freqPattern_.end() ) {
	    return it->second;
	}
	else return UNDEF;
    }
    

    float LevenshteinWeights::getFreq( std::wstring const& substring ) const {
	std::map< std::wstring, count_t >::const_iterator it = freqCorrWord_.find( substring );
	if( it != freqCorrWord_.end() ) {
	    return it->second;
	}
	else return UNDEF;
    }
    
    
    void LevenshteinWeights::printFreqPattern() const{
	for(std::map< csl::Pattern, count_t >::const_iterator it = freqPattern_.begin(); it != freqPattern_.end(); it++){
	    std::wcout << it->first.getLeft() << '-' << it->first.getRight() << " : " << it->second << std::endl;
	}
    }

    void LevenshteinWeights::printFreqCorrect() const{
	for(std::map< std::wstring, count_t >::const_iterator it = freqCorrWord_.begin(); it != freqCorrWord_.end(); it++){
	    std::wcout << it->first << " : " << it->second << std::endl;
	}
    }

    void LevenshteinWeights::finish() {
	float max = 0.0;
	for(std::map< csl::Pattern, count_t>::const_iterator it = freqPattern_.begin(); it != freqPattern_.end(); it++){
	    if( freqCorrWord_[it->first.getLeft()] < 1 ) {// Gegen Division durch null
		setWeight( it->first,  UNDEF );			
	    }
	    else{
		float w = (float)(it->second) / (float)freqCorrWord_[it->first.getLeft()];
		setWeight( it->first, -log(w) ); 
		if( getWeight ( it->first ) > max ) max = getWeight ( it->first );
	    }
	}
	//Die Werte normieren durch maximales Patterngewicht
	for(std::map< csl::Pattern, count_t >::const_iterator it = freqPattern_.begin(); it != freqPattern_.end(); it++) {
	    setWeight ( it->first, ( getWeight( it->first ) / max ) );			
	}
	// printFreqPattern();
	// printFreqCorrect();
	// printPatternWeights();
	    
    }

}
