/**
 * DO NOT INCLUDE THIS HEADER ANYWHERE - INCLUDE FrequencyList.h INSTEAD !!!!!!
 */

#include "../Utils/Utils.h"

namespace OCRCorrection {

    class FrequencyList::Trainer {
    public:
	typedef Document Document_t;

	/**
	 * This is the constructor that should usually be used to train a pattern-aware model for historical language.
	 *
	 * @param[out] myFreqlist  Here the results of the training will be stored when doTraining() is called.
	 * @param[in]  modernDict
	 * @param[in]  patternFile
	 */
	Trainer( FrequencyList& myFreqlist, char const* dictSearchConfig ) :
	  myFreqlist_( myFreqlist ),
	  frequencyThreshold_( 0 ),
	  allModern_( false ),
	  nrOfTrainingTokens_( 0 ) {

	    dictSearch_.readConfiguration( dictSearchConfig );


	    if( myFreqlist_.interpFrequency_ || myFreqlist_.baseWordFrequency_ || myFreqlist_.patternWeights_  ) {
		throw std::runtime_error( "OCRC::FrequencyList::Trainer: FrequencyList-object must not be initialised with other data." );
	    }
	}

	/**
	 * This constructor can be used for a very simple sort of frequency list, e.g. for modern language. All
	 * words of the given corpus/freqlist are taken as modern words.
	 */
	Trainer( FrequencyList& myFreqlist ) :
	    myFreqlist_( myFreqlist ),
	    frequencyThreshold_( 0 ),
	    allModern_( true ), // !!!
	    nrOfTrainingTokens_( 0 ) {

	    if( myFreqlist_.interpFrequency_ || myFreqlist_.baseWordFrequency_ || myFreqlist_.patternWeights_  ) {
		throw std::runtime_error( "OCRC::FrequencyList::Trainer: FrequencyList-object must not be initialised with other data." );
	    }
	}


	/**
	 * @brief update the counters according to the info that the given interpretation
	 *        was seen with the given frequency in the training corpus
	 */
	void registerInterpretation( csl::Interpretation const& interp, float frequency = 1 ) {


	  nrOfTrainingTokens_ += frequency;
	  if( nrOfTrainingTokens_ % 10000 == 0 ) {
	    //std::wcerr << nrOfTrainingTokens_ << " normal tokens processed in " << stopwatch.readMilliseconds() << " ms" <<  std::endl;
	    //stopwatch.start();
	  }

	  interpretations_[ interp ] += frequency;
	  baseWords_[ interp.getBaseWord() ] += frequency;
	  registerCorrect( interp.getBaseWord(), frequency );

	  // register all patterns
	  for( csl::Instruction::const_iterator pat = interp.getInstruction().begin();
	       pat != interp.getInstruction().end();
	       ++pat ) {
	      patterns_[*pat] += frequency;
	  }
	} // registerInterpretation()

	/**
	 * @brief Uses type-frequency-lists of the form '<WORD>\t<FREQ>\n' for training.
	 *
	 */
	void doTrainingOnFreqlist( char const* freqlistFile ) {
	    std::wifstream freqlist_in;
	    freqlist_in.open( freqlistFile );

	    if( ! freqlist_in.good() ) {
		throw OCRCException( std::string( "OCRC::FrequencyList_Trainer::doTrainingOnFreqlist: Could not open freqlist: " ) + freqlistFile );
	    }

	    //freqlist_in.imbue( csl::CSLLocale::Instance() );

	  std::wstring line;
	  csl::DictSearch::CandidateSet cands;

	  size_t lineCount = 0;
	  size_t skippedCount = 0;
	  while( getline( freqlist_in, line ).good() ) { // For this to work completely, also the last line in the file must be terminated by '\n'
	      if( ++lineCount % 10000 == 0 )
		  std::wcerr << "OCRC::FrequencyList_Trainer::doTrainingOnFreqlist: "
			     << lineCount/1000 << "k lines processed. "
			     << int( skippedCount /1000 ) << "k skipped." << std::endl;

	    //std::wcerr << line << std::endl;

	    size_t delimPos = line.find( '\t' );
	    if( delimPos == std::wstring::npos  ) {
		throw OCRCException( "OCRC::FrequencyList_Trainer::doTrainingOnFreqlist: Invalid line in input file" );
	    }
	    std::wstring word = line.substr( 0, delimPos );
            size_t freq = Utils::toNum<size_t>(word);
	    //size_t freq = csl::CSLLocale::string2number< size_t >( line.substr( delimPos + 1 ) );

	    if( word.length() < 4 ) {
		++skippedCount;
		continue;
	    }
	    if( freq < frequencyThreshold_ ) {
		++skippedCount;
		continue;
	    }

	    std::wstring lowercased = word;
	    for( std::wstring::iterator c = lowercased.begin(); c != lowercased.end(); ++c ) {
                    *c = towlower(*c);
	    }

	    // std::wcout << lowercased << std::endl; // DEBUG

	    if( allModern_ ) {
		// create an interpretation with empty instruction
		csl::Interpretation interp;
		interp.setWord( lowercased );
		interp.setBaseWord( lowercased );
		registerInterpretation( interp, freq );
	    }
	    else { // standard behaviour using DictSearch
		cands.clear();
		//	    Stopwatch searchWatch;
		dictSearch_.query( lowercased, &cands );
		//std::wcerr << "DictSearch:" << searchWatch.readMilliseconds() << std::endl;

		// use custom CandSort - who knows how the built-in sort-operator will change in the future
		std::sort( cands.begin(), cands.end(), CandSort() );

		  if( cands.size() > 0 ) {
		      //std::wcout << lowercased << "(" << freq << ")-->" << cands.at(0).toString() << std::endl;
		      registerInterpretation( cands.at( 0 ), freq );

		      // if not present in a background lexicon, add word to corpus lexicon
		      if( cands.at( 0 ).getHistInstruction().size() > 0 ) {
			  corpusLexicon_[ lowercased ] = L"";
			  for( csl::DictSearch::CandidateSet::const_iterator cand = cands.begin(); cand != cands.end(); ++cand ) {
			      corpusLexicon_[ lowercased ] += static_cast< csl::Interpretation >(*cand).toString();
			      corpusLexicon_[ lowercased ] += L"|";
			  }
			  // remove last '|' separator
			  if( ! corpusLexicon_[ lowercased ].empty() ) corpusLexicon_[ lowercased ].resize( corpusLexicon_[ lowercased ].size() -1 );
		      }
		  }
		  else {
		      // create an interpretation with empty instruction
		      csl::Interpretation interp;
		      interp.setWord( lowercased );
		      interp.setBaseWord( lowercased );
		      registerInterpretation( interp, freq );

		      corpusLexicon_[ lowercased ] = lowercased + L":" + lowercased + L"+[],dist=0";
		  }
	    }

	  }

	} // doTrainingOnFreqlist()



	void doTraining( Document_t const& document ) {
	    csl::DictSearch::CandidateSet cands;

            csl::Stopwatch stopwatch;

	    // iterate over all tokens of the given text
	    size_t tokenCount = 0;
	    for( Document_t::const_iterator token = document.begin(); token != document.end(); ++token ) {
	      if( ++tokenCount % 10000 == 0 ) {
		std::wcerr << tokenCount / 1000 << "k tokens processed" << std::endl;
	      }
		if( ! token->isNormal() ) {
		    continue;
		}


		std::wstring lowercased = token->getWOCR();
		for( std::wstring::iterator c = lowercased.begin(); c != lowercased.end(); ++c ) {
                        *c = towlower(*c);
		}

		// std::wcout << lowercased << std::endl; // DEBUG

		cands.clear();
		dictSearch_.query( lowercased, &cands );

		// use custom CandSort - who knows how the built-in sort-operator will change in the future
		//std::sort( cands.begin(), cands.end(), CandSort() );
                std::sort( cands.begin(), cands.end()); // use Interpretation::operator<(...)

		if( cands.size() > 0 ) {
		    registerInterpretation( cands.at( 0 ) );
		}
		else {
		    corpusLexicon_[ lowercased ] += 1;
		    // create an interpretation with empty instruction
		    csl::Interpretation interp;
		    interp.setBaseWord( lowercased );
		    registerInterpretation( interp );
		}
	    }
	}


	class CandSort {
	public:
	    bool operator () ( csl::Interpretation const& a, csl::Interpretation const& b ) {
		int diff = a.getInstruction().size() - b.getInstruction().size();

		if( diff != 0 ) {
		    return diff < 0;
		}
		return ( b.getBaseWordScore() < a.getBaseWordScore() );
	    }
	};


	/**
	 * @brief perform training on a text document passed as utf-8 file
	 *
	 * Note that you always need to finish the training phase using finishTraining()
	 */
	void doTraining( char const* txtFile ) {
	    Document_t document;
	    TXTReader reader;
	    reader.parse( txtFile, &document );
	    doTraining( document );
	}

	/**
	 * @brief Call this method to finish the training phase.
	 */
	void finishTraining() {
	    // store interpretation-frequencies in the FrequencyList's MinDic
 	    myFreqlist_.interpFrequency_ = new csl::MinDic< int >();
 	    myFreqlist_.interpFrequency_->initConstruction();

	    for( std::map< csl::Interpretation, size_t >::const_iterator it = interpretations_.begin();
		 it != interpretations_.end();
		 ++it ) {
		myFreqlist_.interpFrequency_->addToken( myFreqlist_.interpretation2String( it->first ).c_str(), (int) it->second );
		//std::wcerr <<  myFreqlist_.interpretation2String( it->first ) << "#" << it->second << std::endl;
	    }
 	    myFreqlist_.interpFrequency_->finishConstruction();




	    // store baseWord-frequencies in the FrequencyList's MinDic
 	    myFreqlist_.baseWordFrequency_ = new csl::MinDic< float >();
	    std::wcerr<< "new MinDic:" << myFreqlist_.baseWordFrequency_ << std::endl;
	    myFreqlist_.myOwnBaseWordFrequency_ = true; // this tells the Freqlist to delete the object at destruction time
 	    myFreqlist_.baseWordFrequency_->initConstruction();

	    for(std::map< std::wstring, size_t >::const_iterator it = baseWords_.begin();
		it != baseWords_.end();
		++it ) {
		myFreqlist_.baseWordFrequency_->addToken( it->first.c_str(), (int) it->second );
	    }
 	    myFreqlist_.baseWordFrequency_->finishConstruction();
	    myFreqlist_.nrOfTrainingTokens_ = nrOfTrainingTokens_;

	    // calculate relative frequencies (= probabilities) for patterns and store them
	    // in the FrequencyList's PatternWeights object
	    myFreqlist_.patternWeights_ = new csl::PatternWeights();
	    myFreqlist_.myOwnPatternWeights_ = true; // this tells the Freqlist to delete the object at destruction time
	    for(std::map< csl::Pattern, size_t >::const_iterator it = patterns_.begin(); it != patterns_.end(); ++it ) {

		csl::Pattern strippedPattern = it->first;
		strippedPattern.strip(); // remove wordBegin/ wordEnd markers

		//                                      the pattern     its freq count          the freq count of its (stripped) left side
		myFreqlist_.patternWeights_->setWeight( it->first, ( (float)it->second / (float)charNGrams_[ strippedPattern.getLeft() ] ) );
	    }

	    print(); // DEBUG
	}

	void writeCorpusLexicon( char const* corpusLexFile ) const {
	    std::wofstream os( corpusLexFile );
	    for( std::map< std::wstring, std::wstring >::const_iterator it = corpusLexicon_.begin();
		 it != corpusLexicon_.end();
		 ++it ) {
		os << it->first << L"#" << it->second << std::endl;
	    }
	    os.close();
	    std::wcout << "OCRC::FrequencyList_Trainer: wrote corpus lexicon." << std::endl;
	}


	void print( std::wostream& os = std::wcout ) const {

	    std::vector< std::pair< csl::Pattern, size_t > > histPatternCountSorted;
	    for(std::map< csl::Pattern, size_t >::const_iterator it = patterns_.begin(); it != patterns_.end(); ++it ) {
		histPatternCountSorted.push_back( *it );
		//outStream_ << it->first.getLeft() << '-' << it->first.getRight() << " : " << it->second << std::endl;
	    }
	    std::sort( histPatternCountSorted.begin(), histPatternCountSorted.end(), sortBySecond< std::pair< csl::Pattern, size_t > > );
	    for( std::vector< std::pair< csl::Pattern, size_t > >::const_iterator it = histPatternCountSorted.begin();
		 it != histPatternCountSorted.end();
		 ++it ) {
		os << it->first.getLeft() << '-' << it->first.getRight() << " : " << it->second << std::endl;
		if( it - histPatternCountSorted.begin() > 100 )break;
	    }


	    std::vector< std::pair< csl::Interpretation, size_t > > interpretationCountSorted;
	    for( std::map< csl::Interpretation, size_t >::const_iterator it = interpretations_.begin();
		 it != interpretations_.end();
		 ++it ) {
		interpretationCountSorted.push_back( *it );
		//outStream_ << it->first.getLeft() << '-' << it->first.getRight() << " : " << it->second << std::endl;
	    }
	    std::sort( interpretationCountSorted.begin(),
		       interpretationCountSorted.end(),
		       sortBySecond< std::pair< csl::Interpretation, size_t > > );
	    for( std::vector< std::pair< csl::Interpretation, size_t > >::const_iterator it = interpretationCountSorted.begin();
		 it != interpretationCountSorted.end();
		 ++it ) {
		std::wcout << myFreqlist_.interpretation2String( it->first ) << L"-" << it->second << std::endl;

		if( it - interpretationCountSorted.begin() > 100 )break;
	    }

	    std::wcout << "interpretations has " << interpretations_.size() << " entries." << std::endl;
	    std::wcout << "charNGrams has " << charNGrams_.size() << " entries." << std::endl;
	    std::wcout << "patterns has " << patterns_.size() << " entries." << std::endl;

	}

	/**
	 * @brief sorts the std::pairs according to their second value
	 */
	template< typename pair_t >
	static bool sortBySecond( pair_t const& a, pair_t const& b ) {
	    return ( a.second > b.second ); // reverse ordering
	}

	/**
	 * @brief compares to csl::Interpretation not the regular, built-in way, but according
	 *        to their string appearances.
	 */
	class Interp_less {
	public:
	    bool operator() ( csl::Interpretation const& a, csl::Interpretation const& b ) {
		return ( wcscmp( FrequencyList::interpretation2String( a ).c_str(),
				 FrequencyList::interpretation2String( b ).c_str() ) < 0 );
	    }
	};


	void setFrequencyThreshold( size_t t ) {
	    frequencyThreshold_ = t;
	}


    private: /* METHODS */


	/**
	 * This method is stolen from OCRC::LevenshteinWeights. Probably this class should
	 * be  used here, too.
	 *
	 * @param frequency If trained from a frequency list, this gives us the frequency of the correct word. Default is 1.
	 */
	void registerCorrect(const std::wstring& correct, size_t frequency = 1 ) {
	    std::wstring index;
	    for( size_t i = 0; i < correct.length(); i++ ) {
		index = correct.at( i );
		charNGrams_[index] += frequency;

		// also register char-n-grams
		for( size_t n = 2; n <= 5 ; ++n ) {
		    if( ( i + n - 1 ) >= correct.length() ) {
			break;
		    }
		    index += correct.at( i + n - 1 );
		    charNGrams_[index] += frequency;
		}
	    }
	}


    private: /* MEMBERS */
	/**
	 * @brief a reference to the FrequencyList that is to be trained
	 */
	FrequencyList& myFreqlist_;


	csl::DictSearch dictSearch_;

	/**
	 * For frequency list processing, only consider words with frequency larger than or equal to this value
	 */
	size_t frequencyThreshold_;

	/**
	 * This flag indicates that no DictSearch module was set up and all words should be treated simply as modern words.
	 */
	bool allModern_;

	std::map< csl::Interpretation, size_t, Interp_less > interpretations_;
	std::map< std::wstring, size_t > baseWords_;
	/**
	 * @brief contains all tokens which are not found in one of the background lexica, including all possible GT interpretations
	 */
	std::map< std::wstring, std::wstring > corpusLexicon_;
	std::map< std::wstring, size_t > charNGrams_;
	std::map< csl::Pattern, size_t > patterns_;
	size_t nrOfTrainingTokens_;

    }; // class Trainer

} // eon
