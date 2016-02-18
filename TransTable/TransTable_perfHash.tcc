#ifndef CSL_TRANSTABLE_PERFHASH_TCC
#define CSL_TRANSTABLE_PERFHASH_TCC CSL_TRANSTABLE_PERFHASH_TCC


namespace csl {

    /******************** CONSTRUCTORS / DESTRUCTOR ********************/

    template< typename InternalCharType__, typename SizeType__ >
    TransTable< TT_PERFHASH, InternalCharType__, SizeType__ >
    ::TransTable()
    {
	cells_ = 0;
	susoStrings_ = 0;
	susoHash_ = 0;
	ready_ = 0;
    }

    template< typename InternalCharType__, typename SizeType__ >
    TransTable< TT_PERFHASH, InternalCharType__, SizeType__ >
    ::~TransTable() {
	if( cells_ ) free( cells_ );
	if( susoStrings_ ) free( susoStrings_ );
    }


    /******************                           **********************/
    template< typename InternalCharType__, typename SizeType__ >
    bool
    TransTable< TT_PERFHASH, InternalCharType__, SizeType__ >
    ::readyToRead() const {
	return ready_ == 1;
    }

    template< typename InternalCharType__, typename SizeType__ >
    bool
    TransTable< TT_PERFHASH, InternalCharType__, SizeType__ >
    ::readyToWrite() const {
	return ready_ == 2;
    }


    /****************** BASIC ACCESS TO THE TRANSITION TABLE **********************/

    template< typename InternalCharType__, typename SizeType__ >
    typename TransTable< TT_PERFHASH, InternalCharType__, SizeType__ >::StateId_t
    TransTable< TT_PERFHASH, InternalCharType__, SizeType__ >
    ::walk( StateId_t state, wchar_t c ) const {
	assert( cells_[state].isState() );
	return ( ( c > 0 ) && 
		 ( state + c < sizeOfUsedCells_  ) && 
		 ( cells_[state + c].getKey() == c ) ) ? cells_[state + c].getValue() : 0;
    }

    template< typename InternalCharType__, typename SizeType__ >
    typename TransTable< TT_PERFHASH, InternalCharType__, SizeType__ >::StateId_t
    TransTable< TT_PERFHASH, InternalCharType__, SizeType__ >
    ::walkStr( StateId_t state, const wchar_t* str ) const {
	while( *str && state ) {
	    state = walk( state, *str );
	    ++str;
	}
	return state;
    }

    template< typename InternalCharType__, typename SizeType__ >
    typename TransTable< TT_PERFHASH, InternalCharType__, SizeType__ >::StateId_t
    TransTable< TT_PERFHASH, InternalCharType__, SizeType__ >
    ::walkPerfHash( StateId_t state, wchar_t c, size_t* phValue ) const {
	//std::wcout << "walk from " << state << " with " << c << "," << (size_t)c << std::endl;
	assert( cells_[state].isState() );
	if( ( c > 0 ) && ( state + c < sizeOfUsedCells_  ) && ( cells_[state + c].getKey() == c ) ) {
	    *phValue += cells_[state + c].getThirdValue();
	    //std::wcout << "To " << cells_[state + c].getValue() << std::endl;
	    return cells_[state + c].getValue();
	}
	return 0;
    }

    template< typename InternalCharType__, typename SizeType__ >
    typename TransTable< TT_PERFHASH, InternalCharType__, SizeType__ >::StateId_t
    TransTable< TT_PERFHASH, InternalCharType__, SizeType__ >
    ::walkStrPerfHash( StateId_t state, const wchar_t* str, size_t* phValue ) const {
	while( *str && state ) {
	    state = walkPerfHash( state, *str, phValue );
	    //std::wcout<<state<<", "<<*str<< "final="<<(isFinal( state )? 1: 0 ) <<std::endl;
	    ++str;
	}
	return state;
    }


    template< typename InternalCharType__, typename SizeType__ >
    typename TransTable< TT_PERFHASH, InternalCharType__, SizeType__ >::StateId_t
    TransTable< TT_PERFHASH, InternalCharType__, SizeType__ >
    ::getRoot() const {
	return root_;
    }

    template< typename InternalCharType__, typename SizeType__ >
    bool
    TransTable< TT_PERFHASH, InternalCharType__, SizeType__ >
    ::isFinal( StateId_t state ) const {
	return cells_[state].isFinalState();
	
    }

    template< typename InternalCharType__, typename SizeType__ >
    const wchar_t*
    TransTable< TT_PERFHASH, InternalCharType__, SizeType__ >
    ::getSusoString( StateId_t state ) const {
	return ( susoStrings_ + cells_[state].getThirdValue() );
    }
    
    template< typename InternalCharType__, typename SizeType__ >
    bool
    TransTable< TT_PERFHASH, InternalCharType__, SizeType__ >
    ::getTokenIndex( const wchar_t* key, size_t* tokID ) const {
	*tokID = 0;

	uint_t pos = walkStrPerfHash( getRoot(), key, tokID );
	return ( ( pos != 0 ) && isFinal( pos ) );
    }

    template< typename InternalCharType__, typename SizeType__ >
    typename TransTable< TT_PERFHASH, InternalCharType__, SizeType__ >::InternalSize_t
    TransTable< TT_PERFHASH, InternalCharType__, SizeType__ >
    ::getStateAnnotation( StateId_t state ) const {
	return ( cells_[state].getValue() );
    }
    
    


    /******************** CONSTRUCTION ********************/

    template< typename InternalCharType__, typename SizeType__ >
    void TransTable< TT_PERFHASH, InternalCharType__, SizeType__ >
    ::initConstruction() {
	nrOfCells_ = 0;
	allocCells( 80 );
	nrOfStates_ = 0;

	cells_[0].setState(); // cells_[0] is the failure state
	firstFreeCell_ = 1; // cells_[0] is the failure state
	sizeOfUsedCells_ = 1; // cells_[0] is the failure state

	header_.set( *this );

	susoStrings_ = 0;
	lengthOfSusoStrings_ = 0;

	susoHash_ = new Hash< wchar_t >( 100000, susoStrings_, lengthOfSusoStrings_ );
	ready_ = 2;
    }

    template< typename InternalCharType__, typename SizeType__ >
    void TransTable< TT_PERFHASH, InternalCharType__, SizeType__ >
    ::finishConstruction() {
	nrOfCells_ = sizeOfUsedCells_;
	
	lengthOfSusoStrings_ = susoHash_->getLengthOfKeyStrings();
	delete( susoHash_ );
	susoHash_ = 0;

	header_.set( *this );
	ready_ = 1;
    }


    /**
     * resize the array of cells
     */
    template< typename InternalCharType__, typename SizeType__ >
    void TransTable< TT_PERFHASH, InternalCharType__, SizeType__ >
    ::allocCells( size_t newNrOfCells ) {
	cells_ = ( Cell_t* ) realloc( cells_, newNrOfCells * sizeof( Cell_t ) );
	memset( ( cells_ + nrOfCells_ ), 0, ( newNrOfCells - nrOfCells_ ) * sizeof( Cell_t ) );
	nrOfCells_ = newNrOfCells;
// std::cout<<"Resized array cells to "<<newNrOfCells<<" cells."<<std::endl;
    }

    template< typename InternalCharType__, typename SizeType__ >
    typename TransTable< TT_PERFHASH, InternalCharType__, SizeType__ >::StateId_t
    TransTable< TT_PERFHASH, InternalCharType__, SizeType__ >
    ::storeTempState( TempState_t& state ) {
	++nrOfStates_;

	// insert susoString and keep offset
	size_t susoPtr = susoHash_->findOrInsert( state.getSusoString() );

	uint_t slot = findSlot( state );

	// mark stateCell and insert the only annotation and suso offset
	cells_[slot].setState();
	cells_[slot].setValue( state.getAnnotation() );
	cells_[slot].setThirdValue( susoPtr );

	// set finalState
	if ( state.isFinal() ) {
	    cells_[slot].setFinal();
	}

	// insert all transitions
	InternalChar_t label = 0;
	for ( typename TempState< TransTable_t >::const_TransIterator it = state.transBegin(); 
		  it != state.transEnd() ; 
		  ++it ) {
	    label = it->getLabel(); // we need this outside the loop
	    cells_[slot + label].setTrans( label, it->getTarget(), it->getPhNumber() );
	}
	
	// update sizeOfUsedCells_
	sizeOfUsedCells_ = std::max( sizeOfUsedCells_, (size_t)( slot + label + 10 ) ); // 10 is a cautious value here ...
	// sizeOfUsedCells_ = ( sizeOfUsedCells_ > (size_t)( slot + label + 10 ) ) ? sizeOfUsedCells_ : (size_t)( slot + label + 10 );
	// sizeOfUsedCells_ = std::max( sizeOfUsedCells_, ( slot + Global::maxNrOfChars + 2 ) ); // this is if, at runtime, we want to assure that each state cell is followed by at least Global::maxNrOfChars cells.


	// the following lines are to prevent firstFreeCell_ to get stuck in a lower area, where none of the
	// new states can never fit.
	if( ( slot - firstFreeCell_ ) > searchWindow ) {
//	    std::cerr << "ffc hack: " << firstFreeCell_ << std::endl;
	    firstFreeCell_ = slot - searchWindow; // a slot which is really empty is searched right below
	}

	//update firstFreeCell
	for ( ; ! ( cells_[firstFreeCell_].isEmpty() ); ++firstFreeCell_ );

	return slot;
    }

    template< typename InternalCharType__, typename SizeType__ >
    void
    TransTable< TT_PERFHASH, InternalCharType__, SizeType__ >
    ::setRoot( StateId_t root ) {
	root_ = root;
    }



    /**
     * find slot to store state
     */
    template< typename InternalCharType__, typename SizeType__ >
    typename TransTable< TT_PERFHASH, InternalCharType__, SizeType__ >::StateId_t
    TransTable< TT_PERFHASH, InternalCharType__, SizeType__ >
    ::findSlot( const TempState_t& state ) {
	size_t slot = firstFreeCell_ - 1; // is incremented again at beginning of loop

	bool mightFit = false;
	while ( !mightFit ) {
	    do {
		++slot;
		while ( ( slot  + Global::maxNrOfChars + 1 ) > nrOfCells_ ) {
		    allocCells( nrOfCells_ * 2 );
		}
	    } while ( ! ( cells_[slot].isEmpty() ) );

	    mightFit = true;

	    // check if all required cells for transitions are available
	    for ( typename TempState< TransTable_t >::const_TransIterator it = state.transBegin(); 
		  it != state.transEnd() ; 
		  ++it ) {
		if ( !cells_[slot + it->getLabel()].isEmpty() ) {
		    mightFit = false;
		}
	    }
	}
	return slot;
    }




    template< typename InternalCharType__, typename SizeType__ >
    bool
    TransTable< TT_PERFHASH, InternalCharType__, SizeType__ >
    ::compareStates( const TempState_t& temp, StateId_t comp ) const {
	if ( temp.isFinal() != isFinal( comp ) ) return false; // both must be either final or not

	if( temp.getAnnotation() != getStateAnnotation( comp ) ) return false; // values must be equal

	const wchar_t* c_comp = getSusoString( comp );
	typename TempState< TransTable_t >::const_TransIterator tempIt = temp.transBegin();
	for ( ; 
		 ( tempIt != temp.transEnd() ) && ( *c_comp != 0 ); 
		  ++tempIt, ++c_comp ) {
		if( tempIt->getLabel() != *c_comp ) return false; // both must have the same char as next label
		if ( tempIt->getTarget() != walk( comp, *c_comp ) ) return false; // both must point to the same state
	}
	if( ( tempIt != temp.transEnd() ) || ( *c_comp != 0 )) return false; // both must be at the end simultaneously
	return true;
    }


    /******************** SERIALIZATION ********************/

    template< typename InternalCharType__, typename SizeType__ >
    bool
    TransTable< TT_PERFHASH, InternalCharType__, SizeType__ >
    ::loadFromFile( const char* binFile ) {
	FILE * fi;

	std::cerr << "TransTable: Reading " << binFile << " ... " << std::flush;
	fi = fopen( binFile, "rb" );
	if ( !fi ) {
	    throw exceptions::badFileHandle( "Couldn't open file '" +
					     std::string( binFile ) +
					     "' for reading." );
	}

	loadFromStream( fi );
	fclose( fi );
	std::cerr<< "Ok"<<std::endl;
	return true;
    }

    template< typename InternalCharType__, typename SizeType__ >
    void 
    TransTable< TT_PERFHASH, InternalCharType__, SizeType__ >
    ::loadFromStream( FILE* fi ) {
	if( fread( &header_, sizeof( Header ), 1, fi ) != 1 ) {
	  throw exceptions::badDictFile( "csl::TransTable::loadFromStream: Read error while reading header" );
	}

	if ( ( header_.getMagicNumber() != magicNumber_ ) ) {
	    throw exceptions::badDictFile( "TransTable: Magic number comparison failed.\n" );
	}

	/// @todo check for correct dictionary type!!!

	lengthOfSusoStrings_ = header_.getLengthOfSusoStrings();
	nrOfCells_ = header_.getNrOfCells();
	root_ = header_.getRoot();

	cells_ = (Cell_t*) malloc( nrOfCells_ * sizeof( Cell_t ) );
	if( fread( cells_, sizeof( Cell_t ), nrOfCells_, fi ) != nrOfCells_ ) {
	  throw exceptions::badDictFile( "csl::TransTable::loadFromStream: Read error while reading transTable" );	  
	}

	susoStrings_ = (wchar_t*) malloc( lengthOfSusoStrings_ * sizeof( wchar_t ) );
	if( fread( susoStrings_, sizeof( wchar_t ), lengthOfSusoStrings_, fi ) != lengthOfSusoStrings_ ) {
	  throw exceptions::badDictFile( "csl::TransTable::loadFromStream: Read error while reading suso strings" );
	}
	
	sizeOfUsedCells_ = nrOfCells_;
	ready_ = 1;
    }
    

    template< typename InternalCharType__, typename SizeType__ >
    void 
    TransTable< TT_PERFHASH, InternalCharType__, SizeType__ >
    ::createBinary( char* compFile ) {
	writeToFile( compFile );
    }

    template< typename InternalCharType__, typename SizeType__ >
    void 
    TransTable< TT_PERFHASH, InternalCharType__, SizeType__ >
    ::writeToFile( char* compFile ) {
	FILE * fo = fopen( compFile, "wb" );
	if ( !fo ) {
	    std::cerr << "Couldn't open " << compFile << std::endl;
	    exit( 1 );
	}

	std::cerr << "Dumping automaton to " << compFile << " ... ";
	writeToStream( fo );
	fclose( fo );
	std::cerr << "Ok" << std::endl;
    }

    template< typename InternalCharType__, typename SizeType__ >
    void 
    TransTable< TT_PERFHASH, InternalCharType__, SizeType__ >
    ::writeToStream( FILE* fo ) const {
	if ( !fo ) {
	    throw exceptions::badFileHandle( "TransTable: Couldn't write to filehandle " );
	}
	std::cerr << "Writing TransTable"<<std::endl;
	fwrite( &header_, sizeof( Header ), 1, fo );
	fwrite( cells_, sizeof( Cell_t ), sizeOfUsedCells_, fo );
	
	fwrite( susoStrings_, sizeof( wchar_t ), lengthOfSusoStrings_, fo );
    }


    ///////////// DEBUG AND DOT PRINT ///////////////////////////////////////////////

    template< typename InternalCharType__, typename SizeType__ >
    size_t
    TransTable< TT_PERFHASH, InternalCharType__, SizeType__ >
    ::count( StateId_t pos ) const {
	size_t res = 0;
	if( isFinal( pos ) ) ++res;
	const wchar_t* transitions = getSusoString( pos );
	while( *transitions ) {
	    StateId_t nextState = walk( pos, *transitions );
	    res += count( nextState );
	    ++transitions;
	}
	return res;
    }

    template< typename InternalCharType__, typename SizeType__ >
    void
    TransTable< TT_PERFHASH, InternalCharType__, SizeType__ >
    ::printCells() const {
	std::wcout << L"i\ttype\tkey\tvalue" << std::endl << L"--------------" << std::endl;
	for ( size_t i = 0; i < nrOfCells_; ++i ) {
	    std::wcout << i << L"\t" << cells_[i].getType() << L"\t" << (unsigned int)cells_[i].getKey() << L"\t" << cells_[i].getValue() << std::endl;
	}
    }

    template< typename InternalCharType__, typename SizeType__ >
    void
    TransTable< TT_PERFHASH, InternalCharType__, SizeType__ >
    ::toDot() const {
	wprintf( L"Digraph TransTable_out { //DOTCODE\nrankdir=LR;ordering=out; //DOTCODE\n" );
	for ( size_t i = 1; i < sizeOfUsedCells_; ++i ) {
	    if ( cells_[i].isTransition() ) {
		StateId_t base = i - cells_[i].getKey();
		
		wprintf( L"%d->%d[label=\"%lc (%d)\"] //DOTCODE\n", base, cells_[i].getValue(), cells_[i].getKey(), cells_[i].getThirdValue()  );
	    } else if ( cells_[i].isState() ) {
		int peripheries = ( cells_[i].isFinalState() ) ? 2 : 1;
		wprintf( L"%d[peripheries=%d] //DOTCODE\n", i, peripheries );
		wprintf( L"%lu->%lu[label=\"%d,%d\"] //DOTCODE\n", 
			 (unsigned long)i, (unsigned long)i, cells_[i].getValue(), cells_[i].getThirdValue() ) ;
		
	    }
	}
	wprintf( L"} //DOTCODE\n" );
    }

    template< typename InternalCharType__, typename SizeType__ >
    void
    TransTable< TT_PERFHASH, InternalCharType__, SizeType__ >
    ::doAnalysis() const {

	size_t empty = 0;
	size_t states = 0;
	size_t transitions = 0;
	for ( size_t i = 1; i < sizeOfUsedCells_; ++i ) {
	    if ( cells_[i].isEmpty() ) ++empty;
	    else if ( cells_[i].isState() ) ++states;
	    else if ( cells_[i].isTransition() ) ++transitions;
	}

	double emptyRatio = ( double( empty ) / double( sizeOfUsedCells_ ) ) * 100; 
	float cells_MB = (float)( sizeOfUsedCells_ * sizeof( Cell_t ) ) / 1000000;
	float susoStrings_MB = (float)( header_.getLengthOfSusoStrings() * sizeof( wchar_t ) ) / 1000000;

	printf( "**********\nTransTable Analysis\n**********\nCells:\t%zd (%.3f MB)\nStates:\t%zd\nTransitions:\t%zd\nEmpty:\t%zd (%2.2f%%)\nsusoString: %zd chars (%.3f MB)\n\n",
		sizeOfUsedCells_,
		cells_MB,
		states,
		transitions,
		empty,
		emptyRatio,
		header_.getLengthOfSusoStrings(),
		susoStrings_MB
	    );
    }


} // eon

#endif
