#include<string>
#include<sstream>
#include<csl/TransTable/TransTable.h>
#include<csl/DictSearch/DictSearch.h>
#include<csl/Pattern/ComputeInstruction.h>
#include<Document/Document.h>
#include<Exceptions.h>
#include <time.h>

#include "CompoundDictModule.h"


namespace OCRCorrection {

/**
 * @brief Is used to automatically add groundtruth traces to a Document, based on wocr and worig
 */
    class GuessTraces {
    public:
	enum Interactive {CLICK, GUESS_ALL, GUESS_UNAMBIGUOUS};

	GuessTraces():
	    interactive_( CLICK )
	    {

	    }

	~GuessTraces() {
	}

	void setInteractive( Interactive i ) {
	    interactive_ = i;
	}
    
	csl::DictSearch& getDictSearch() {
	    return dictSearch_;
	}
    
	void processDocument( Document* document ) {
	    
	
	    csl::DictSearch::CandidateSet cands;

	    csl::PatternWeights patternProbabilities;
	    patternProbabilities.setDefault( csl::PatternWeights::PatternType( 1, 1 ), 1e-5 );
	    patternProbabilities.setDefault( csl::PatternWeights::PatternType( 1, 0 ), 1e-5 );
	    patternProbabilities.setDefault( csl::PatternWeights::PatternType( 0, 1 ), 1e-5 );
	    patternProbabilities.setDefault( csl::PatternWeights::PatternType( 1, 2 ), 1e-5 );
	    patternProbabilities.setDefault( csl::PatternWeights::PatternType( 2, 1 ), 1e-5 );
	    patternProbabilities.setSmartMerge( true );



	    csl::ComputeInstruction instructionComputer;
	    instructionComputer.connectPatternProbabilities( patternProbabilities );
	    instructionComputer.setMaxNumberOfInstructions( 10 );
	    std::vector< csl::Instruction > ocrInstructions; // is filled later in the document loop
	

	
	    for( OCRCorrection::Document::iterator token = document->begin(); // for all tokens
		 token != document->end();
		 ++token ) {

		if( ! token->isNormal() ) continue;

		std::wstring wOCR = token->getWOCR();
		std::wstring wOCR_lc = wOCR;
		for( std::wstring::iterator c = wOCR_lc.begin(); c != wOCR_lc.end(); ++c ) {
		    *c = std::tolower( *c );
		}
	    
		if( ! token->hasGroundtruth() ) {
		    std::wcerr << "ALARM: " << token->getWOCR() << " hat kein GT" << std::endl;
		}

		std::wstring wOrig = token->getGroundtruth().getWOrig();
		std::wstring wOrig_lc = wOrig;
		for( std::wstring::iterator c = wOrig_lc.begin(); c != wOrig_lc.end(); ++c ) {
		    *c = std::tolower( *c );
		}

		//std::wcout << "Worig=" << wOrig <<", wOCR=" << wOCR <<  std::endl;
	
		// OCRCorrection::Document does not distinguish between short words and non-words. They are all "not normal"
		bool is_a_word = 
		    //token->isNormal() || 
		    ( wOrig_lc.find_first_of( L"abcdefghijklmnopqrstuvwxyzäöüß" ) != std::wstring::npos );
	    
	    
	    
		cands.reset();
		if( is_a_word ) dictSearch_.query( wOrig_lc, &cands );
	    
		// here, a custom-made sort-operator can be passed as 3rd argument
		std::sort( cands.begin(), cands.end() );

		token->getGroundtruth().setClassified( L"null" );

		if( wOrig.empty() ) {
		    token->getGroundtruth().setClassified( L"split_tail" );
		}

		if( wOCR == L" " ) {
		    token->getGroundtruth().setVerified( Token::VERIFIED_FALSE );
		}
		else if( wOCR == L"\n" ) {
		    token->getGroundtruth().setVerified( Token::VERIFIED_FALSE );
		}
		else {
		    std::wstring is_a_word_string = ( is_a_word )? L"true" : L"false";
		    if( token->getGroundtruth().getClassified() != L"null" ) {
			// was set before; do not over-rule
		    }
		    else if( cands.size() == 0 ) {
			token->getGroundtruth().setClassified( L"unknown" );
		    }
		    else if( cands.at( 0 ).getInstruction().size() == 0 ) {
			token->getGroundtruth().setClassified( L"modern" );
			token->getGroundtruth().setBaseWord( cands.at( 0 ).getBaseWord() );
			token->getGroundtruth().setHistTrace( cands.at( 0 ).getHistInstruction().toString() );
		    }
		    else {
			token->getGroundtruth().setClassified( L"hypothetic" );
			token->getGroundtruth().setBaseWord( cands.at( 0 ).getBaseWord() );
			token->getGroundtruth().setHistTrace( cands.at( 0 ).getHistInstruction().toString() );
		    }
		
		    ocrInstructions.clear();
		
		    try {
			instructionComputer.computeInstruction( wOrig_lc, wOCR_lc, &ocrInstructions );

			// if instructions include 4 or more patterns (check is for 1st instruction, but they should all have equal length)
			if( ! ocrInstructions.empty() && ocrInstructions.at(0).size() >= 3 ) {
			    token->getGroundtruth().setOCRTrace( L"*DESTROYED*" );
			    token->getGroundtruth().setVerified( Token::VERIFIED_GUESSED );
			}
			else {
			    token->getGroundtruth().setOCRTrace( ocrInstructions.at(0).toString() );
			}
		    } catch( std::exception& exc ) {
			
			std::wcerr << "Dirk_groundtruth: caught exception in csl::...::computeInstruction.: " << csl::CSLLocale::string2wstring( exc.what() ) << std::endl
				   << "wOrig=" << wOrig_lc << ", wOCR=" <<  wOCR_lc << std::endl
				   << "  Will ignore it" << std::endl;
			token->getGroundtruth().setOCRTrace( L"*DESTROYED*" );
			
		    }

		    enum GuessOCR { NONE, FIRST, DESTROYED };
		    GuessOCR guessOCR = NONE;


		    token->getGroundtruth().setVerified( Token::VERIFIED_GUESSED );
		

		} // all words but newlines and spaces
	    
	    } // for each token
	
	
	}
    

    private:
    
	csl::DictSearch dictSearch_; 
    
	int verbose_;
	Interactive interactive_;

    }; // class GuessTraces

}
