#ifndef OCRC_PROFILER_CXX
#define OCRC_PROFILER_CXX OCRC_PROFILER_CXX

#include "./Profiler.h"
#include "Utils/NoThousandGrouping.h"
#include "./Profiler_Token.tcc"

namespace OCRCorrection {

    Profiler::Profiler() :
    	freqList_(),
    	baseWordFrequencyDic_( 0 ),
    	htmlStream_( 0 )
    {


    }

    Profiler::~Profiler() {
	if( htmlStream_ ) {
	    delete htmlStream_;
	    htmlStream_ = 0;
	}

	// delete all external DictModules of DictSearch
	for( std::vector< csl::DictSearch::iDictModule* >::iterator it = externalDictModules_.begin();
	     it != externalDictModules_.end();
	     ++it ) {
	    delete( *it );
	}
    }

    void Profiler::readConfiguration( char const* configFile ) {
	csl::INIConfig iniConf( configFile );
	readConfiguration( iniConf );
    }

    void Profiler::readConfiguration( csl::INIConfig const& iniConf ) {


	config_.nrOfIterations_ = iniConf.getint( "global:numberOfIterations" );
	if( config_.nrOfIterations_ == (size_t)-1 ) throw OCRCException( "OCRC::Profiler::readConfiguration: no value found for numberOfIterations" );

	config_.patternCutoff_hist_ = iniConf.getdouble( "global:patternCutoff_hist" );
	config_.patternCutoff_ocr_ = iniConf.getdouble( "global:patternCutoff_ocr" );

	config_.histPatternSmoothingProb_ = iniConf.getdouble( "global:histPatternSmoothingProb" );
	config_.ocrPatternSmoothingProb_ = iniConf.getdouble( "global:ocrPatternSmoothingProb" );

	config_.ocrPatternStartProb_ = iniConf.getdouble( "global:ocrPatternStartProb" );

	config_.resetHistPatternProbabilities_ = iniConf.getint( "global:resetHistPatternProbabilities" );
	config_.resetOCRPatternProbabilities_ = iniConf.getint( "global:resetOCRPatternProbabilities" );

	config_.donttouch_hyphenation_ = iniConf.getbool( "global:donttouch_hyphenation" );
	config_.donttouch_lineborders_ = iniConf.getbool( "global:donttouch_lineborders" );


	freqList_.loadFromFile( iniConf.getstring( "language_model:freqListFile" ),
				iniConf.getstring( "language_model:patternWeightsFile" ) );

	if (iniConf.hasKey("adaptive_profiling:writeAdaptiveDictionary")) {
		config_.writeAdaptiveDictionary_ =
			iniConf.getbool("adaptive_profiling:writeAdaptiveDictionary");
		if (config_.writeAdaptiveDictionary_) {
			config_.adaptiveDictionaryPath_ =
				iniConf.getstring("adaptive_profiling:adaptiveDictionaryPath");
		}
	}


	dictSearch_.readConfiguration( iniConf );

	htmlWriter_.readConfiguration( iniConf );
    }


    void Profiler::createProfile( Document& sourceDoc ) {
	    doCreateProfile(sourceDoc);
	    sourceDoc.set_global_profile(globalProfile_);
    }

    void Profiler::initGlobalOcrPatternProbs(int itn) {
	    if (itn <= 1) {
                globalProfile_.ocrPatternProbabilities_.
		    setDefault( csl::PatternWeights::PatternType( 1, 1 ), config_.ocrPatternStartProb_ );
		globalProfile_.ocrPatternProbabilities_.
	    		setDefault( csl::PatternWeights::PatternType( 2, 1 ), config_.ocrPatternStartProb_ );
		globalProfile_.ocrPatternProbabilities_.
		    setDefault( csl::PatternWeights::PatternType( 1, 2 ), config_.ocrPatternStartProb_ );
		globalProfile_.ocrPatternProbabilities_.
		    setDefault( csl::PatternWeights::PatternType( 0, 1 ), config_.ocrPatternStartProb_ );
		globalProfile_.ocrPatternProbabilities_.
		    setDefault( csl::PatternWeights::PatternType( 1, 0 ), config_.ocrPatternStartProb_ );
	    } else {
		// remove default weights for all ocr operations, set smoothing weights instead
		globalProfile_.ocrPatternProbabilities_.setDefault(
				csl::PatternWeights::PatternType( 1, 1 ), config_.ocrPatternSmoothingProb_ );
		globalProfile_.ocrPatternProbabilities_.setDefault(
				csl::PatternWeights::PatternType( 2, 1 ), config_.ocrPatternSmoothingProb_ );
		globalProfile_.ocrPatternProbabilities_.setDefault(
				csl::PatternWeights::PatternType( 1, 2 ), config_.ocrPatternSmoothingProb_ );
		globalProfile_.ocrPatternProbabilities_.setDefault(
				csl::PatternWeights::PatternType( 0, 1 ), config_.ocrPatternSmoothingProb_ );
		globalProfile_.ocrPatternProbabilities_.setDefault(
				csl::PatternWeights::PatternType( 1, 0 ), config_.ocrPatternSmoothingProb_ );
	    }
    }

    void Profiler::doCreateProfile(Document& sourceDoc) {
	config_.print( std::wcerr );

	if( config_.nrOfIterations_ == 0 ) {
	    std::wcerr << "OCRC::Profiler::createNonAdaptiveProfile: config says 0 iterations, "
		    "so I do nothing." << std::endl;
	}

	prepareDocument( sourceDoc );



	// this means never to take actual frequencies from the corpus but to always take the formula
	// to compute that value. This switch is hard-wired now because we're not likely to change it.
	freqList_.doApplyStaticFreqs( false );

	freqList_.setHistPatternSmoothingProb( config_.histPatternSmoothingProb_ );

	instructionComputer_.connectPatternProbabilities( globalProfile_.ocrPatternProbabilities_ );


	//////// 1ST ITERATION ///////////////////
	initGlobalOcrPatternProbs(1);
	globalProfile_.ocrPatternProbabilities_.setSmartMerge(); // this means that pseudo-merges and splits like ab<>b ot ab<>a are not allowed

	//                --> true/false specifies if HTML output for 1st iteration is to be written to stdout
	bool doWriteHTML = ( config_.nrOfIterations_ == 1 );

	doIteration( 1, doWriteHTML );

	//////// 2ND AND FURTHER ITERATIONS ///////////////////
	initGlobalOcrPatternProbs(2);


	freqList_.connectPatternProbabilities( &globalProfile_.histPatternProbabilities_ );
 	// connectBaseWordFrequency ... is connected below
 	freqList_.setNrOfTrainingTokens( nrOfProfiledTokens_ );

	for( size_t iterationNr = 2; iterationNr <= config_.nrOfIterations_ ; ++ iterationNr ) {
	    // this has to be done again because the MinDic's position in memory changes
	    freqList_.connectBaseWordFrequency( baseWordFrequencyDic_ );
	    //                     do print in the last iteration
	    doIteration(  iterationNr, ( iterationNr == config_.nrOfIterations_ ) );
	}


// 	std::wofstream histPatternFile( "./histPatterns.xml" );
// 	globalProfile_.histPatternProbabilities_.writeToXML( histPatternFile );
// 	histPatternFile.close();

// 	std::wofstream ocrPatternFile( "./ocrPatterns.xml" );
// 	globalProfile_.ocrPatternProbabilities_.writeToXML( ocrPatternFile );
// 	ocrPatternFile.close();

    }

    void Profiler::doIteration( size_t iterationNumber, bool lastIteration ) {
            csl::Stopwatch iterationTime;

	std::wcout << "*** Iteration " << iterationNumber << " ***" << std::endl;

	// static_cast< csl::PatternProbabilities >( globalProfile_.ocrPatternProbabilities_ ).print( std::wcout );

	globalProfile_.dictDistribution_.clear();

	csl::DictSearch::CandidateSet tempCands;

	// note that the baseWordFrequency_ are of course carried to the next iteration,
	// but in the form of a DFA. That's why this variable is local to doIteration().
	std::map< std::wstring, float > baseWordFrequency;

	std::map< csl::Pattern, std::set< std::wstring > > ocrPatternsInWords;


	Evaluation evaluation( *this );

	htmlWriter_.newIteration( iterationNumber, lastIteration );
	nrOfProfiledTokens_ = 0;

	std::map< std::wstring, double > counter;

	csl::Stopwatch stopwatch;

	histCounter_.clear();
	ocrCounter_.clear();

	for( Document_t::iterator token = document_.begin(); // for all tokens
	     token != document_.end();
	     ++token ) {
                //std::wcout << "TOKEN: " << token->getWOCR() << std::endl;
	    if( ( config_.pageRestriction_ != (size_t)-1 ) &&
                token->origin().getPageIndex()  >= config_.pageRestriction_ ) {
                break;
            }

	    // remove old correction candidates from Document::Token
	    // Don't confuse this with the Profiler_Interpretations!!
	    token->origin().removeCandidates();

	    /*
	     * Those two statements refer only to Profiler_Interpretations
	     */
	    candidates_.clear();
	    token->setCandidateSet( &candidates_ );

	    Evaluation_Token evalToken( *token );



	    //////////////// //////////////// //////////////// ////////////////

	    if( ! token->isNormal() ) {
                ++counter[L"notNormal"];
                token->setSuspicious( token->getAbbyySpecifics().isSuspicious() );
                htmlWriter_.registerToken( *token, evalToken, candidates_ );
	    }
	    else if( token->isShort() ) {
                ++counter[L"short"];
                token->setSuspicious( token->getAbbyySpecifics().isSuspicious() );
                htmlWriter_.registerToken( *token, evalToken, candidates_ );
	    }
	    else if( token->isDontTouch() ) {
                ++counter[L"dont_touch"];
                token->setSuspicious( token->getAbbyySpecifics().isSuspicious() );
                htmlWriter_.registerToken( *token, evalToken, candidates_ );
                evalToken.registerNoCandidates(); // this class of tokens is included in evaluation
	    }
	    else { // normal
                ++counter[L"normalAndLongTokens"];
                token->setTokenNr( static_cast< size_t >( counter[L"normalAndLongTokens"] ) );
                if( (int)counter[L"normalAndLongTokens"] % 1000 == 0 ) {
                        std::wcerr << counter[L"normalAndLongTokens"] / 1000
                                   << "k/" << document_.size() / 1000
                                   << "k tokens processed in "
                                   << stopwatch.readMilliseconds() << "ms" << std::endl;
                        stopwatch.start();
                }
		calculateCandidateSet(*token, tempCands);
		// tempCands.reset();
		// //std::wcout << "Profiler:: process Token " << token->getWOCR_lc() << std::endl;
		// dictSearch_.query( token->getWOCR_lc(), &tempCands );


		// std::sort( tempCands.begin(), tempCands.end() );


		// compute ocrTraces, values like the candProbabilities and the sum of cand-scores
		double sumOfProbabilities = 0;
		for( csl::DictSearch::CandidateSet::const_iterator cand = tempCands.begin(); cand != tempCands.end(); ++cand ) {
		    std::vector< csl::Instruction > ocrInstructions;

		    // throw away "short" candidates for "long" words
		    if( cand->getWord().length() < 4 ) {
			continue;
		    }
		    // throw away candidates containing a hyphen
		    // Yes, there are such words in staticlex :-/
		    if( cand->getWord().find( '-') != std::wstring::npos ) {
			continue;
		    }


		    //std::wcerr << "instructionComputer_.computeInstruction( " << cand->getWord() << ", " <<token->getWOCR_lc() <<", "<<&ocrInstructions<<" )"<<std::endl; // DEBUG
		    instructionComputer_.computeInstruction( cand->getWord(), token->getWOCR_lc(), &ocrInstructions );
		    //std::wcout << "BLA: Finished" << std::endl;


		    // std::wcerr << cand->toString() << std::endl;
		    //std::wcerr<<"instructionComputer_.computeInstruction( " << cand->getWord() << ", " <<token->getWOCR_lc() <<", "<<&ocrInstructions<<" )"<<std::endl; // DEBUG
		    // ocrInstructions is empty, if the ocr errors can be explained by std. lev. distance but not
		    // by the distance defined with the patternWeights object. In that case, drop the candidate.
		    if( ocrInstructions.empty() ) {
			    std::wcerr << "NO OCR INSTRUCTIONS\n";
			continue;
		    }

		    struct {
			bool operator() ( Profiler_Interpretation const& a,
					  Profiler_Interpretation const&  b ) const {
			    return ( (a.getHistTrace() == b.getHistTrace() ) &&
				     (a.getOCRTrace() == b.getOCRTrace() )

				);
			}
		    } myEquals;

		    // If the OCR instruction is ambiguous, the interp. is cloned for each possible instruction
		    // It's not exactly clear if this is the right thing to do!!
		    for( std::vector< csl::Instruction >::const_iterator instruction = ocrInstructions.begin();
			 instruction != ocrInstructions.end();
			 ++instruction ) {
			// std::wcerr<<"Instr: "<<*instruction << std::endl; // DEBUG
			//     std::wcerr << "(POFILER) " << token->getWOCR_lc() << ","
			// 	       << cand->getWord() << "," << cand->getLevDistance()
			// 	       << "+ocr" << *instruction << "\n";

			if( instruction->size() > cand->getLevDistance() ) {
				// std::wcerr << "instruction->size(): "
				// 	   << instruction->size() << "\n"
				// 	   << "cand->getLevDistance(): "
				// 	   << cand->getLevDistance() << "\n"
				// 	   << "instructions:";
				// for (const auto& i: *instruction) {
				// 	std::wcerr << " " << i;
				// }
				// std::wcerr << "\nFOO\n";
				continue;
			}

			Profiler_Interpretation current = Profiler_Interpretation( *cand );
			current.setOCRTrace( *instruction );

			current.setCombinedProbability( getCombinedProb( current ) );

			//     std::wcerr << "(Profiler) token: "
			// 	       << token->getWOCR() << "\n";
			//     std::wcerr << "(Profiler) current: " << current << "\n";
			//     std::wcerr << "(Profiler) current.getOCRTrace(): "
			// 	       << current.getOCRTrace() << "\n";

			// Here, candidates with equal hist- and OCR-Trace are avoided!
			// See the compare-operator above
			if( candidates_.empty() || ( ! myEquals( current, candidates_.back() ) ) ) {
			    //if( ( iterationNumber < 2 ) || current.getCombinedProbability() > 1e-8 ) { // EXPERIMENTAL
				sumOfProbabilities += current.getCombinedProbability();
				candidates_.push_back( current );
				//}
			}
			else {
// 			    std::wcout << "Remove duplicate:" << std::endl
// 				       << candidates_.back() << std::endl
// 				       << current << std::endl;
			}
		    }

		    // modern wins!
		    // if(
		    // 	cand->getInstruction().empty() && ( cand->getLevDistance() == 0 ) ) {
		    // 	break;
		    // }
		}

		token->setProbNormalizationFactor( (double)1 / (double)sumOfProbabilities );

		// this is an ugly thing: Evaluation_Token holds a COPY of the Profiler_Token
		//evalToken.tok_.setProbNormalizationFactor( (double)1 / (double)sumOfProbabilities );

		token->setWOCRFreq( wOCRFreqlist_[token->getWOCR()] );


		// This is the actual run through the interpretations where the profiling is done
		for( std::vector< Profiler_Interpretation >::iterator cand = candidates_.begin(); cand != candidates_.end(); ++cand ) {

		    // voteWeight is normalized by the sum of all candProbabilities so all weights
		    // will add up to 1
		    cand->setVoteWeight( cand->getCombinedProbability() / sumOfProbabilities );

		    // check if voteWeight is NaN (not a number)
		    if( cand->getVoteWeight() != cand->getVoteWeight() ) {
			std::wcerr << "NAN=" << cand->getVoteWeight()  << "," << token->getWOCR_lc() << "," << cand->toString() <<  std::endl;
			continue;
		    }



		    for( csl::Trace::const_iterator pat = cand->getHistInstruction().begin();
			 pat != cand->getInstruction().end();
			 ++pat ) {

			// std::wcout << "histCounter_[" << static_cast<csl::Pattern>(*pat).toString() <<"] += "<< token->voteWeight << std::endl;
			histCounter_.registerPattern( *pat, cand->getVoteWeight() );
		    }

		    for( csl::Trace::const_iterator pat = cand->getOCRTrace().begin();
			 pat != cand->getOCRTrace().end();
			 ++pat ) {

			//std::wcerr << "ocrCounter_[" << pat->toString() <<"] += "<< token->voteWeight << std::endl;
// 			    if( pat->getLeft().find_first_of( '-' ) != std::wstring::npos ) {
// 				std::wcerr << cand->getWord() << "," << token->wOCR_lc << std::endl;
// 			    }

			ocrCounter_.registerPattern( *pat, cand->getVoteWeight() );

			if( cand->getVoteWeight() > 0.5 ) {
			    ocrPatternsInWords[*pat].insert( token->getWOCR_lc() );
			}
		    }

		    if( cand->getHistInstruction().size() > 0 ) {
			counter[L"guessed_histTrace_nonempty"] += cand->getVoteWeight();
		    }
		    if( cand->getOCRTrace().size() > 0 ) {
			counter[L"guessed_ocrTrace_nonempty"] += cand->getVoteWeight();
		    }

		    globalProfile_.dictDistribution_[cand->getDictModule().getName()].frequency += cand->getVoteWeight();



		    ocrCounter_.registerNGrams( cand->getWord(), cand->getVoteWeight() );
		    histCounter_.registerNGrams( cand->getBaseWord(), cand->getVoteWeight() );

		    baseWordFrequency[cand->getBaseWord()] += cand->getVoteWeight();

		    evalToken.registerCandidate( *cand );
		    counter[L"wasProfiled"] += cand->getVoteWeight();

		} // for all interpretations



		if( candidates_.empty() ) {
		    evalToken.registerNoCandidates();
		}

		// a custom-made sort-operator is passed as 3rd argument
		std::sort( candidates_.begin(), candidates_.end(), CandSort() );


		////// ERROR DETECTION //////////////
		if( candidates_.empty() ) {
		    if( token->isDontTouch() ) {
			// for "DontTouch" words, don't change Abbyy's judgment
			token->setSuspicious( token->getAbbyySpecifics().isSuspicious() );
		    }
		    else {
			// This applies to words which were actually profiled but don't have any interpretations
			// Treat those words as suspicious
			token->setSuspicious( true );
		    }
		}
		else if( candidates_.at( 0 ).getOCRTrace().size() > 0 ) {
		    Profiler_Interpretation const& topCand = candidates_.at( 0 );
		    token->setSuspicious( true );
		    for( csl::Instruction::const_iterator pat = topCand.getOCRTrace().begin();
			 pat != topCand.getOCRTrace().end();
			 ++pat ) {
			ocrPatterns2Types_[ *pat ].push_back( std::make_pair( token->getWOCR_lc(), topCand.getWord() ) );
		    }
		}
		else {
		    token->setSuspicious( false );
		}
		////////////////////////////////

		////// MODIFY ORIGINAL TOKENS FOR CORRECTION SYSTEM //////////////
		if( lastIteration ) {
		    token->origin().setSuspicious( token->isSuspicious() );


		    std::set< std::wstring > seen;
		    for( std::vector< Profiler_Interpretation >::iterator cand = candidates_.begin();
			 cand != candidates_.end();
			 ++cand ) {

			// check if voteWeight is NaN (not a number)
			if( cand->getVoteWeight() != cand->getVoteWeight() ) {
			    continue;
			}

			// if( !seen.empty() && cand->getVoteWeight() < 1e-9 ) {
			//     break;
			// }

			// candidate string not suggested before
			if( ( seen.insert( cand->getWord() ).second == true ) ) {
			    token->origin().addCandidate( *cand );
			}
		    }
		}

		////////////////////////////////


		htmlWriter_.registerToken( *token, evalToken, candidates_ );

		if( candidates_.empty() ) {
		    ++counter[L"unknown"];
		}
		else { // there are no interpretations

		}
	    } // normal

	    evaluation.registerToken( evalToken );

	    token->setCandidateSet( 0 ); // This makes clear that the pointer can't be used any more!!

	} // for each token
	///////////////////////////////////     END:  FOR EACH TOKEN //////////////////////////////////


	// compute probabilities for hist. variant patterns
	if( config_.resetHistPatternProbabilities_ ) globalProfile_.histPatternProbabilities_.clear();
	for( PatternCounter::PatternIterator it = histCounter_.patternsBegin(); it != histCounter_.patternsEnd(); ++it ) {

	    // note that config_.patternCutoff_hist_ is a threshold "per 1.000 profiled tokens"
	    if(  ( it->second )  > ( config_.patternCutoff_hist_ * ( counter[L"wasProfiled"] / 1000 ) ) ) {

		csl::Pattern strippedPattern = it->first;
		strippedPattern.strip(); // remove wordBegin/ wordEnd markers

		globalProfile_.histPatternProbabilities_.setWeight(
		    it->first, //pattern
		    ( it->second / histCounter_.getNGramCount( strippedPattern.getLeft() ) ),  // its count / the ( stripped) left-hand-side's nGramCount
		    it->second           // absolute freq
		    );
		// std::wcerr << "globalProfile_.histPatternProbabilities_.setWeight( " << it->first.toString()
		// 	   <<", ( " << it->second << " / " << histCounter_.getNGramCount( strippedPattern.getLeft() ) << " ) )"
		// 	   << "=" << ( it->second / histCounter_.getNGramCount( strippedPattern.getLeft() ) ) << std::endl;
	    }
	    else {
	    }
	}

	// compute probabilities for ocr patterns
	if( config_.resetOCRPatternProbabilities_ ) globalProfile_.ocrPatternProbabilities_.clearExplicitWeights();

	for( PatternCounter::PatternIterator it = ocrCounter_.patternsBegin(); it != ocrCounter_.patternsEnd(); ++it ) {

	    // hack: applies to ins and del
	    if( it->first.getLeft().size() + it->first.getRight().size() == 1 ) {
	    	continue;
	    }

	    // applies to insertions
	    // if( it->first.getLeft().size() == 0 && it->first.getRight().size() == 1 ) {
	    // 	continue;
	    // }


	    // Note that config_.patternCutoff_ocr_ is a threshold "per 1.000 profiled tokens"
	    if( ( it->second  > ( config_.patternCutoff_ocr_ * ( counter[L"wasProfiled"] / 1000 ) ) ) ) {


		//
		// if( iterationNumber >= 3 ) {
		//     if( ocrPatternsInWords[it->first].size() < 2 ) {
		// 	std::wcout << it->first.toString() << "(" << it->second << ") only in ";
		// 	for( std::set< std::wstring >::const_iterator wit = ocrPatternsInWords[it->first].begin(); wit != ocrPatternsInWords[it->first].end(); ++wit ) {
		// 	    std::wcout << *wit << ",";
		// 	}
		// 	std::wcout << ", continue" << std::endl;
		// 	continue;
		//     }
		// }


		// the denominator for the relative freq (== probability) of the pattern
		double denominator = 0;
		if( it->first.getLeft().empty() ) { // insertion pattern
		    denominator = ocrCharacterCount_;
		}
		else {       // all other patterns
		    denominator = ocrCounter_.getNGramCount( it->first.getLeft() );
		}

		// add pattern probability to global profile except it is very very small
		if( ( it->second / denominator ) > 1e-25 ) {
		    globalProfile_.ocrPatternProbabilities_.setWeight(
			it->first,            // pattern
			( it->second / denominator ), // its count / the left-hand-side's nGramCount
			it->second );        // absolute freq

		//     std::wcout << "globalProfile_.ocrPatternProbabilities_.setWeight( " << it->first.toString()
		// 	       <<", ( " << it->second << " / " << ocrCounter_.getNGramCount( it->first.getLeft() ) << " ) )"
		// 	       << "=" << ( it->second / ocrCounter_.getNGramCount( it->first.getLeft() ) ) << std::endl;
		}
	    }
	}

	// create MinDic for baseWord frequencies;
	// The next few lines are a not very elegant way to clear the MinDic
	if( baseWordFrequencyDic_ ) {
	    delete baseWordFrequencyDic_;
	}
	baseWordFrequencyDic_ = new csl::MinDic< float >();

	baseWordFrequencyDic_->initConstruction();
	for( std::map< std::wstring, float >::const_iterator it = baseWordFrequency.begin();
	     it != baseWordFrequency.end();
	     ++it ) {
	    baseWordFrequencyDic_->addToken( it->first.c_str(), it->second );
	}
	baseWordFrequencyDic_->finishConstruction();
	//baseWordFrequencyDic_->writeToFile( "basewords.dynamic.mdic" );

	nrOfProfiledTokens_ = static_cast< size_t >( counter[L"wasProfiled"] );

	evaluation.finish();


	// update the proportions of the DictDistributions
	for( std::map< std::wstring, GlobalProfile::DictDistributionPair >::iterator it = globalProfile_.dictDistribution_.begin();
	     it!= globalProfile_.dictDistribution_.end();
	     ++ it ) {

	    it->second.proportion = it->second.frequency / counter[ L"wasProfiled" ];
	}


	htmlWriter_.registerStatistics( counter, globalProfile_, evaluation );


	////////////////////////////////////////////////
	//////// HTML OUTPUT ///////////////////////////
	////////////////////////////////////////////////
	if( lastIteration ) {
	    htmlWriter_.bottomStream() << "<hr><pre>" << std::endl;
	    time_t t = time(NULL);
	    htmlWriter_.bottomStream() << "created at:" << asctime(localtime(&t)) << "\n<p>\n" << std::endl;
	    config_.print( htmlWriter_.bottomStream() );
	    htmlWriter_.bottomStream() << "</pre><hr>" << std::endl;



	    if( htmlStream_ ) htmlWriter_.print( *htmlStream_ );

	} //if lastIteration

	std::wcerr << "Finished iteration in " << iterationTime.readSeconds() << " seconds." <<  std::endl;

    } // void doIteration


    double Profiler::getCombinedProb( Profiler_Interpretation& cand ) const {
	// lang Probability
	double langProb = freqList_.getLanguageProbability( cand );
	// give each pattern at least a small probability
	if (langProb == 0)
		langProb = config_.ocrPatternStartProb_;
	// std::wcerr << "LANGPROP: " << langProb << "\n";

	cand.setLangProbability( langProb );

	// ocr Probability
	double ocrProb = 1;
	double w = 0;

	for( csl::Instruction::const_iterator posPat = cand.getOCRTrace().begin();
	     posPat != cand.getOCRTrace().end();
	     ++posPat ) {

	    w = globalProfile_.ocrPatternProbabilities_.getWeight( *posPat );
//		std::wcerr << "weight for " << ((csl::Pattern)*posPat).toString() << " is " << w << std::endl;
	    ocrProb *= ( w != csl::PatternWeights::UNDEF )? w : 0;
	    }
	cand.setChannelProbability( ocrProb );

	double combinedProb = ocrProb * langProb;

// 	    cand.print( std::wcerr ); std::wcerr << std::endl;
// 	    std::wcerr << "freqList_.getInterpretationFrequency()=" << freqList_.getInterpretationFrequency( cand ) << std::endl;
// 	    std::wcerr << "langProb=" << langProb << std::endl;
// 	    std::wcerr << "ocrProb=" << ocrProb << std::endl;
// 	    std::wcerr << "score=" << combinedProb << std::endl;


	return combinedProb;
    }


    void Profiler::profile2xml( char const* filename ) const {
	std::wofstream fo( filename );
	if( ! fo.good() ) throw OCRCException( std::string( "OCRC::Profiler::profile2xml: Can not open file for writing: " ) + filename );
//	fo.imbue( csl::CSLLocale::Instance() );
	profile2xml( fo );
    }

    void Profiler::profile2xml( std::wostream& os ) const {
	if( ! os.good() ) throw OCRCException( std::string( "OCRC::Profiler::profile2xml: Bad filehandle" ) );

	time_t t = time(NULL);
	std::string timeString( asctime(localtime(&t)) );
	timeString.resize( timeString.size() - 1 ); // remove newline
	std::wstring wide_timeString = Utils::utf8(timeString);
	//csl::CSLLocale::string2wstring( timeString, wide_timeString );

	os.imbue(std::locale(std::locale(), new NoThousandGrouping()));
	os << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl
	   << "<profile>" << std::endl
	   << "<head>" << std::endl
	   << "<source_file>" << "</source_file>" << std::endl
	   << "<created_at>" << wide_timeString << "</created_at>" << std::endl
	   << "</head>" << std::endl;

	os << "<dictionary_distribution>" << std::endl;
	for( std::map< std::wstring, GlobalProfile::DictDistributionPair >::const_iterator it = globalProfile_.dictDistribution_.begin();
	     it!= globalProfile_.dictDistribution_.end();
	     ++ it ) {

	    os << "<item dict=\"" << it->first << "\" frequency=\"" << it->second.frequency<< "\" proportion=\"" << it->second.proportion << "\"/>" << std::endl;

	}
	os << "</dictionary_distribution>" << std::endl;


	os << "<spelling_variants>" << std::endl;
	globalProfile_.histPatternProbabilities_.writeToXML_patternsOnly( os );
	os << "</spelling_variants>" << std::endl;
	os << "<ocr_errors>" << std::endl;


	std::vector< std::pair< csl::Pattern, double > > patternsSorted;
	globalProfile_.ocrPatternProbabilities_.sortToVector( &patternsSorted );
	for( std::vector< std::pair< csl::Pattern, double > >::const_iterator pat_freq = patternsSorted.begin();
	     pat_freq != patternsSorted.end();
	     ++pat_freq ) {
	    os << "<pattern left=\"" << pat_freq->first.getLeft() << "\" right=\"" << pat_freq->first.getRight() << "\""
	       << " pat_string=\"" << pat_freq->first.getLeft() << "_" << pat_freq->first.getRight() << "\""
	       << " relFreq=\"" << globalProfile_.ocrPatternProbabilities_.getWeight( pat_freq->first )  << "\""
	       << " absFreq=\"" << ( globalProfile_.ocrPatternProbabilities_.getAbsoluteFreqs().find( pat_freq->first ) )->second << "\""
	       << ">"
	       << std::endl;

	    os << "<pattern_occurrences>" << std::endl;

	    std::map< csl::Pattern, std::vector< std::pair< std::wstring, std::wstring > > >::const_iterator patOcc_it =
		ocrPatterns2Types_.find( pat_freq->first );

	    if( patOcc_it != ocrPatterns2Types_.end() ) {
		std::vector< std::pair< std::wstring, std::wstring > > const& patternOccurrences = patOcc_it->second;

		for( std::vector< std::pair< std::wstring, std::wstring > >::const_iterator word = patternOccurrences.begin();
		     word != patternOccurrences.end();
		     ++word ) {
		    // std::wstring wSuggest = word->second; // we need a copy here because we maybe want to capitalize it
		    // if( isupper( word->first.at( 0 ), loc ) ) {
		    // 	wSuggest = toupper( word->second.at( 0 ), loc );
		    // }
		    os << "<type wOCR_lc=\"" << word->first
		       << "\" wSuggest=\"" << word->second
		       <<  "\" freq=\"" << Utils::queryConstMap<std::wstring, size_t>(wOCRFreqlist_, word->first, 0)
		       << "\"/>" << std::endl;
		}
	    }
	    os << "</pattern_occurrences>" << std::endl;
	    os << "</pattern>" << std::endl;
	}
	os << "</ocr_errors>" << std::endl;
	os << "</profile>" << std::endl;
    }




    void Profiler::prepareDocument( Document& tmpDoc ) {

	tmpDoc.findLineBorders();
	tmpDoc.findHyphenation();

	for( Document::iterator tmpToken = tmpDoc.begin();
	     tmpToken != tmpDoc.end();
	     ++tmpToken ) {
	    document_.push_back( Profiler_Token( *tmpToken ) );

	    Profiler_Token& token = document_.back();

	    token.setWOCR( tmpToken->getWOCR() );
	    token.setWOCR_lc( tmpToken->getWOCR_lc() );
	    token.setWCorr(tmpToken->getWCorr().data());
	    token.getAbbyySpecifics().setSuspicious( tmpToken->getAbbyySpecifics().isSuspicious() );


	    if( config_.donttouch_lineborders_  ) {
 		// DONTTOUCH the token if it is at the beginning or endog a line or was marked DONTTOUCH otherwise.
		token.setDontTouch( tmpToken->isDontTouch() ||
				    tmpToken->hasProperty( Token::LINE_BEGIN ) ||
				    tmpToken->hasProperty( Token::LINE_END )
		    );
	    }

	    if( config_.donttouch_hyphenation_  ) {
		// DONTTOUCH the token if it is part of a hyphenation or was marked DONTTOUCH otherwise.
		token.setDontTouch( tmpToken->isDontTouch() ||
				    tmpToken->hasProperty( Token::HYPHENATION_1ST ) ||
				    tmpToken->hasProperty( Token::HYPHENATION_2ND )
		    );
	    }


	    if( tmpToken->hasGroundtruth() ) {
		token.getGroundtruth() = tmpToken->getGroundtruth();
	    }
	}

        std::wcerr << "Create wOCR frequency list ... " << std::flush;
	ocrCharacterCount_ = 0;
	for( Document_t::iterator token = document_.begin(); // for all tokens
	     token != document_.end();
	     ++token ) {

 	    bool firstToken = ( token == document_.begin() );
 	    bool lastToken = ( token+1 == document_.end() );

	    ++( wOCRFreqlist_[token->getWOCR_lc()] );

	    if( token->isNormal() ) {
		ocrCharacterCount_ += token->getWOCR().length();
	    }


	}
	std::wcerr << "ok" << std::endl;

    }


    void Profiler::printConfigTemplate( std::wostream& os ) {
	os << "######## Profiler Configuration ########################" << std::endl
	   << "#                                                      #" << std::endl
	   << "########################################################" << std::endl
	   << "" << std::endl
	   << "" << std::endl
	   << "" << std::endl
	   << "########################################################" << std::endl
	   << "########### Probability settings #######################" << std::endl
	   << "########################################################" << std::endl
	   << "" << std::endl
	   << "# the class OCRCorrection::Profiler::Configuration owns" << std::endl
	   << "# member variables corresponding to all those values. Please" << std::endl
	   << "# consult this class' reference." << std::endl
	   << "# If you do not know exactly what you're doing, it might be" << std::endl
	   << "# best not to change these values at all." << std::endl
	   << "" << std::endl
       << "[global]" << std::endl
	   << "numberOfIterations = 10" << std::endl
	   << "" << std::endl
	   << "patternCutoff_hist=0,4" << std::endl
	   << "patternCutoff_ocr=0,05" << std::endl
	   << "" << std::endl
	   << "histPatternSmoothingProb = 1e-5" << std::endl
	   << "ocrPatternSmoothingProb  = 1e-5" << std::endl
	   << "" << std::endl
	   << "resetHistPatternProbabilities = 0" << std::endl
	   << "resetOCRPatternProbabilities  = 0" << std::endl
	   << "" << std::endl
	   << "" << std::endl
	   << "" << std::endl
	   << "####################################################################" << std::endl
	   << "########### Language model/ Pattern settings #######################" << std::endl
	   << "####################################################################" << std::endl
	   << "" << std::endl
	   << "[language_model]" << std::endl
	   << "# If you are about t ochange the pattern set, NOTE THAT the language model" << std::endl
	   << "# was trained using a particular pattern set. So be sure to use the correct" << std::endl
	   << "# pattern set here." << std::endl
	   << "#" << std::endl
	   << "# The file must be utf-8 encoded and must contain 1 pattern per line, with <SPACE> as separator." << std::endl
	   << "# Example:" << std::endl
	   << "# t th" << std::endl
	   << "# ei ey" << std::endl
	   << "" << std::endl
	   << "patternFile = \"/path-to/patterns.txt\"" << std::endl
	   << "patternWeightsFile = \"/path-to/patternWeights.txt\"  # This file is generated by the tool trainFrequencyList" << std::endl
	   << "freqListFile = \"/path-to/freqlist.binfrq\"           # This file is generated by the tool trainFrequencyList" << std::endl
	   << "" << std::endl
	   << "" << std::endl
	   << "####################################################################" << std::endl
	   << "########### Dictionary settings ####################################" << std::endl
	   << "####################################################################" << std::endl
	   << "# IF YOU ADD NEW DICTIONARIES, don't forget to update the list 'activeDictionaries'" << std::endl
	   << "# This list is space-separated" << std::endl
	   << "[dictionaries]" << std::endl
	   << "activeDictionaries = \"modern names\"" << std::endl
	   << "" << std::endl
	   << "" << std::endl
	   << "# [name_of_lexicon]" << std::endl
	   << "# path = \"/path-to/some_lexicon.fbdic\"" << std::endl
	   << "# histPatterns = n            # allow n variant pattern applications in one word" << std::endl
	   << "# ocrErrors = n               # allow approximate search with levenshtein distance n" << std::endl
	   << "# ocrErrorsOnHypothetic = n   # for tokens with variant pattern applications, allow levenshtein distance n" << std::endl
	   << "# priority = 100              # For the sorting order of matches, this value decides in case of doubt." << std::endl
	   << "" << std::endl
	   << "[modern]" << std::endl
	   << "path = \"/path-to/moderndic.fbdic\"" << std::endl
	   << "histPatterns = 2" << std::endl
	   << "ocrErrors = 2" << std::endl
	   << "ocrErrorsOnHypothetic = 1" << std::endl
	   << "priority = 100" << std::endl
	   << "cascadeRank = 0" << std::endl
	   << "" << std::endl
	   << "[names] # This is just another example for a 2nd lexicon" << std::endl
	   << "path = \"/path-to/a_lexicon-of-names.fbdic\"" << std::endl
	   << "histPatterns = 1000" << std::endl
	   << "ocrErrors = 0" << std::endl
	   << "ocrErrorsOnHypothetic = 0" << std::endl
	   << "priority = 90" << std::endl
	   << "cascadeRank = 0" << std::endl
	    ;


    }


}
#endif
