namespace csl {

	template< class AnnType_t >
	inline MinDic< AnnType_t >::MinDic( const char* dicFile ) {
		if( dicFile ) {
			loadFromFile( dicFile );
		}
	}

	template< class AnnType_t >
	inline void MinDic< AnnType_t >::loadFromFile( const char* binFile ) {
		FILE* fi = fopen( binFile, "rb" );

		if( ! fi ) {
			throw exceptions::badFileHandle( std::string( "MinDic: Couldn't open file '" ) +
				std::string( binFile ) +
				std::string( "' for reading." ) );
		}

		loadFromStream( fi );
		fclose( fi );
	}


	template< class AnnType_t >
	inline void MinDic< AnnType_t >::loadFromStream( FILE* fi ) {
		// read the MinDic-Header
	  if( fread( &header_, sizeof( Header ), 1, fi ) != 1 ) {
	    throw exceptions::badDictFile( "csl::MinDic::loadFromStream: Read error while reading header" );
	  }

		if( header_.getMagicNumber() != magicNumber_ ) {
			throw exceptions::badDictFile( "MinDic: Magic number comparison failed.\n" );
		}

		nrOfKeys_ = header_.getNrOfKeys();

		// read the TransTable
		TransTable_t::loadFromStream( fi );
		// read the annotations
		annotations_ = new AnnType_t[header_.getNrOfKeys()];
		if( fread( annotations_, sizeof( AnnType_t ), header_.getNrOfKeys(), fi ) != header_.getNrOfKeys() ) {
		  throw exceptions::badDictFile( "csl::MinDic::loadFromStream: Read error while reading annotations" );
		}
	}

	template< class AnnType_t >
	inline void MinDic< AnnType_t >::writeToFile( char const* binFile ) const {
		FILE* fo = fopen( binFile, "wb" );
		if( ! fo ) {
			throw exceptions::badFileHandle( "MinDic: Couldn't open file '" +
				std::string( binFile ) +
				"' for writing." );
		}
		writeToStream( fo );
		fclose( fo );
	}


	template< class AnnType_t >
	inline void MinDic< AnnType_t >::writeToStream( FILE* fo ) const {

		// write the header
		fwrite( &header_, sizeof( Header ), 1, fo );
		// write the TransTable
		TransTable_t::writeToStream( fo );
		// write the annotations
		fwrite( annotations_, sizeof( AnnType_t ), nrOfKeys_, fo );

	}


	template< class AnnType_t >
	inline void MinDic< AnnType_t >::initConstruction() {
	    // std::wcerr<<"MinDic::initConstruction()"<<std::endl;
		TransTable_t::initConstruction();
		tempStates_ =( TempState_t* ) malloc( Global::lengthOfStr * sizeof( TempState_t ) ); // allocate memory for all tempStates
		for( size_t i = 0; i < Global::lengthOfStr; ++i ) {
			new( &( tempStates_[i] ) ) TempState_t(); // call constructor for all tempStates
		}

		hashtable_ = new StateHash< TransTable_t >( *this );

		sizeOfAnnotationBuffer_ = 100000;
		annotations_ = (AnnType_t*)malloc( sizeOfAnnotationBuffer_ * sizeof( AnnType_t ) );

		nrOfKeys_ = 0;
		*lastKey_ = 0; // reset the string to ""
	}

	template< class AnnType_t >
	inline void MinDic< AnnType_t >::finishConstruction() {
		// store the very last word
		int i = wcslen( lastKey_ ); // let this be an integer, because it has to get -1 to break the loop below
		//std::wcout<<"size of lastkey="<<i << std::endl;


		StateId_t storedState = 0;
		for(	 ; i >= 0; --i ) {
			storedState = replaceOrRegister( tempStates_[i] );
			tempStates_[i].reset();
			if( i > 0 ) tempStates_[i-1].addTransition( lastKey_[i-1], storedState, tempStates_[i].getPhValue() );
		}
		setRoot( storedState ); // declare tempStates_[0] to be the root

		// call destructors for tempStates and free the allocated memory
		for( size_t i = 0; i < Global::lengthOfStr; ++i ) {
			tempStates_[i].~TempState_t();
		}
		free( tempStates_ );
		delete( hashtable_ );

		TransTable_t::finishConstruction();

		header_.set( *this );
	}

	template< class AnnType_t >
	inline void MinDic< AnnType_t >::compileDic( const char* txtFile ) {
		wchar_t* key = 0;


		initConstruction();

		std::wifstream fileHandle( txtFile );
//		fileHandle.imbue( CSL_UTF8_LOCALE ); // imbue the stream with csl's custom utf8 locale
		if( !fileHandle.good() ) {
		    throw exceptions::badFileHandle( "Couldn't open file '" +
						     std::string( txtFile ) +
						     "' for reading." );
		}

		std::wstring line;

		size_t lineCount = 0;
		while( std::getline( fileHandle, line ).good() )  {

			if ( line.length() > Global::lengthOfLongStr ) {
				throw exceptions::badInput( "csl::MinDic::compileDic: Maximum length of input line violated (set by Global::lengthOfLongStr)" );
			}
			AnnType_t annotation;
			parseAnnotation( &line, &annotation );

			addToken( line.c_str(), annotation );
			++lineCount;
		}
		if( errno == EILSEQ ) { // catch encoding error
		    throw exceptions::badInput( "MinDic::compileDic: Encoding error in input sequence." );
		}

		fileHandle.close();
		finishConstruction();
	}

	template<>
	inline void MinDic< int >::parseAnnotation( std::wstring* str, AnnType_t* annotation ) const {

		size_t delimPos = str->find_first_of( Global::keyValueDelimiter );

		if( delimPos != str->npos ) {
			*annotation = wcstol( (str->substr( delimPos + 1 ) ).c_str() , 0, 0 );
			str->resize( delimPos );

		}
		else *annotation = 0;
	}


	template< class AnnType_t >
	inline void MinDic< AnnType_t >::addToken( const wchar_t* key, const AnnType_t& annotation ) {

	    static size_t commonPrefix, lengthOfKey;
		static StateId_t storedState;

		commonPrefix = lengthOfKey = storedState = 0;

		// key = key;

		//wprintf(L"input: %ls -> %d\n", key, annotation );


		// remark: maybe this could be spared if we remember the length during widechar-conversion
		lengthOfKey = wcslen( key );


		if( lengthOfKey == 0 ) {
		    throw exceptions::badInput( "csl::MinDic: Insertion of empty key " );
		}

		if( lengthOfKey > Global::lengthOfStr - 1 ) {
		    throw exceptions::badInput( "MinDic: Global::string_length sets the maximum string length for an entry of a MinDic. Maximum violated" );
		}

		// check alphabetical order
		if( *lastKey_ && ( wcscmp( lastKey_, key ) >= 0 ) ) {
			std::wcerr<<"Alphabetical order violated:"<<std::endl
				<<"**********"<<std::endl
				<<lastKey_<<std::endl
				<<key<<std::endl
				<<"**********"<<std::endl;
			throw exceptions::badInput( "MinDic::addToken: Alphabetical order violated" );
			exit( 1 );
		}

		///////////////////// store suffix of lastKey_
		commonPrefix = 0;
		while( ( key[commonPrefix] == lastKey_[commonPrefix] ) && key[commonPrefix] ) {
			++commonPrefix;
		}
		// e.g., commonPrefix==2 if first two letters of both words are equal


		for( size_t i = wcslen( lastKey_ ); i > commonPrefix; --i ) {
			storedState = replaceOrRegister( tempStates_[i] );
			tempStates_[i-1].addTransition( lastKey_[i-1], storedState, tempStates_[i].getPhValue() );
			tempStates_[i].reset();
		}

		//////////////////// set final state of key
		tempStates_[lengthOfKey].setFinal();

		// store value
		// first, see that annotation buffer is large enough
		if( sizeOfAnnotationBuffer_ <= ( nrOfKeys_ + 1 ) ) {
		    sizeOfAnnotationBuffer_ *= 2;
		    annotations_ = (AnnType_t*)realloc( annotations_, sizeOfAnnotationBuffer_ * sizeof( AnnType_t ) );
		    if( !annotations_ ) throw( exceptions::cslException( "csl::MinDic: could not re-allocate memory for annotation array." ) );
		}
		annotations_[nrOfKeys_] = annotation;

		wcscpy( lastKey_, key );
		++nrOfKeys_;

		if( ! ( nrOfKeys_ %  100000 ) ) {
			std::wcerr	<< nrOfKeys_ /1000 << "k tokens processed. "
						<< TransTable_t::getNrOfStates() / 1000 << "k states."
						// << "key was: " << key
						<< std::endl;
		}

	} // method addToken()

	template< class AnnType_t >
	inline StateId_t MinDic< AnnType_t >::replaceOrRegister( TempState_t& state ) {

	    StateId_t storedState = 0;
	    if( ( storedState = hashtable_->findState( state ) ) == 0 ) { // if equiv. state doesn't exist
		storedState = storeTempState( state ); // store it
		hashtable_->insert( state, storedState ); // add it to the hashtable
	    }

	    return storedState;
	}


	template< class AnnType_t >
	inline bool MinDic< AnnType_t >::lookup( const wchar_t* key, AnnType_t* annotation ) const {
	    if( ! readyToRead() ) {
		throw exceptions::cslException( "csl::MinDic::lookup: Dict not ready to read" );
	    }
		size_t tokIdx = 0;
		if( TransTable_t::getTokenIndex( key, &tokIdx ) ) {
			if( annotation ) *annotation = annotationsAt( tokIdx );
			return true;
		}
		else {
			return false;
		}
	}

	template< class AnnType_t >
	inline bool MinDic< AnnType_t >::lookup( std::wstring const& key, AnnType_t* annotation ) const {
	    return lookup( key.c_str(), annotation );
	}

	template< class AnnType_t >
	inline bool MinDic< AnnType_t >::hasPrefix( std::wstring const& prefix ) const {
	    if( ! readyToRead() ) {
		throw exceptions::cslException( "csl::MinDic::lookup: Dict not ready to read" );
	    }
		size_t state = getRoot();
		for( std::wstring::const_iterator c = prefix.begin(); c != prefix.end() && (state != 0) ; ++c ) {
			//std::wcout<<L"Walk with "<<(int)*c << std::endl;
			if( *c >200 ) {
				return false;
				// state.walk( 0 );
				//std::wcout << L"Walked "<< (int)*c << std::endl;
			}
			else state = walk( state, *c );
			//std::wcout << L"Walked"<< std::endl;
		}
		if( state > 0 ) {
			//std::wcout<<L"prefix"<<" is a prefix"<<std::endl;
			return true;
		}
		else {
			//std::wcout<<L"prefix"<<" is not a prefix"<<std::endl;
			return false;
		}
	}

	template< class AnnType_t >
	inline bool MinDic< AnnType_t >::getAnnotation( const wchar_t* key, AnnType_t* annotation ) const {
		std::cerr<<"csl::MinDic::getAnnotation: This method is deprecated. Instead, use method lookup() with the same signature"<<std::endl;
		return lookup( key, annotation );
	}

	template< class AnnType_t >
	inline const AnnType_t& MinDic< AnnType_t >::getAnnotation( size_t perfHashValue ) const {
		return annotationsAt( perfHashValue );
	}

	template< class AnnType_t >
	inline const AnnType_t& MinDic< AnnType_t >::annotationsAt( size_t n ) const {
		if( n >= nrOfKeys_ ) throw exceptions::outOfRange( "csl::MinDic: out-of-range request for annotation-array." );
		return annotations_[n];
	}

	template< class AnnType_t >
	inline size_t MinDic< AnnType_t >::getNrOfKeys() const {
		return nrOfKeys_;
	}

	template< class AnnType_t >
	inline void MinDic< AnnType_t >::printDic() const {
		count_ = 0;
		printDic_rec( getRoot(), 0, 0 );
	}

// 	template< class AnnType_t >
// 	inline void MinDic< AnnType_t >::printDic() const {
// 	    for( TokenIterator it = tokensBegin(); it != tokensEnd(); ++it ) {
// 		std::wcout << *it << std::endl;
// 	    }
// 	}

	template< class AnnType_t >
	inline void MinDic< AnnType_t >::printDic_rec( StateId_t pos, int depth, size_t perfHashValue ) const {
		int newPos;
		static wchar_t w[Global::lengthOfStr];
		size_t newPerfHashValue;

		const wchar_t* transitions = getSusoString( pos );
		while( *transitions ) {
			newPerfHashValue = perfHashValue;




			if( ( newPos = walkPerfHash( pos, *transitions, &newPerfHashValue ) ) ) {
				w[depth] = *transitions;
				if( isFinal( newPos ) ) {
				    w[depth+1] = 0;
				    std::wcout<< w <<"#"<<getAnnotation( newPerfHashValue )<<std::endl;

				    if( ( ++count_ % 100000 ) == 0 ) fprintf( stderr, "%d\n", (int)count_ );
				} // if isFinal
				printDic_rec( newPos, depth + 1, newPerfHashValue );

			} // if couldWalk
			else {
			    throw exceptions::badDictFile( "suso-string seems to be corrupted." );
			}
			++transitions;
		} // while
	} // end of method


	template< class AnnType_t >
	inline void MinDic< AnnType_t >::doAnalysis() const {

		float annotations_MB = (float)( nrOfKeys_ * sizeof( AnnType_t ) ) / 1048576;

		TransTable_t::doAnalysis();
		printf( "**********\nMinDic Analysis\n**********\nnr of keys:\t%zd\nannotation array: %.3f MB\n\n",
			nrOfKeys_,
			annotations_MB
			);


	}

} //eon
