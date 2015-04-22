#ifndef OCRCORRECTION_ALPHASCORE_CXX
#define OCRCORRECTION_ALPHASCORE_CXX OCRCORRECTION_ALPHASCORE_CXX

#include "../Stopwatch.h"

namespace OCRCorrection {


    AlphaScore::AlphaScore() :
	alpha_( 0 ) {
    }

    void AlphaScore::setAlpha( double alpha ) {
	alpha_ = alpha;
    }

    template< typename Document_t >
    void AlphaScore::addScoreToDocument( Document_t& document ) const {

	Stopwatch watch;
	watch.start();
	//std::wcout<<"AlphaScore::addScoreToDocument: do nothing yet"<<std::endl;
	
	// Compute maximum standard Lev. distance and maximum Frequency
	size_t maxDLev = 0;
	double maxFreq = 0;
	
	for( typename Document_t::iterator iter = document.begin(); iter != document.end(); ++iter) {
	    for( size_t i = 0; i < iter->getNrOfCandidates(); i++ ) {
		if( iter->candidateAt(i).getDlev()>maxDLev ) 
		    maxDLev = iter->candidateAt(i).getDlev();
		if(iter->candidateAt(i).getFrequency()>maxFreq)
		    maxFreq = iter->candidateAt(i).getFrequency();
	    }
	} 

	maxFreq = log10( (float) maxFreq );
	//std::wcerr<<"max dlev is "<<maxDLev<<", max freq is "<<maxFreq<<std::endl;
	//std::wcout<<"max dlev is "<<maxDLev<<", max freq is "<<maxFreq<<std::endl;
	//Set the combined score
	double alphaScore;
	double dLev_norm, freq_norm;
	for( typename Document_t::iterator iter = document.begin(); iter != document.end(); ++iter){
	    for(size_t i = 0; i < iter->getNrOfCandidates(); i++){
		dLev_norm =(double) iter->candidateAt(i).getDlev();
		freq_norm =(double) iter->candidateAt(i).getFrequency();
		freq_norm=log10(freq_norm);
		// std::wcout<<"burdayim";
		dLev_norm = 1 - ( dLev_norm / (double) maxDLev );
		freq_norm = freq_norm / (double) maxFreq;
		//std::wcout<<"dlev norm="<<dLev_norm<<"freq norm="<<freq_norm<<std::endl;			
		alphaScore = ( ( alpha_ * dLev_norm ) + ( ( 1.0 - alpha_ ) * freq_norm ) ) / 2;
		//std::wcout<<"Alpha Score= "<<alphaScore<<"alpha= "<<alpha_<<std::endl;
		iter->candidateAt(i).setAlphaScore(alphaScore);
	    }
	} 

	std::wcout<<"OCRCorrection::AlphaScore: Alphascored "<<document.getNrOfTokens()<<" tokens in "<<watch.readMilliseconds()<<" milliseconds"<<std::endl;;
	return;
    }
}

#endif
