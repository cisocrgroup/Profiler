#ifndef CSL_VAAM_TCC
#define CSL_VAAM_TCC CSL_VAAM_TCC

namespace csl {

    template< typename MinDicType >
    Vaam< MinDicType >::Vaam( const MinDic_t& baseDic, const char* patternFile ) :
	baseDic_( &baseDic ),
	filterDic_( 0 ),
	levDEA_( 0 ), // Here the default levenshtein threshold is specified
	caseMode_( Global::asIs ),
	minNrOfPatterns_( 0 ),
	maxNrOfPatterns_( Vaam::INFINITE )
    {
	patternGraph_.loadPatterns( patternFile );
    }


    template< typename MinDicType >
    void Vaam< MinDicType >::setDistance( size_t d ) {
	levDEA_.setDistance( d );
    }

    template< typename MinDicType >
    void Vaam< MinDicType >::setMinNrOfPatterns( size_t n ) {
	minNrOfPatterns_ = n;
    }

    template< typename MinDicType >
    void Vaam< MinDicType >::setMaxNrOfPatterns( size_t n ) {
	maxNrOfPatterns_ = n;
    }

    template< typename MinDicType >
    bool Vaam< MinDicType >::query( const std::wstring& word, iInterpretationReceiver* interpretations ) const {
	query_ = word;
	interpretations_ = interpretations;
	foundAnswers_ = false;

	if( ! baseDic_ ) throw csl::exceptions::LogicalError( "csl::Vaam::query: No baseDic_ loaded." );
	if( ! baseDic_->readyToRead() ) throw csl::exceptions::LogicalError( "csl::Vaam::query: baseDic_ not ready to read." );

	wasUpperCase_ = false;
	if( ( caseMode_ != Global::asIs ) && std::iswupper( query_.at( 0 )/*, locale_*/ ) ) {
	    wasUpperCase_ = true;
	    query_.at( 0 ) = std::tolower( query_.at( 0 ), locale_ );
	}


	levDEA_.loadPattern( query_.c_str() );
	stack_.clear();
	// create a first StackItem
	stack_.push_back( StackItem( *this ) );
	// insert an initial position at the the levDEA's start state, with 0 applied patterns
	stack_.at( 0 ).push_back( Position( levDEA_.getRoot(), 0 ) );
       // set the patternPos to the graph's root, then walk the wordBegin symbol
	stack_.at( 0 ).patternPos_ = patternGraph_.getRoot();
	stack_.at( 0 ).patternPos_.walk( Global::wordBeginMarker );


	query_rec( 0 );

	return foundAnswers_;
    }

    template< typename MinDicType >
    void Vaam< MinDicType >::setFilterDic( iDictionary< int > const& filterDic ) {
	filterDic_ = &filterDic;
    }

    template< typename MinDicType >
    void Vaam< MinDicType >::setBaseDic( MinDic_t const& filterDic ) {
	baseDic_ = &filterDic;
    }


    template< typename MinDicType >
    void Vaam< MinDicType >::query_rec( size_t depth ) const {


	stack_.push_back( StackItem( *this ) );

	// this lets us use the []-operator for stack_ with a somewhat purer conscience
	// But the at()-operator turned out to be a real bottleneck here ...
	if( stack_.size() != depth + 2 ) {
	    throw exceptions::LogicalError( "csl::Vaam::query_rec: current stack seems out of sync with backtracking procedure" );
	}



	applyPatterns( depth );

	size_t nrOfPositions = stack_[depth].size(); //remember the nr of postitions before applying the wordEndMarker
	if( stack_[depth].dicPos_.isFinal() ) {
	    PatternGraph::State oldPatternPos = stack_[depth].patternPos_;
	    stack_[depth].patternPos_.walk( Global::wordEndMarker );
	    applyPatterns( depth );
	    stack_[depth].patternPos_ = oldPatternPos;
	}


	checkForMatches( depth );

	// erase positions that were added using the wordEndMarker
	stack_[depth].erase( stack_[depth].begin()+nrOfPositions, stack_[depth].end() );


	// for all outgoing transitions
	for( const wchar_t* c = stack_[depth].dicPos_.getSusoString();
	     *c;
	     ++c ) {
	    stack_.at( depth + 1 ).clear();

	    stack_[depth + 1].patternPos_ = stack_[depth].patternPos_.getTransTarget( *c );

	    // see which of the Positions can be moved with this *c
	    // see also if the levDEA reaches a final state
	    size_t positionIndex = 0;
	    for( typename StackItem::iterator position = stack_[depth].begin();
		 position != stack_[depth].end();
		 ++position, ++positionIndex ) {
		LevDEA::Pos newLevPos = levDEA_.walk( position->levPos_, *c );
		if( newLevPos.isValid() ) {
		    stack_[depth + 1].push_back( Position( newLevPos, position->getNrOfPatternsApplied(), std::make_pair( depth, positionIndex ) ) );
		}
	    }

	    // set lookAheadDepth_ for next stackItem:
	    if( ! stack_[depth + 1].empty() ) { // set to 0 if new positions exist
		stack_[depth + 1].lookAheadDepth_ = 0;
	    }
	    else { // increment if no new positions exist
		stack_[depth + 1].lookAheadDepth_ = stack_[depth].lookAheadDepth_ + 1;
	    }

	    if( stack_[depth + 1].lookAheadDepth_ <= stack_[depth + 1].patternPos_.getPrefixLength() ) {
		stack_[depth + 1].dicPos_ = stack_[depth].dicPos_.getTransTarget( *c );
		baseWord_.resize( depth + 1 );
		baseWord_.at( depth ) = *c;

		query_rec( depth + 1 );
	    }
	}

	stack_.pop_back();
    } // query_rec


    template< typename MinDicType >
    void Vaam< MinDicType >::applyPatterns( size_t depth ) const {
	// apply all patterns whose left sides end here
	if( stack_[depth].patternPos_.isFinal() ) {
	    // a copy is needed here, because the state must descend the errorlinks in order to find more matches (-->Aho-Corasick alg.)
	    PatternGraph::State patPos = stack_[depth].patternPos_;


	    assert( patPos.isFinal() );

	    // for all patterns fitting a suffix of the current path
	    for( PatternGraph::Replacements_t::const_iterator rightSide = patPos.getReplacements().begin();
		 rightSide != patPos.getReplacements().end();
		 ++rightSide ) {
		    // std::wcerr << "current: " << rightSide->first << "," << rightSide->second << "\n";


		// Here, take the "stripped" version of the pattern without wordBegin- or worEnd-markers
		size_t sizeOfLeftSide = patternGraph_.stripped_at( rightSide->second ).getLeft().length();

		//std::wcout << "Pat=" <<  patternGraph_.at( rightSide->second ).toString() << ",stripped=" <<patternGraph_.stripped_at( rightSide->second ).toString() << std::endl;

		// for all positions of the stackItem (tracked back leftside)
		size_t count = 0;
		size_t nrOfOldPositions = stack_.at( depth - sizeOfLeftSide ).size();
		for( typename StackItem::iterator position = stack_.at( depth - sizeOfLeftSide ).begin();
		     count < nrOfOldPositions; // don't check for end() here, because if the container is changed during the loop
		     ++position, ++count ) {

		    // check if maxNrOfPatterns_ is reached already
			// std::wcerr << "max: " << maxNrOfPatterns_ << "\n";
			// // // std::wcerr << "app: " << position->getNrOfPatternsApplied() << "\n";
		    if(( maxNrOfPatterns_ != Vaam::INFINITE ) && ( position->getNrOfPatternsApplied() == maxNrOfPatterns_ ) )
			continue;


		    LevDEA::Pos newLevPos = levDEA_.walkStr( position->levPos_, patternGraph_.stripped_at( rightSide->second ).getRight().c_str() );
		    if( newLevPos.isValid() )  {
			//                              1 more applied pattern than cur position       store current position as 'mother'-position
			Position newPosition( newLevPos, position->getNrOfPatternsApplied() + 1, std::make_pair( depth - sizeOfLeftSide, count ) );
			newPosition.addPosPattern( PosPattern( patternGraph_.at( rightSide->second ).getLeft(),
							       patternGraph_.at( rightSide->second ).getRight(),
							       depth - sizeOfLeftSide ) );
			// CAUTION! Here stack_[depth] might re-alloc and make

			stack_[depth].push_back( newPosition );
			stack_[depth].lookAheadDepth_ = 0;
		    }
		} // for all rightSides
	    } // for all positions

	} // if found left pattern side
    }

    template< typename MinDicType >
    void Vaam< MinDicType >::checkForMatches( size_t depth ) const {
	// report matches
	if( ( stack_[depth].lookAheadDepth_ == 0 ) &&  // we are not in the lookahead-phase AND
	    ( stack_[depth].dicPos_.isFinal() ) // arrived at a final state of the dic
	    ) {
	    // for all positions
	    size_t count = 0;
	    for( typename StackItem::iterator position = stack_[depth].begin();
		 position != stack_[depth].end();
		 ++position ) {
		if( levDEA_.isFinal( position->levPos_ ) ) {
		    reportMatch( &( *position ), stack_[depth].dicPos_.getAnnotation() );
		    //std::wcout<<"|d=" << levDEA_.getDistance( position->levPos_ );
		}
		++count;
	    } // for all positions
	} // report matches
    }

    template< typename MinDicType >
    void Vaam< MinDicType >::reportMatch( const Position* cur, int baseWordScore ) const {
	Interpretation interpretation;

	interpretation.setBaseWord( baseWord_ );

	reportMatch_rec( cur, &interpretation );
	interpretation.setLevDistance( levDEA_.getDistance( cur->levPos_ ) );
	interpretation.setBaseWordScore( baseWordScore );

	// find out what word we're talking about by applying the pattern to the baseWord
	std::wstring word = baseWord_;
	interpretation.getInstruction().applyTo( &word );
	interpretation.setWord( word );
	
	if( ( filterDic_ && filterDic_->lookup( interpretation.getWord() ) )  || // if there's a filterDic_ and interpretation.word is in it or ..
	    ( interpretation.getInstruction().size() < minNrOfPatterns_ )

	    ) {
			return;
	}

	if( wasUpperCase_ ) {

	    std::wstring tmp = interpretation.getBaseWord();
	    tmp.at( 0 ) = std::toupper( tmp.at( 0 ), locale_ );
	    interpretation.setBaseWord( tmp );

	    tmp = interpretation.getWord();
	    tmp.at( 0 ) = std::toupper( tmp.at( 0 ), locale_ );
	    interpretation.setWord( tmp );
	}
	// std::wcerr << word << L" (" << baseWord_ << L"): " << interpretation.getBaseWord() << std::endl;

	foundAnswers_ = true;
	interpretations_->receive( interpretation );
    }

    template< typename MinDicType >
    void Vaam< MinDicType >::reportMatch_rec( const Position* cur, Interpretation* interpretation ) const {
	if( cur->mother_.first == -1 ) {
	    return;
	}
	else {
	    // std::wcerr << cur->mother_.first << "," << cur->mother_.second << "," << stack_.at( cur->mother_.first ).size() << std::endl;
	    reportMatch_rec( &( stack_.at( cur->mother_.first ).at( cur->mother_.second ) ), interpretation );
	}

	if( ! cur->posPattern_.empty() ) {
	    interpretation->getInstruction().push_back( cur->posPattern_ );
	}
    }



} // namespace csl
#endif
