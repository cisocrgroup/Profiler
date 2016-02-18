#ifndef CSL_MSMATCH_TCC
#define CSL_MSMATCH_TCC CSL_MSMATCH_TCC

namespace csl {

    template<>
    inline MSMatch< STANDARD >::MSMatch( size_t k, const char* minDicFile ) :
	fbDic_( 0 ),
	dictFW_( 0 ),
	dictBW_( 0 ),
	caseMode_( Global::asIs ),
	k_( k )
    {
	if( minDicFile ) {
	    dictFW_ = new MinDic<>( minDicFile );
	}

	dictBW_ = 0;
 
	levDEASecond_ = new LevDEA( k_ );
    }

    template<>
    inline MSMatch< FW_BW >::MSMatch( size_t k, const char* fbDicFile ) :
	fbDic_( 0 ),
	dictFW_( 0 ),
	dictBW_( 0 ),
	caseMode_( Global::asIs ),
	k_( k )
    {
	if( fbDicFile ) {
	    fbDic_ = new FBDic<>( fbDicFile );
	    dictFW_ =  &( fbDic_->getFWDic() );
	    dictBW_ = &( fbDic_->getBWDic() );
	}

	levDEAFirst_ = new LevDEA( 0 );
	levDEASecond_ = new LevDEA( 0 );
    }
    
    template< MSMatchMode Mode >
    inline MSMatch< Mode >::~MSMatch() {
    }

    template<>
    inline void MSMatch< FW_BW>::setFBDic( FBDic<> const& fbDic ) {
	fbDic_ = &fbDic;
	dictFW_ =  &( fbDic_->getFWDic() );
	dictBW_ = &( fbDic_->getBWDic() );
    }






    template< MSMatchMode Mode >
    inline void MSMatch< Mode >::intersectSecond( int dicPos, LevDEA::Pos levPos, int depth ) {
	static int newDicPos;
	static LevDEA::Pos newLevPos;

	if( (size_t)depth > ( Global::lengthOfWord-1 ) ) throw exceptions::cslException( "csl::MSMatch: candidate too long" );

	// store w if node is final in dic and lev;
	static int levDistanceSecond;
	levDistanceSecond = levDEASecond_->getDistance( levPos );
	if( curDict_->isFinal( dicPos ) && ( levDistanceSecond >= minDistSecond_ ) ) {
	    word_[depth] = 0;
	    static wchar_t wordReversed[Global::lengthOfWord];
	    wchar_t* wordCorrectDir; // In either case, this points to the token in its original direction

	    // if necessary, reverse word to its original
	    if( reverse_ ) {
		for( int i = depth - 1, iRev = 0; i >=0; --i, ++iRev ) wordReversed[iRev] = word_[i];
		wordReversed[depth] = 0;
		wordCorrectDir = wordReversed;
	    }
	    else wordCorrectDir = word_;

	    // follow the word through the automaton once more to get the perfect hashing value
	    size_t perfHashValue;
	    uint_t dicPos2;
	    LevDEA::Pos levPos2;
	    perfHashValue = 0;
	    dicPos2 = dictFW_->getRoot();
	    for( wchar_t* c = wordCorrectDir; *c; ++c ) {
		assert( dicPos2 ); // this is a logical conclusion, so it's an assert, not an exception
		dicPos2 = dictFW_->walkPerfHash( dicPos2, *c, &perfHashValue );
	    }

	    // printf( "FOUND SECOND:%ls, dist of 2nd part is %d\n", word_, levDistanceSecond ); // DEBUG
	    // printf( "levDistanceFirst_ = %d\n", levDistanceFirst_ ); // DEBUG

	    // push word and annotated value into the results_ container
	    size_t levDistance = levDistanceFirst_ + levDistanceSecond;
	    CandidateMap::iterator pos = results_.find( wordCorrectDir );
	    if( pos == results_.end() ) {
		// printf( "MATCH FIRST TIME :%ls, dist is %d\n", wordCorrectDir, levDistance ); // DEBUG
		results_.insert( std::pair< std::wstring, std::pair< size_t, int > >( 
				     std::wstring( wordCorrectDir ), 
				     std::pair< size_t, int >( 
					 levDistance, 
					 dictFW_->getAnnotation( perfHashValue ) 
					 ) 
				     )
		    );
	    }
	    else {
		// printf( "MATCH AGAIN:%ls, dist is %d\n", wordCorrectDir, levDistance ); // DEBUG
		// (pos->second).first holds the lev.-distance of the candidate
 		if( (pos->second).first > levDistance ) {
 		    (pos->second).first = levDistance;
 		}
	    }

	} // if match was found

	const wchar_t* c = curDict_->getSusoString( dicPos );
	for( ; *c; ++c ) {
//	    printf( "word is %s, dicpos is %d, Try with %c(%d)\n", word_, dicPos, alph_.decode( *c ), *c );
	    if( ( newLevPos = levDEASecond_->walk( levPos, *c ) ).isValid() ) {
		newDicPos = curDict_->walk( dicPos, *c );
		

		assert( newDicPos != 0 ); // the transition always exists
		word_[depth] = *c;
		word_[depth+1] = 0;
		// printf(" intersectSecond at depth %d: %s\n", depth, word_ ); // DEBUG

//  word_[depth+1]=0;std::cout<<"word="<<word<<std::endl;

		intersectSecond( newDicPos, newLevPos, depth + 1 );
	    }
	}
    }

    template<>
    inline void MSMatch< FW_BW >::intersectFirst( int dicPos, LevDEA::Pos levPos, int depth ) {
	static int newDicPos;
	static LevDEA::Pos newLevPos;

	// if the position is final the lev.automaton, proceed to right half of the pattern, starting
	// with the same state of the dict and the start state of the LevDEA
	levDistanceFirst_ = levDEAFirst_->getDistance( levPos );
	if( levDistanceFirst_ >= minDistFirst_ ) {
	    // printf( "FOUND FIRST:%ls, dist of 1st part is %d\n", word_, levDistanceFirst_ );// DEBUG
	    intersectSecond( dicPos, LevDEA::Pos( 0, 0 ), depth );
	}


	const wchar_t* c = curDict_->getSusoString( dicPos );
	for( ; *c; ++c ) {
	    if( ( newLevPos = levDEAFirst_->walk( levPos, *c ) ).isValid() ) {
		newDicPos = curDict_->walk( dicPos, *c );
		assert( newDicPos ); // the transition always exists
		word_[depth] = *c;
		word_[depth+1] = 0;
		// printf(" intersectFirst at depth %d: %s\n", depth, word_ );

		//  word_[depth+1]=0;std::cout<<"word="<<word<<std::endl;

		intersectFirst( newDicPos, newLevPos, depth + 1 );
	    }
	}
    }

    template<>
    inline void MSMatch< FW_BW >::queryCases_0() {
	int ann = 0;
	if( dictFW_->lookup( pattern_, &ann ) ) {
	    results_.insert( std::pair< std::wstring, std::pair< size_t, int > >( std::wstring( pattern_ ), std::pair< size_t, int >( 0, ann ) ) );
	}
    }
    
    template<>
    inline void MSMatch< FW_BW >::queryCases_1() {
	uint_t pos = 0;

     	// 0 | 0,1 errors
 	reverse_ = false;
 	curDict_ = dictFW_;
 	if( ( pos = curDict_->walkStr( curDict_->getRoot(), patLeft_ ) ) ) {
	    levDistanceFirst_ = 0;
 	    wcscpy( word_, patLeft_ );
	    minDistSecond_ = 0;
	    levDEASecond_->setDistance( 1 );
 	    levDEASecond_->loadPattern( patRight_ );
 	    intersectSecond( pos, LevDEA::Pos( 0, 0 ), wcslen( patLeft_ ) );
 	}

     	// 1 | 0 errors
  	reverse_ = true;
 	curDict_ = dictBW_;
 	if( ( pos = curDict_->walkStr( curDict_->getRoot(), patRightRev_ ) ) ) {
	    levDistanceFirst_ = 0;
 	    wcscpy( word_, patRightRev_ );
	    minDistSecond_ = 1;
	    levDEASecond_->setDistance( 1 );
 	    levDEASecond_->loadPattern( patLeftRev_ );
	    intersectSecond( pos, LevDEA::Pos( 0, 0 ), wcslen( patRightRev_ ) );
 	}
    }
    
    template<>
    inline void MSMatch< FW_BW >::queryCases_2() {
	uint_t pos = 0;
	
//	printf( "0 | 0,1,2 errors\n" );
	// 0 | 0,1,2 errors
 	reverse_ = false;
 	curDict_ = dictFW_;
	// load pattern outside the if-statement: we need it anyways in the next case
	levDEASecond_->loadPattern( patRight_ );
 	if( ( pos = curDict_->walkStr( curDict_->getRoot(), patLeft_ ) ) ) {
	    levDistanceFirst_ = 0;
 	    wcscpy( word_, patLeft_ );
	    minDistSecond_ = 0;
	    levDEASecond_->setDistance( 2 );
 	    intersectSecond( pos, LevDEA::Pos( 0, 0 ), wcslen( patLeft_ ) );
 	}

//	printf( "1 | 0,1 errors\n" );
 	// 1 | 0,1 errors
 	reverse_ = false;
 	curDict_ = dictFW_;
	minDistFirst_ = 1;
	levDEAFirst_->setDistance( 1 );
	levDEAFirst_->loadPattern( patLeft_ );
	// pattern for levDEASecond is still valid from last case
	minDistSecond_ = 0;
	levDEASecond_->setDistance( 1 );
	intersectFirst( curDict_->getRoot(), LevDEA::Pos( 0, 0 ), 0 );

//	printf( "2 | 0 errors\n" );
	// 2 | 0 errors
 	reverse_ = true;
 	curDict_ = dictBW_;
 	if( ( pos = curDict_->walkStr( curDict_->getRoot(), patRightRev_ ) ) ) {
	    levDistanceFirst_ = 0;
 	    wcscpy( word_, patRightRev_ );
	    minDistSecond_ = 2;
	    levDEASecond_->setDistance( 2 );
 	    levDEASecond_->loadPattern( patLeftRev_ );
 	    intersectSecond( pos, LevDEA::Pos( 0, 0 ), wcslen( patRightRev_ ) );
 	}
    }

    template<>
    inline void MSMatch< FW_BW >::queryCases_3() {
  	uint_t pos = 0;

     	// 0 | 0,1,2,3 errors
 	reverse_ = false;
 	curDict_ = dictFW_;
	// load pattern outside the if-statement: we need it anyways in the next case
	levDEASecond_->loadPattern( patRight_ );
 	if( ( pos = curDict_->walkStr( curDict_->getRoot(), patLeft_ ) ) ) {
	    levDistanceFirst_ = 0;
 	    wcscpy( word_, patLeft_ );
	    // printf( "FOUND FIRST:%ls, dist of 1st part is 0\n", word_ );
	    minDistSecond_ = 0;
	    levDEASecond_->setDistance( 3 );
 	    intersectSecond( pos, LevDEA::Pos( 0, 0 ), wcslen( patLeft_ ) );
 	}

 	// 1 | 0,1,2 errors
 	reverse_ = false;
 	curDict_ = dictFW_;
	levDEAFirst_->setDistance( 1 );
	levDEAFirst_->loadPattern( patLeft_ );
	minDistFirst_ = 1;
	levDEASecond_->setDistance( 2 );
	// pattern for levDEASecond is still valid from last case
	minDistSecond_ = 0;
	intersectFirst( curDict_->getRoot(), LevDEA::Pos( 0, 0 ), 0 );

	// 2,3 | 0 errors
	reverse_ = true;
	curDict_ = dictBW_;
	// load pattern outside the if-statement: we need it anyways in the next case
	levDEASecond_->loadPattern( patLeftRev_ );
	if ( ( pos = curDict_->walkStr( curDict_->getRoot(), patRightRev_ ) ) ) {
	    levDistanceFirst_ = 0;
	    wcscpy( word_, patRightRev_ );
	    //printf( "FOUND FIRST:%ls, dist of 1st part is 0\n", word_ );
	    levDEASecond_->setDistance( 3 );
	    minDistSecond_ = 2;
	    
	    intersectSecond( pos, LevDEA::Pos( 0, 0 ), wcslen( patRightRev_ ) );
	}

	// 2 | 1 errors
 	reverse_ = true;
 	curDict_ = dictBW_;
	levDEAFirst_->setDistance( 1 );
	levDEAFirst_->loadPattern( patRightRev_ );
	minDistFirst_ = 1;
	levDEASecond_->setDistance( 2 );
	// pattern for levDEASecond is still valid from last case
	minDistSecond_ = 2;
	intersectFirst( curDict_->getRoot(), LevDEA::Pos( 0, 0 ), 0 );
    }



    

    template<>
    inline void MSMatch< STANDARD >::intersect( int dicPos, LevDEA::Pos levPos, int depth ) {
	static int newDicPos;
	static LevDEA::Pos newLevPos;

	for( const wchar_t* c = dictFW_->getSusoString( dicPos ); *c; ++c ) {
	    if( ( newDicPos = dictFW_->walk( dicPos, *c ) ) && ( newLevPos = curLevDEA_->walk( levPos, *c ) ).isValid() ) {
		word_[depth] = *c;

//  word[depth+1]=0;std::cout<<"word="<<word<<std::endl;

		// print w if node is final in dic and lev;
		if( dictFW_->isFinal( newDicPos ) && curLevDEA_->isFinal( newLevPos ) ) {
		    word_[depth+1] = 0;
		    
		    // push word and annotated value into the output list
		    // follow the word through the automaton once more to get the perfect hashing value
		    static size_t perfHashValue; static uint_t dicPos2;
		    perfHashValue = 0; dicPos2 = dictFW_->getRoot();
		    for( wchar_t* cc = word_; *cc; ++cc ) {
			dicPos2 = dictFW_->walkPerfHash( dicPos2, *cc, &perfHashValue );
		    }
		    foundAnswers_ = true;
		    // USE DUMMY VALUE FOR LEVDISTANCE
		    candReceiver_->receive( word_, 42, dictFW_->getAnnotation( perfHashValue ) );
		}
		intersect( newDicPos, newLevPos, depth + 1 );
	    }
	}
    }

    template<>
    inline bool MSMatch< STANDARD >::query( const wchar_t* pattern, CandidateReceiver& candReceiver ) {
	if( ! dictFW_ ) throw exceptions::LogicalError( "csl::MSMatch< STANDARD >::query: Method called without a dictionary being available" );

	candReceiver_ = &candReceiver;
	wcscpy( pattern_, pattern );
	
	curLevDEA_ = levDEASecond_;
	curLevDEA_->setDistance( k_ );
	curLevDEA_->loadPattern( pattern );
	foundAnswers_ = false;
	intersect( dictFW_->getRoot(), LevDEA::Pos( 0, 0 ), 0 );

	return foundAnswers_;
    } // method query()


    template<>
    inline bool MSMatch< FW_BW >::query( const wchar_t* pattern, CandidateReceiver& candReceiver ) {
	if( ! ( dictFW_ && dictBW_ ) ) throw exceptions::LogicalError( "csl::MSMatch< STANDARD >::query: Method called without a dictionary being available" );
	if( ! *pattern ) {
	    throw( exceptions::badInput( "csl::MSMatch::query: Empty pattern string is forbidden" ) );
	}

	candReceiver_ = &candReceiver; // is not necessary at the moment
	
	// std::wcout << "query=" << pattern << std::flush;
	// std::wcout << ",results=" << &results_ << std::flush;
	// std::wcout << ",size=" << results_.size() << std::endl;
	// for( CandidateMap::iterator it = results_.begin(); it != results_.end(); ++it ) {
	//     std::wcout << ">>" << it->first << std::endl;
	// }
	results_.clear();

	pattern_[Global::lengthOfWord - 1] = 0;
	wcsncpy( pattern_, pattern, Global::lengthOfWord );
	if( pattern_[Global::lengthOfWord - 1] != 0 ) {
	    throw exceptions::badInput( "csl::MSMATCH::query: Maximum Pattern length (as specified by Global::lengthOfWord) violated." );
	}

	bool wasUpperCase = 0;
	if( ( caseMode_ != Global::asIs ) && iswupper( pattern_[0] ) ) {
	    wasUpperCase = 1;
	    pattern_[0] = std::tolower( pattern_[0] );
	}

	// split pattern into 2 halves
	size_t patLength = wcslen( pattern_ );
	size_t patCenter = (size_t) ( patLength / 2 );
	size_t cLeft = 0, cRight = 0;	
	for ( size_t i = 0; i < patLength; ++i ) {
	    if( i < patCenter ) patLeft_[cLeft++] = pattern_[i];
	    else patRight_[cRight++] = pattern_[i];
	}
	patLeft_[cLeft] = patRight_[cRight] = 0;
	
	// reverse patterns
	for( int i = cLeft - 1, iRev = 0; i >=0; --i, ++iRev ) patLeftRev_[iRev] = patLeft_[i];
	for( int i = cRight - 1, iRev = 0; i >=0; --i, ++iRev ) patRightRev_[iRev] = patRight_[i];
	patLeftRev_[cLeft] = patRightRev_[cRight] = 0;

// 	printf("pattern=%ls\npatLeft=%ls\npatRight=%ls\npatLeftRev=%ls\npatRightRev=%ls\n", pattern_, patLeft_, patRight_, patLeftRev_, patRightRev_ ); // DEBUG

	if( k_ == 0 ) queryCases_0();
	else if( k_ == 1 ) queryCases_1();
	else if( k_ == 2 ) queryCases_2();
	else if( k_ == 3 ) queryCases_3();
	else throw exceptions::invalidLevDistance( "csl::MSMATCH::query: invalid levenshtein distance" );

	for( CandidateMap::const_iterator it = results_.begin(); it != results_.end(); ++it ) {
	    if( ( caseMode_ == Global::restoreCase ) && wasUpperCase ) {
		std::wstring tmp = it->first;
		if( ! tmp.empty() ) tmp.at( 0 ) = std::toupper( tmp.at( 0 ) );
		candReceiver_->receive( tmp.c_str(), (it->second).first , (it->second).second  );
	    }
	    else candReceiver_->receive( (it->first).c_str(), (it->second).first , (it->second).second );
	}
	return ( ! results_.empty() );
    }
    
	

} // eon

#endif
