#ifndef CSL_TRANSTABLE_CXX
#define CSL_TRANSTABLE_CXX CSL_TRANSTABLE_CXX

#include "./TransTable.h"

namespace csl {

    // TODO
    // update of sizeOfUsedCells_ must know how many special transitions (like AhoCorasick) are present.
    // However, at the moment it is updated to (cell_ + Global::maxNrOfChars + 2),  which is bullshit
    // The same problem exists for compareStates<BASIC>

    template< CellType CellTypeValue >
    inline TransTable< CellTypeValue >::TransTable()
    {
	
	cells_ = 0;
	susoStrings_ = 0;
	susoHash_ = 0;
    }

    template< CellType CellTypeValue >
    inline TransTable< CellTypeValue >::~TransTable() {
	// if( cells_ ) free( cells_ );
	// if( susoStrings_ ) free( susoStrings_ );
	// header ?
    }

    template< CellType CellTypeValue >
    inline const Alphabet& TransTable< CellTypeValue >::getAlphabet() const {
	return alph_;
    }


    template< CellType CellTypeValue >
    inline uint_t TransTable< CellTypeValue >::walk( StateId_t state, char16 c ) const {
	assert( cells_[state].isOfType( Cell_t::STATE ) );
	return ( ( c > 0 ) && cells_[state + c].getKey() == c ) ? cells_[state + c].getValue() : 0;
    }

    template<>
    inline StateId_t TransTable< TOKDIC >::walkPerfHash( StateId_t state, char16 c, size_t& perfHashValue ) const {
	assert( cells_[state].isOfType( Cell_t::STATE ) );
	if( ( c > 0 ) && ( cells_[state + c].getKey() == c ) ) {
	    perfHashValue += cells_[state + c].getAddByte();
	    return cells_[state + c].getValue();
	}
	return 0;
    }
    
    template<>
    inline StateId_t TransTable< TOKDIC >::walkStrPerfHash( StateId_t state, const wchar_t* str, size_t& perfHashValue ) const {
	while( *str && state ) {
	    state = walkPerfHash( state, *str, perfHashValue );
	    ++str;
	}
	return state;
    }

    template<>
    inline bool TransTable< TOKDIC >::getTokID( const wchar_t* key, size_t* tokID ) const {
	*tokID = 0;
	uint_t pos = walkStrPerfHash( getRoot(), key, *tokID );
	return ( ( pos != 0 ) && isFinal( pos ) );
    }

    template< CellType CellTypeValue >
    inline void TransTable< CellTypeValue >::initConstruction() {
	alph_.initConstruction();
	nrOfCells_ = 0;
	allocCells( 80 );
	nrOfStates_ = 0;

	cells_[0].setType( Cell_t::STATE ); // cells_[0] is the failure state
	firstFreeCell_ = 1; // cells_[0] is the failure state
	sizeOfUsedCells_ = 1; // cells_[0] is the failure state

	header_.set( *this );

	susoStrings_ = 0;
	lengthOfSusoStrings_ = 0;

	if ( CellTypeValue == TOKDIC ) {
	    susoHash_ = new Hash<wchar_t>( 100000, susoStrings_, lengthOfSusoStrings_ );
	}
    }

    template< CellType CellTypeValue >
    inline void TransTable< CellTypeValue >::finishConstruction() {
	alph_.finishConstruction();
	nrOfCells_ = sizeOfUsedCells_;
	
	if ( CellTypeValue == TOKDIC ) { // not very nice
	    lengthOfSusoStrings_ = susoHash_->getLengthOfKeyStrings();
	    delete( susoHash_ );
	    susoHash_ = 0;
	}
	else lengthOfSusoStrings_ = 0;

	header_.set( *this );
    }

 

    /**
     * resize the array of cells
     */
    template< CellType CellTypeValue >
    inline void TransTable< CellTypeValue >::allocCells( size_t newNrOfCells ) {
	cells_ = ( Cell_t* ) realloc( cells_, newNrOfCells * sizeof( Cell_t ) );
	memset( ( cells_ + nrOfCells_ ), 0, ( newNrOfCells - nrOfCells_ ) * sizeof( Cell_t ) );
	nrOfCells_ = newNrOfCells;
// std::cout<<"Resized array cells to "<<newNrOfCells<<" cells."<<std::endl;
    }


    /**
     * find slot to store state
     */
    template<>
    inline StateId_t TransTable< BASIC >::findSlot( const TempState& state ) {
	size_t slot = firstFreeCell_ - 1; // is incremented again at beginning of loop
	size_t nrOfAnnotations = state.getNrOfAnnotations();

	bool mightFit = false;
	while ( !mightFit ) {
	    do {
		++slot;
		while ( ( slot + nrOfAnnotations + Global::maxNrOfChars + 1 ) > nrOfCells_ ) {
		    allocCells( nrOfCells_ * 2 );
		}
	    } while ( ! ( cells_[slot].isEmpty() ) );

	    mightFit = true;

	    // check if cells for annotations are available;
	    // increment slot with the loop, so that slot points to the cell that gets type STATE
	    for ( size_t i = 1; ( mightFit && i < nrOfAnnotations ) ;++i ) {
		if ( !cells_[++slot].isEmpty() )
		    mightFit = false;
	    }

	    // check if all required cells for transitions are available
	    for ( TempState::ConstIterator it = state.getConstIterator(); mightFit && it.isValid(); ++it ) {
		if ( !cells_[slot + it->getLabel()].isEmpty() ) {
		    mightFit = false;
		}
	    }
	}
	return (StateId_t) slot;
    }

    /**
     * find slot to store state
     */
    template<>
    inline StateId_t TransTable< TOKDIC >::findSlot( const TempState& state ) {
	size_t slot = firstFreeCell_ - 1; // is incremented again at beginning of loop
	size_t nrOfAnnotations = state.getNrOfAnnotations();

	bool mightFit = false;
	while ( !mightFit ) {
	    do {
		++slot;
		while ( ( slot + nrOfAnnotations + Global::maxNrOfChars + 1 ) > nrOfCells_ ) {
		    allocCells( nrOfCells_ * 2 );
		}
	    } while ( ! ( cells_[slot].isEmpty() ) );

	    mightFit = true;

	    // check if cells for annotations are available;
	    // increment slot with the loop, so that slot points to the cell that gets type STATE
	    for ( size_t i = 1; ( mightFit && i < nrOfAnnotations ) ;++i ) {
		if ( !cells_[++slot].isEmpty() )
		    mightFit = false;
	    }
	    
	    // check if all required cells for transitions are available
		for ( TempState::TransitionConstIterator it = state.transitionsBegin();
			mightFit && ( it != state.transitionsEnd() ) ; ++it ) {
		if ( !cells_[slot + it->getLabel()].isEmpty() ) {
		    mightFit = false;
		}
	    }
	}
	return (StateId_t)slot;
    }

    template< CellType CellTypeValue >
    inline StateId_t TransTable< CellTypeValue >::storeTempState( TempState& state ) {
	return 0; // dummy
    }

    template<>
    inline StateId_t TransTable< BASIC >::storeTempState( TempState& state ) {
	++nrOfStates_;



	size_t nrOfAnnotations = state.getNrOfAnnotations();

	size_t slot = findSlot( state );
	size_t i;


	// mark stateCell and insert the first annotation
	cells_[slot].setType( Cell_t::STATE );
	if ( nrOfAnnotations > 0 ) {
	    cells_[slot].setType( Cell_t::HAS_ANN );
	    cells_[slot].setValue( state.getAnnotation( 0 ) );
	}

	// set finalState
	if ( state.isFinal() ) {
	    cells_[slot].setFinal( true );
	}

	// insert further annotations
	for ( i = 1; i < nrOfAnnotations ;++i ) {
	    cells_[slot - i].setType( Cell_t::IS_ANN );
	    cells_[slot - i].setValue( state.getAnnotation( i ) );
	}

	// insert all transitions
	for ( TempState::ConstIterator it = state.getConstIterator(); it.isValid() ; ++it ) {
	    alph_.addChar( it->getLabel() );
	    cells_[slot + (*it).getLabel()].setTrans( (*it).getLabel(), (*it).getTarget() );
	}

	// update nrOfUsedCells
	sizeOfUsedCells_ = std::max( sizeOfUsedCells_, ( slot + Global::maxNrOfChars + 2 ) );

	// the following lines are to prevent firstFreeCell_ to get stuck in a lower area, where none of the
	// new states can never fit.
	if( ( slot - firstFreeCell_ ) > searchWindow ) {
	    // std::wcerr << "ffc hack: " << firstFreeCell_ << std::endl;
	    firstFreeCell_ = slot - searchWindow; // a slot which is really empty is searched right below
	}


	//update firstFreeCell
	for ( ;! ( cells_[firstFreeCell_].isEmpty() );++firstFreeCell_ );


	return (StateId_t)slot;
    }



    template<>
    inline StateId_t TransTable< TOKDIC >::storeTempState( TempState& state ) {
	++nrOfStates_;

	// add offset for susoString as value
	size_t susoPtr = susoHash_->findOrInsert( state.getSusoString() );
	state.addAnnotation( (int)susoPtr );
	// std::wcout<<"Insert: "<<state.getSusoString()<<" at "<<susoPtr<<std::endl;

	uint_t slot = findSlot( state );

	// mark stateCell and insert the only annotation
	cells_[slot].setType( Cell_t::STATE );
	cells_[slot].setType( Cell_t::HAS_ANN );
	cells_[slot].setValue( state.getAnnotation( 0 ) );

	// set finalState
	if ( state.isFinal() ) {
	    cells_[slot].setFinal( true );
	}

	// insert all transitions
	for ( TempState::TransitionConstIterator it = state.transitionsBegin();
		it != state.transitionsEnd() ; 
		++it ) {
	    alph_.addChar( it->getLabel() );
	    cells_[slot + it->getLabel()].setTrans( it->getLabel(), it->getTarget(), (int)it->getPhNumber() );
	}
	
	// update sizeOfUsedCells_
	sizeOfUsedCells_ = std::max( sizeOfUsedCells_, ( slot + Global::maxNrOfChars + 2 ) );

	// the following lines are to prevent firstFreeCell_ to get stuck in a lower area, where none of the
	// new states can never fit.
	if( ( slot - firstFreeCell_ ) > searchWindow ) {
	    // std::wcerr << "ffc hack: " << firstFreeCell_ << std::endl;
	    firstFreeCell_ = slot - searchWindow; // a slot which is really empty is searched right below
	}

	//update firstFreeCell
	for ( ; ! ( cells_[firstFreeCell_].isEmpty() ); ++firstFreeCell_ );

	return slot;
    }

    template<>
    inline bool TransTable< TOKDIC >::compareStates( const TempState& temp, StateId_t comp ) const {

		if ( temp.isFinal() != isFinal( comp ) ) return false; // both must be either final or not

		const wchar_t* c_comp = getSusoString( comp );
	TempState::TransitionConstIterator tempIt = temp.transitionsBegin();
	while( ( tempIt != temp.transitionsEnd() ) && *c_comp ) {

	    if( tempIt->getLabel() != *c_comp ) return false; // both must have the same char as next label
	    if ( tempIt->getTarget() != walk( comp, *c_comp ) ) return false; // both must point to the same state
	    ++tempIt;
	    ++c_comp;
	}
	if( ( tempIt != temp.transitionsEnd() ) || ( *c_comp != 0 )) return false; // both must be at the end simultaneously
	return true;
    }

    /**
     * compare a temporary state to a compressed one.
     * return iff both are equivalent w.r.t their outgoing transitions
     */
    template< CellType CellTypeValue  >
    inline bool TransTable< CellTypeValue >::compareStates( const TempState& temp, StateId_t comp ) const {
	if ( temp.isFinal() != isFinal( comp ) ) return false;

	TempState::TransitionConstIterator tempIt = temp.transitionsBegin();
	wchar_t compLabel = 1;
	while( tempIt != temp.transitionsEnd() ) {
	    while( compLabel < tempIt->getLabel() ) { 
		if( walk( comp, compLabel ) ) return false; // chars without a label in temp must be 0
		++compLabel;
	    }
	    if ( tempIt->getTarget() != walk( comp, compLabel ) ) return false;
	}
	while( compLabel < Global::maxNrOfChars + 2 ) { // UHHAAA
		if( walk( comp, compLabel ) ) return false; // chars without a label in temp must be 0
		++compLabel;
	}
	return true;
    }



    ////////// FILE DUMP FUNCTIONALITY /////////////////////////////////////

    template< CellType CellTypeValue >
    inline bool TransTable< CellTypeValue >::loadFromFile( const char* binFile ) {
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

    template< CellType CellTypeValue >
    inline void TransTable< CellTypeValue >::loadFromStream( FILE* fi ) {
	fread( &header_, sizeof( Header ), 1, fi );

	if ( ( header_.getMagicNumber() != magicNumber_ ) ) {
	    throw exceptions::badDictFile( "TransTable: Magic number comparison failed.\n" );
	}

	if ( header_.getCType() != CellTypeValue ) {
	    std::cerr << header_.getCType() << "<->" << CellTypeValue << std::endl;
	    throw exceptions::badDictFile( "csl::TransTable - File is incompatible to dictionary type.\n" );
	}

	lengthOfSusoStrings_ = header_.getLengthOfSusoStrings();
	nrOfCells_ = header_.getNrOfCells();
	root_ = header_.getRoot();

			

	alph_.loadFromStream( fi );
	
	cells_ = (Cell_t*) malloc( nrOfCells_ * sizeof( Cell_t ) );
	fread( cells_, sizeof( Cell_t ), nrOfCells_, fi );

	susoStrings_ = (wchar_t*) malloc( lengthOfSusoStrings_ * sizeof( wchar_t ) );
	fread( susoStrings_, sizeof( wchar_t ), lengthOfSusoStrings_, fi );
	
	sizeOfUsedCells_ = nrOfCells_;
    }
    

    template< CellType CellTypeValue  >
    inline void TransTable< CellTypeValue >::createBinary( char const* compFile ) {
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

    template< CellType CellTypeValue  >
    inline void TransTable< CellTypeValue >::writeToStream( FILE* fo ) const {
	if ( !fo ) {
	    throw exceptions::badFileHandle( "TransTable: Couldn't write to filehandle " );
	}
	std::wcerr << "Writing TransTable"<<std::endl;
	fwrite( &header_, sizeof( Header ), 1, fo );
	alph_.writeToStream( fo );
	fwrite( cells_, sizeof( Cell_t ), sizeOfUsedCells_, fo );
	
	fwrite( susoStrings_, sizeof( wchar_t ), lengthOfSusoStrings_, fo );
    }

    ///////////// DEBUG AND DOT PRINT ///////////////////////////////////////////////

    template<>
    inline size_t TransTable< TOKDIC >::count( StateId_t pos ) const {
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

    template< CellType CellTypeValue  >
    inline void TransTable< CellTypeValue >::printCells() const {
	std::wcout << L"i\ttype\tkey\tvalue" << std::endl << L"--------------" << std::endl;
	for ( size_t i = 0; i < nrOfCells_; ++i ) {
	    std::wcout << i << L"\t" << cells_[i].getType() << L"\t" << (unsigned int)cells_[i].getKey() << L"\t" << cells_[i].getValue() << std::endl;
	}
    }

    template< CellType CellTypeValue >
    inline void TransTable< CellTypeValue >::toDot() const {
	Cell_t * cellArray = TransTable::getCells();
	std::wcout<<"Digraph TransTable_out { //DOTCODE\nrankdir=LR; //DOTCODE"<<std::endl;
	for ( size_t i = 1; i < sizeOfUsedCells_; ++i ) {
	    if ( cellArray[i].isOfType( Cell_t::TRANS ) ) {
		StateId_t base = i - cellArray[i].getKey();

		std::wcout<<
		    base<<
		    "->"<<
		    cellArray[i].getValue()<<
		    "[label=\""<<
		    cellArray[i].getKey()<<
		    // "("<<cellArray[i].getAddByte()<<")" <<
		    "\"] //DOTCODE"<<
		    std::endl;
	    } else if ( cellArray[i].isOfType( Cell_t::STATE ) ) {
		int peripheries = ( cellArray[i].isOfType( Cell_t::FINALSTATE ) ) ? 2 : 1;
		std::wcout<<
			i<<
			"[peripheries="<<peripheries<<"] //DOTCODE"<<
			std::endl;

		if ( cellArray[i].isOfType( Cell_t::HAS_ANN ) ) {
			std::wcout<< i << "->" << i << "[";

		    AnnIterator it( *this, i );
		    while ( it.isValid() ) {
				std::wcout << *it << ",";
				++it;
		    }
			std::wcout << "]\",fontcolor=blue,style=dotted,dir=none] //DOTCODE" << std::endl;
		}
	    }
	}
	std::wcout<< "} //DOTCODE" << std::endl;
    }

    template<>
    inline void TransTable< TOKDIC >::toDot() const {
	Cell_t * cellArray = getCells();
	std::wcout << 
		"Digraph TransTable_out { //DOTCODE" << std::endl <<
		"rankdir=LR; //DOTCODE" << std::endl <<
		"ordering=out; //DOTCODE" << std::endl;

	for ( size_t i = 1; i < sizeOfUsedCells_; ++i ) {
	    if ( cellArray[i].isOfType( Cell_t::TRANS ) ) {
		StateId_t base = (StateId_t)( i - cellArray[i].getKey() );
		std::wcout<<
		    base<<
		    "->"<<
		    cellArray[i].getValue()<<
		    "[label=\""<<
		    cellArray[i].getKey()<<
		    // "("<<cellArray[i].getAddByte()<<")"<<
		    "\"] //DOTCODE"<<
		    std::endl;

	    } else if ( cellArray[i].isOfType( Cell_t::STATE ) ) {
		int peripheries = ( cellArray[i].isOfType( Cell_t::FINALSTATE ) ) ? 2 : 1;
		std::wcout<<
			i<<
			"[peripheries="<<peripheries<<"] //DOTCODE"<<
			std::endl;
		
		if ( cellArray[i].isOfType( Cell_t::HAS_ANN ) ) {}
	    }
	}
	std::wcout << "} //DOTCODE\n" << std::endl;
    }

    template<>
    inline void TransTable< TOKDIC >::doAnalysis() const {
	Cell_t * cellArray = getCells();

	size_t empty = 0;
	size_t states = 0;
	size_t transitions = 0;
	size_t annotations = 0;
	for ( size_t i = 1; i < sizeOfUsedCells_; ++i ) {
	    if ( cellArray[i].isEmpty() ) ++empty;
	    else if ( cellArray[i].isOfType( Cell_t::STATE ) ) ++states;
	    else if ( cellArray[i].isOfType( Cell_t::TRANS ) ) ++transitions;
	    else if ( cellArray[i].isOfType( Cell_t::IS_ANN ) ) ++annotations;
	}

	double emptyRatio = ( double( empty ) / double( sizeOfUsedCells_ ) ) * 100; 
	float cells_MB = (float)( sizeOfUsedCells_ * sizeof( Cell_t ) ) / 1000000;
	float susoStrings_MB = (float)( header_.getLengthOfSusoStrings() * sizeof( wchar_t ) ) / 1000000;

	printf( "**********\nTransTable Analysis\n**********\nCells:\t%zd (%.3f MB)\nStates:\t%zd\nTransitions:\t%zd\nAnnotations:\t%zd\nEmpty:\t%zd (%2.2f%%)\nsusoString: %zd chars (%.3f MB)\n\n",
		sizeOfUsedCells_,
		cells_MB,
		states,
		transitions,
		annotations,
		empty,
		emptyRatio,
		header_.getLengthOfSusoStrings(),
		susoStrings_MB
	    );
    }


} // eon

#endif
