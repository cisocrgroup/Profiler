#ifndef CSL_TRANSTABLE_STANDARD_TCC
#define CSL_TRANSTABLE_STANDARD_TCC CSL_TRANSTABLE_STANDARD_TCC


namespace csl {

    template< typename InternalCharType__, typename SizeType__ >
    TransTable< TT_STANDARD, InternalCharType__, SizeType__ >
    ::TransTable()
    {
	cells_ = 0;
	susoStrings_ = 0;
	susoHash_ = 0;
	susoIndex_ = 0;

	std::wcerr<<"TransTable_standard: cellsize="<< sizeof( Cell_t ) <<" bytes"<< std::endl;

    }

    template< typename InternalCharType__, typename SizeType__ >
    TransTable< TT_STANDARD, InternalCharType__, SizeType__ >
    ::~TransTable() {
	if( cells_ ) free( cells_ );
	if( susoStrings_ ) free( susoStrings_ );
    }

    template< typename InternalCharType__, typename SizeType__ >
    bool
    TransTable< TT_STANDARD, InternalCharType__, SizeType__ >
    ::readyToRead() const {
	return ( ready_ == 1 );
    }

    template< typename InternalCharType__, typename SizeType__ >
    bool
    TransTable< TT_STANDARD, InternalCharType__, SizeType__ >
    ::readyToWrite() const {
	return ( ready_ == 2 );
    }


    template< typename InternalCharType__, typename SizeType__ >
    uint_t
    TransTable< TT_STANDARD, InternalCharType__, SizeType__ >
    ::walk( StateId_t state, char16 c ) const {
	assert( cells_[state].isState() );
	return ( ( c > 0 ) && cells_[state + c].getKey() == c ) ? cells_[state + c].getValue() : 0;
    }

    template< typename InternalCharType__, typename SizeType__ >
    typename TransTable< TT_STANDARD, InternalCharType__, SizeType__ >::StateId_t
    TransTable< TT_STANDARD, InternalCharType__, SizeType__ >
    ::getRoot() const {
	return root_;
    }

    template< typename InternalCharType__, typename SizeType__ >
    bool
    TransTable< TT_STANDARD, InternalCharType__, SizeType__ >
    ::isFinal( StateId_t state ) const {
	return cells_[state].isFinal();
    }

    template< typename InternalCharType__, typename SizeType__ >
    void TransTable< TT_STANDARD, InternalCharType__, SizeType__ >
    ::initConstruction() {

	susoStrings_ = 0;
	lengthOfSusoStrings_ = 0;

	susoHash_ = new Hash< wchar_t >( 100000, susoStrings_, lengthOfSusoStrings_ );
	susoIndex_ = new std::vector< size_t >;

	nrOfCells_ = 0;
	allocCells( 80 );
	nrOfStates_ = 0;


	cells_[0].setState(); // cells_[0] is the failure state
	firstFreeCell_ = 1; // cells_[0] is the failure state
	sizeOfUsedCells_ = 1; // cells_[0] is the failure state

	header_.set( *this );
	ready_ = 2;
    }

    template< typename InternalCharType__, typename SizeType__ >
    void TransTable< TT_STANDARD, InternalCharType__, SizeType__ >::finishConstruction() {
	nrOfCells_ = sizeOfUsedCells_;
	
	lengthOfSusoStrings_ = susoHash_->getLengthOfKeyStrings();
	delete( susoHash_ );
	susoHash_ = 0;

	delete( susoStrings_ );
	susoStrings_ = 0;

	delete( susoIndex_ );
	susoIndex_ = 0;

	header_.set( *this );
	ready_ = 1;
    }

    /**
     * resize the array of cells
     */
    template< typename InternalCharType__, typename SizeType__ >
    void TransTable< TT_STANDARD, InternalCharType__, SizeType__ >
    ::allocCells( size_t newNrOfCells ) {

	cells_ = ( Cell_t* ) realloc( cells_, newNrOfCells * sizeof( Cell_t ) );
	memset( ( cells_ + nrOfCells_ ), 0, ( newNrOfCells - nrOfCells_ ) * sizeof( Cell_t ) );
	nrOfCells_ = newNrOfCells;

	susoIndex_->resize( newNrOfCells, 0 );
	// std::cout<<"Resized array cells to "<<newNrOfCells<<" cells."<<std::endl;
    }

    template< typename InternalCharType__, typename SizeType__ >
    wchar_t const*
    TransTable< TT_STANDARD, InternalCharType__, SizeType__ >
    ::getSusoString( StateId_t state ) const {
	if( susoStrings_ == 0 ) {
	    throw exceptions::LogicalError( "csl::TransTable_standard::getSusoString: called while not in construction phase" );
	}
	return ( susoStrings_ + susoIndex_->at( state ) );
    }

    template< typename InternalCharType__, typename SizeType__ >
    void
    TransTable< TT_STANDARD, InternalCharType__, SizeType__ >
    ::setRoot( StateId_t root ) {
	root_ = root;
    }

    /**
     * find slot to store state
     */
    template< typename InternalCharType__, typename SizeType__ >
    typename TransTable< TT_STANDARD, InternalCharType__, SizeType__ >::StateId_t
    TransTable< TT_STANDARD, InternalCharType__, SizeType__ >
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
    typename TransTable< TT_STANDARD, InternalCharType__, SizeType__ >::StateId_t
    TransTable< TT_STANDARD, InternalCharType__, SizeType__ >
    ::storeTempState( TempState_t& state ) {

	++nrOfStates_;

	uint_t slot = findSlot( state );

	// insert susoString to temporary (!) susoStrings
	size_t susoPtr = susoHash_->findOrInsert( state.getSusoString() );
	susoIndex_->at( slot ) = susoPtr;

	// mark stateCell and insert the only annotation
	cells_[slot].setState();
	cells_[slot].setValue( state.getAnnotation() );

	// set finalState
	if ( state.isFinal() ) {
	    cells_[slot].setFinal();
	}

	// insert all transitions
	for ( typename TempState< TransTable_t >::const_TransIterator it = state.transBegin(); 
		  it != state.transEnd() ; 
		  ++it ) {
	    cells_[slot + it->getLabel()].setTrans( it->getLabel(), it->getTarget() );
	}
	
	// update sizeOfUsedCells_
	sizeOfUsedCells_ = std::max( sizeOfUsedCells_, ( slot + Global::maxNrOfChars + 2 ) );

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
    bool
    TransTable< TT_STANDARD, InternalCharType__, SizeType__ >
    ::compareStates( const TempState_t& temp, StateId_t comp ) const {

	if ( temp.isFinal() != isFinal( comp ) ) return false; // both must be either final or not
	if ( temp.getAnnotation() != cells_[comp].getValue() ) return false; // both must have the same annotation

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



    ////////// FILE DUMP FUNCTIONALITY /////////////////////////////////////

    template< typename InternalCharType__, typename SizeType__ >
    bool
    TransTable< TT_STANDARD, InternalCharType__, SizeType__ >
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
    TransTable< TT_STANDARD, InternalCharType__, SizeType__ >
    ::loadFromStream( FILE* fi ) {
	size_t elementsRead = fread( &header_, sizeof( Header ), 1, fi );
	if( elementsRead != 1 ) {
	    throw exceptions::badInput( "csl::TransTable< TT_STANDARD >::loadFromStream: could not read Header" );
	}


	if ( ( header_.getMagicNumber() != magicNumber_ ) ) {
	    throw exceptions::badDictFile( "TransTable: Magic number comparison failed.\n" );
	}

	/// @todo check for correct dictionary type!!!

	nrOfCells_ = header_.getNrOfCells();
	root_ = header_.getRoot();

	cells_ = (Cell_t*) malloc( nrOfCells_ * sizeof( Cell_t ) );
	elementsRead = fread( cells_, sizeof( Cell_t ), nrOfCells_, fi );
	if( elementsRead != nrOfCells_ ) {
	    throw exceptions::badInput( "csl::TransTable< TT_STANDARD >::loadFromStream: could not read cells." );
	}

	sizeOfUsedCells_ = nrOfCells_;
    }
    

    template< typename InternalCharType__, typename SizeType__ >
    void 
    TransTable< TT_STANDARD, InternalCharType__, SizeType__ >
    ::createBinary( char const* compFile ) {
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
    TransTable< TT_STANDARD, InternalCharType__, SizeType__ >
    ::writeToStream( FILE* fo ) const {
	if ( !fo ) {
	    throw exceptions::badFileHandle( "TransTable: Couldn't write to filehandle " );
	}
	std::cerr << "Writing TransTable"<<std::endl;
	fwrite( &header_, sizeof( Header ), 1, fo );
	fwrite( cells_, sizeof( Cell_t ), sizeOfUsedCells_, fo );
	
    }

    ///////////// DEBUG AND DOT PRINT ///////////////////////////////////////////////

    template< typename InternalCharType__, typename SizeType__ >
    size_t
    TransTable< TT_STANDARD, InternalCharType__, SizeType__ >
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
    TransTable< TT_STANDARD, InternalCharType__, SizeType__ >
    ::printCells() const {
	std::wcout << L"i\ttype\tkey\tvalue" << std::endl << L"--------------" << std::endl;
	for ( size_t i = 0; i < nrOfCells_ && i < 10; ++i ) {
	    std::wcout << i << L"\t" << cells_[i].getType() << L"\t" << (unsigned int)cells_[i].getKey() << L"\t" << cells_[i].getValue() << std::endl;
	}
    }

    template< typename InternalCharType__, typename SizeType__ >
    void
    TransTable< TT_STANDARD, InternalCharType__, SizeType__ >
    ::toDot() const {
	std::wcout<<"Digraph TransTable_out { //DOTCODE\nrankdir=LR; //DOTCODE\n"<<std::endl;
	for ( size_t i = 1; i < sizeOfUsedCells_; ++i ) {
	    if ( cells_[i].isTransition() ) {
		StateId_t base = i - cells_[i].getKey();

		wprintf( L"%d->%d[label=\"%lc\"]; //DOTCODE\n", base, cells_[i].getValue(), cells_[i].getKey() );
	    } else if ( cells_[i].isState() ) {
		int peripheries = ( cells_[i].isFinal() ) ? 2 : 1;
		wprintf( L"%d[peripheries=%d]; //DOTCODE\n", i, peripheries );

	    }
	}
	wprintf( L"} //DOTCODE\n" );
    }

    template< typename InternalCharType__, typename SizeType__ >
    void
    TransTable< TT_STANDARD, InternalCharType__, SizeType__ >
    ::doAnalysis() const {

	size_t empty = 0;
	size_t states = 0;
	size_t transitions = 0;
	size_t annotations = 0;
	for ( size_t i = 1; i < sizeOfUsedCells_; ++i ) {
	    if ( cells_[i].isEmpty() ) ++empty;
	    else if ( cells_[i].isState() ) ++states;
	    else if ( cells_[i].isTransition() ) ++transitions;
	}

	double emptyRatio = ( double( empty ) / double( sizeOfUsedCells_ ) ) * 100; 
	float cells_MB = (float)( sizeOfUsedCells_ * sizeof( Cell_t ) ) / 1000000;

	printf( "**********\nTransTable Analysis\n**********\nCells:\t%zd (%.3f MB)\nStates:\t%zd\nTransitions:\t%zd\nAnnotations:\t%zd\nEmpty:\t%zd (%2.2f%%)\n\n",
		sizeOfUsedCells_,
		cells_MB,
		states,
		transitions,
		annotations,
		empty,
		emptyRatio
	    );
    }


} // eon

#endif
