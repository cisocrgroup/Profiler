#ifndef OCRC_SIMPLEENRICH_H
#define OCRC_SIMPLEENRICH_H OCRC_SIMPLEENRICH_H

#include <string>

#include "../DictSearch/DictSearch.h"
#include "../INIConfig/INIConfig.h"
#include "../Document/Document.h"

namespace OCRCorrection {

    /**
     * @brief This class can be used to add correction suggestions to a document object
     *        in a very simple way, just by applying a csl::DictSearch dictionary query
     *        and ranking the result list in a simple way.
     */
    class SimpleEnrich {
    public:
	SimpleEnrich( csl::INIConfig const& iniConf ) :
	    limitNrOfCandidates_( -1 ) {
	    dictSearch_.readConfiguration( iniConf );

	}

	void limitNrOfCandidates( size_t l ) {
	    limitNrOfCandidates_ = l;
	}

	/**
	 * @brief Pass a pointer to a document object, and correction suggestions will be added
	 *        to the tokens of your document.
	 *
	 */
	void enrichDocument( Document* doc ) {
	    csl::DictSearch::CandidateSet candset;

	    // make unique correction suggestions
	    std::set< std::wstring > seen;

	    size_t count = 0;
	    for( Document::iterator token = doc->begin(); token != doc->end(); ++token ) {
		if( token->isNormal() && token->getWOCR_lc().size() > 3 ) {
		    if( ++count % 1000 == 0 ) {
			std::wcerr << "Analyzed " << count / 1000 << "k tokens" << std::endl;
		    }
		    candset.clear();
		    dictSearch_.query( token->getWOCR_lc(), &candset );
		    std::sort( candset.begin(), candset.end() );

		    if( candset.empty() ) {
			token->setSuspicious( true );
		    }
		    else {
			if( candset.at( 0 ).getLevDistance() == 0 ) {
			    token->setSuspicious( false );
			}
			else {
			    token->setSuspicious( true );
			}

			seen.clear();
			size_t candCount = 0;
			for( csl::DictSearch::CandidateSet::const_iterator cand = candset.begin();
			     cand != candset.end();
			     ++cand ) {
			    if( ( limitNrOfCandidates_ != (size_t)-1 ) && ( candCount > limitNrOfCandidates_  ) ) break;

			    // candidate string not suggested before
			    if( ( seen.insert( cand->getWord() ).second == true ) ) {
				token->addCandidate( *cand );
			    }
			}
		    }
		}

	    }



	}


    private:

	csl::DictSearch dictSearch_;
	size_t limitNrOfCandidates_;
    }; // class SimpleEnrich


}



#endif
