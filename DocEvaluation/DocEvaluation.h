#ifndef OCRC_DOCEVALUATION_H
#define OCRC_DOCEVALUATION_H OCRC_DOCEVALUATION_H

#include<iostream>

#include "../Document/Document.h"
#include "../LevDistance/LevDistance.h"
#include"./Counter.h"

namespace OCRCorrection {

    class DocEvaluation {
    public:
	DocEvaluation();

	enum OutputCategory {
	    ERRDET_FALSENEG,
	    ERRDET_FALSEPOS
	};

	void analyzeDocument( Document& doc );

	void setVerbose( bool );
	void setOutputCategory( OutputCategory const& cat, bool b );
	bool doOutput( OutputCategory const& cat );


    private:
	static void findHyphenation( Document& doc );

	Counter counter_;

	csl::LevDistance levDistanceComputer_;

	bool verbose_;

	std::set< OutputCategory > outputCategories_;

    };


    DocEvaluation::DocEvaluation() :
	verbose_( true ) {
    }


    void DocEvaluation::setOutputCategory( OutputCategory const& cat, bool b ) {
	if( b ) {
	    outputCategories_.insert( cat );
	}
	else {
	    outputCategories_.erase( cat );
	}
    }

    bool DocEvaluation::doOutput( OutputCategory const& cat ) {
	return ( outputCategories_.find( cat ) != outputCategories_.end() );
    }



    void DocEvaluation::setVerbose( bool b ) {
	verbose_ = b;
    }


    void DocEvaluation::analyzeDocument( Document& doc ) {
	counter_.clear();

	findHyphenation( doc );

	for( Document::iterator tok = doc.begin(); tok != doc.end(); ++ tok ) {
	    //////////////////////////////////////
	    if( ! tok->isNormal() ) {
		//if( verbose_ ) std::wcout << "[NN] " << tok->getGroundtruth().getWOrig_lc() << " - " << tok->getWOCR_lc() << std::endl;
		++counter_( Counter::not_normal );
		continue;
	    }
	    if( tok->getWOCR_lc().size() <= 3 ) {
		++counter_( Counter::not_long );
		continue;
	    }
	    ++counter_( Counter::normal_and_long );

	    //////////////////////////////////////


	    if( ! tok->hasGroundtruth() ) {
		throw OCRCException( (*tok).getWOCR_lc() + L" has no groundtruth" );
	    }

	    if( tok->getGroundtruth().getWOrig_lc() == tok->getWOCR_lc() ) {
		//if( verbose_ ) std::wcout << "[correct] " << tok->getGroundtruth().getWOrig_lc() << " - " << tok->getWOCR_lc() << std::endl;
		++counter_( Counter::correct );
	    }
	    else {
		++counter_( Counter::error );

		if( tok->isSuspicious() ) {
		    ++counter_( Counter::error_suspicious );
		}

		bool destroyed = false;
		std::wstring destroyed_comment;

		// if( tok->getGroundtruth().getOCRTrace() == L"*DESTROYED*"
		//     || tok->getGroundtruth().getClassified() == L"unknown"
		//     ) {

		//     destroyed = true;
		// }
		// else {

		//     if( levDistanceComputer_.getLevDistance( tok->getGroundtruth().getWOrig_lc(), tok->getWOCR_lc() ) > 2 ) {
		// 	destroyed = true;
		//     }
		// }


		if( ( tok->getGroundtruth().getWOrig_lc().find( L" " ) != std::wstring::npos )
		    || ( tok->getWOCR_lc().find( L" " ) != std::wstring::npos ) ) {
		    destroyed_comment = L"SPACE FOUND";

		    destroyed = true;
		}
 		else if( tok->hasProperty( Token::HYPHENATION_1ST ) || tok->hasProperty( Token::HYPHENATION_2ND ) ) {
		    destroyed_comment = L"HYPHENATION";
		    destroyed = true;
		}
		else if( levDistanceComputer_.getLevDistance( tok->getGroundtruth().getWOrig_lc(), tok->getWOCR_lc() ) > 2 ) {
		    destroyed_comment = L"LEV-THRESHOLD";
		    destroyed = true;
		}


		if( destroyed ) {
		    ++counter_( Counter::error_destroyed );
		    // if( verbose_ ) {
		    // 	std::wcout << "[error_destroyed][" << destroyed_comment << "] "
		    // 		   << tok->getGroundtruth().getWOrig_lc() << " - " << tok->getWOCR_lc() << std::endl;
		    // 	std::wcout << "GT ocrTrace=" << tok->getGroundtruth().getOCRTrace() << std::endl;
		    // }
		    // std::wcout << std::endl << std::endl << std::endl;
		    continue;
		}


		++counter_( Counter::error_correctable );
		if( verbose_ ) {
		    std::wcout << "[error_correctable] " << tok->getGroundtruth().getWOrig_lc() << " - " << tok->getWOCR_lc() << std::endl;
		    std::wcout << "ocrTrace=" << tok->getGroundtruth().getOCRTrace() << std::endl;
		}


		if( tok->getNrOfCandidates() != 0 ) {
		    ++counter_( Counter::error_correctable_hasCandidates );

		    counter_( Counter::error_correctable_hasCandidates_candidateSum ) += tok->getNrOfCandidates();

		    // pos counts in an intuitive (not IT-like) style: the 1st candidate has pos 1.
		    // pos==0 means the cand was not found (yet)
		    size_t pos = 0;
		    size_t i = 0;
		    for( Token::CandidateIterator cand = tok->candidatesBegin(); cand != tok->candidatesEnd() && (pos == 0); ++cand ) {
			++i;

			if( tok->getWOCR_lc() == L"madchen" ) {
			    if( verbose_ ) std::wcout << cand->getWord() << " for " << tok->getGroundtruth().getWOrig_lc() << " at pos " << i << std::endl;
			}

			std::wstring cand_lc = cand->getWord();
			for( std::wstring::iterator c = cand_lc.begin(); c != cand_lc.end(); ++c ) {
                                *c = towlower(*c);
			}

			if( cand_lc == tok->getGroundtruth().getWOrig_lc() ) {
			    pos = i;
			}
		    }

		    if( pos == 0 ) {
			++counter_( Counter::error_correctable_hasCandidates_nohit );
			if( verbose_ ) std::wcout << "nohit" << std::endl;
		    }
		    else {
			if( verbose_ ) std::wcout << "pos=" << pos << std::endl;
			if( pos == 1 ) {
			    ++counter_( Counter::error_correctable_hasCandidates_1best );
			}

			if( pos <= 3 ) {
			    ++counter_( Counter::error_correctable_hasCandidates_3best );
			}

			if( pos <= 5 ) {
			    ++counter_( Counter::error_correctable_hasCandidates_5best );
			}

			if( pos <= 10 ) {
			    ++counter_( Counter::error_correctable_hasCandidates_10best );
			}

			if( pos <= 50 ) {
			    ++counter_( Counter::error_correctable_hasCandidates_50best );
			}
		    }

		}
		else {
		    ++counter_( Counter::error_correctable_hasNoCandidates );
		    if( verbose_ ) std::wcout << "no cands" << std::endl;
		}

		if( verbose_ ) std::wcout << std::endl << std::endl << std::endl;
	    }




	}
	std::wcout
	    << "not_long                                " << counter_( Counter::not_long ) << std::endl
	    << "normal_and_long                         " << counter_( Counter::normal_and_long ) << std::endl
	    << "correct                                 " << counter_( Counter::correct ) << std::endl
	    << "error                                   " << counter_( Counter::error ) << std::endl
	    << "error_destroyed                         " << counter_( Counter::error_destroyed ) << std::endl
	    << "error_correctable                       " << counter_( Counter::error_correctable ) << std::endl
	    << "error_correctable_hasNoCandidates       " << counter_( Counter::error_correctable_hasNoCandidates ) << std::endl
	    << "error_correctable_hasCandidates         " << counter_( Counter::error_correctable_hasCandidates ) << std::endl
	    << "error_correctable_hasCandidates_1best   " << counter_( Counter::error_correctable_hasCandidates_1best ) << std::endl
	    << "error_correctable_hasCandidates_3best   " << counter_( Counter::error_correctable_hasCandidates_3best ) << std::endl
	    << "error_correctable_hasCandidates_5best   " << counter_( Counter::error_correctable_hasCandidates_5best ) << std::endl
	    << "error_correctable_hasCandidates_10best   " << counter_( Counter::error_correctable_hasCandidates_10best ) << std::endl
	    << "error_correctable_hasCandidates_50best  " << counter_( Counter::error_correctable_hasCandidates_50best ) << std::endl
	    << "error_correctable_hasCandidates_nohit   " << counter_( Counter::error_correctable_hasCandidates_nohit ) << std::endl
	    << std::endl
	    << "avg nr of candidates on errors          " << (float)counter_( Counter::error_correctable_hasCandidates_candidateSum ) / counter_( Counter::error_correctable_hasCandidates ) << std::endl
	    << std::endl
	    << "Error Detection Recall                  " << counter_( Counter::error_suspicious ) << " / " << counter_( Counter::error )
	    << " = " <<  (float)counter_( Counter::error_suspicious ) / counter_( Counter::error )  << std::endl
	    ;
    }


    /**
     * This is stolen from class Document. But, in this case the hyphenation nifo is taken from groundtruth
     */
    void DocEvaluation::findHyphenation( Document& doc ) {
	std::wstring hyphenationMarks = std::wstring( L"\u00AC" );
	std::wstring mergedWord;


	if( doc.getNrOfTokens() < 4 ) return;

	/*
	 *  hyphe       ¬          \n         nation
	 *   i -2      i-1         i           i+1
	 *
	 */
        for( size_t i = 2; i < doc.getNrOfTokens()-1 ; ++i ) {
	    if( doc.at( i ).getGroundtruth().getWOrig() == L"\n" ) {
		if( ( doc.at( i - 1 ).getGroundtruth().getWOrig().size() == 1 ) &&
		    ( hyphenationMarks.find( doc.at( i - 1 ).getGroundtruth().getWOrig().at( 0 ) ) != std::wstring::npos ) ) {

		    mergedWord = doc.at( i - 2 ).getWOCR() + doc.at( i + 1 ).getWOCR();

		    doc.at( i - 2 ).setProperty( Token::HYPHENATION_1ST, true );
		    doc.at( i - 2 ).setHyphenationMerged( mergedWord );
		    doc.at( i - 1 ).setProperty( Token::HYPHENATION_MARK, true );
		    doc.at( i + 1 ).setProperty( Token::HYPHENATION_2ND, true );
		    doc.at( i + 1 ).setHyphenationMerged( mergedWord );

		}
	    }

	}
    }



} // ns


#endif
