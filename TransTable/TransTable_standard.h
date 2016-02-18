#ifndef CSL_TRANSTABLE_STANDARD_H
#define CSL_TRANSTABLE_STANDARD_H CSL_TRANSTABLE_STANDARD_H

#include "../Global.h"
#include<cstdlib>

// forward declaration needed for the other help classes
namespace csl {
    template< typename InternalCharType__, typename SizeType__ >
    class TransTable< TT_STANDARD, InternalCharType__, SizeType__ >;
}

#include "./Cell.h"
#include "./TempState.h"
#include "../Hash/Hash.h"

namespace csl {
    template< typename InternalCharType__, typename SizeType__ >
    class TransTable< TT_STANDARD, InternalCharType__, SizeType__ > {
    public:
	typedef InternalCharType__ InternalChar_t ;
	typedef SizeType__ InternalSize_t ;

	/**
	 * StateId_t should be 8 bytes whenever 
	 * SizeType__ is 8 bytes
	 */
	typedef SizeType__ StateId_t;

	typedef TransTable< TT_STANDARD, InternalChar_t, InternalSize_t > TransTable_t;
	typedef Cell< TT_STANDARD, InternalChar_t, InternalSize_t > Cell_t;
	typedef TempState< TransTable_t > TempState_t;
	
	
	/**
	 *
	 */
	inline TransTable();
	
	/**
	 * Destructor
	 */
	inline virtual ~TransTable();


	virtual bool readyToRead() const;
	virtual bool readyToWrite() const;

	/**
	 * Loads an automaton from a binary file. (usually .dic or .trie or similar)
	 * @param binFile filename of the binary storing the automaton
	 * @throw exceptions::badFileHandle
	 */
	inline bool loadFromFile( const char* binFile );

	inline void loadFromStream( FILE* fi );

	/**
	 * Dumps the automaton into a file
	 * @param binFile filename to dump the automaton into (usually .dic)
	 */
	inline void createBinary( char const * binFile );

	/**
	 * 
	 */
	void writeToStream( FILE* fo ) const;


	/**
	 * Call this method before adding the first state to the automaton
	 */
	inline void initConstruction();

	/**
	 * Call this method to finish the construction phase
	 */
	inline void finishConstruction();

	/**
	 * inserts a new state into the table, returns the state id.
	 *
	 * @param state a TempState-object
	 * @returns the state id
	 */
	inline StateId_t storeTempState( TempState_t& state );

	/**
	 * declare a root (or start state) of the automaton.
	 */
	inline void setRoot( StateId_t rootId );
	
	/// returns the root
	inline StateId_t getRoot() const;

	/**
	 * returns true iff state is marked as final
	 * @param state id of a state
	 */
	inline bool isFinal( StateId_t state ) const;

	inline void setFinal( StateId_t state, bool b = true );


	/**
	 * perform one step inside the automaton
	 * @param state the state to start from
	 * @param c the character to walk with
	 * @return the state id reached by the transition. 0, if the transition does not exist
	 */
	inline uint_t walk( StateId_t state, char16 c ) const;

	/**
	 * 
	 */
	inline uint_t walkStr( StateId_t state, const wchar_t* str ) const;

	/**
	 * perform one step inside the automaton and keep track of the perfect hashing value
	 * Implemented for TOKDIC only
	 * @param state the state to start from
	 * @param c the character to walk with (c already being coded according to custom alphabet)
	 * @param perfHashValue the perfHash value of the current traansition is ADDED to this variable.
	 *        If transition does not exist, the perfHashValue is left untouched
	 */
	inline StateId_t walkPerfHash( StateId_t state, char16 c, size_t& perfHashValue ) const;

	inline StateId_t walkStrPerfHash( StateId_t state, const wchar_t* str, size_t& perfHashValue ) const;

	inline InternalSize_t getAnnotationAtState( StateId_t stateId ) const {
	    if( stateId >= getNrOfCells() ) {
		throw exceptions::outOfRange( "csl::TransTable_standard::getAnnotationAtState: invalid cell index" );
	    }
	    return cells_[ stateId ].getValue(); 
	}

	inline Cell_t* getCells() {
	    return cells_;
	}
	inline size_t getNrOfCells() const {
	    return nrOfCells_;
	}

	inline bool compareStates( const TempState_t& temp, StateId_t comp ) const;

	inline size_t count( StateId_t pos ) const;

	inline void printCells() const;
	inline void toDot() const;
	inline void doAnalysis() const;


    protected:

	inline size_t getSizeOfUsedCells() const {
	    return sizeOfUsedCells_;
	}

	inline size_t getNrOfStates() const {
	    return nrOfStates_;
	}

    private:

	/**
	 * @return the string containing labels of all existing outgoing transitions from state
	 *
	 * This method is private here because suso-strings are avaliable ONLY DURING CONSTRUCTION
	 * of a new dictionary.
       	 */
	inline const wchar_t* getSusoString( StateId_t state ) const;

	/**
	 * This number specifies the maximum size of the area that is considered when
	 * looking for a slot. Free cells 
	 */
	static const size_t searchWindow = 1000;

	Cell_t* cells_;
	size_t nrOfCells_;

	/**
	 * @brief indicate the object's state
	 *
	 * 0: nothing is initialised
	 * 1: ready to read
	 * 2: ready to write
	 */
	int ready_;

	wchar_t* susoStrings_;

	// this is the number of characters in susoStrings_ (not the size in bytes!)
	size_t lengthOfSusoStrings_;
	Hash< wchar_t >* susoHash_;
	std::vector< size_t >* susoIndex_;

	static const bits64 magicNumber_ = 345672461;

	/**
	 * This class represents the header information for a TransTable. An object of this kind
	 * is dumped to/ read from the stream with fread/fwrite. The policy is to have all that
	 * information in separate variables in TransTable and to copy the values to and from the header
	 * just for dumping and loading
	 */
	class Header {
	public:
	    Header() :
		magicNumber_( 0 ),
		nrOfCells_( 0 ),
		root_( 0 ) {
	    }

	    bits64 getMagicNumber() const {
		return magicNumber_;
	    }

	    StateId_t getRoot() const {
		return static_cast< StateId_t >( root_ );
	    }

	    size_t getNrOfCells() const {
			return static_cast< size_t >( nrOfCells_ );
	    }

	    void set( const TransTable_t& transTable ) {
		magicNumber_ = transTable.magicNumber_;
		nrOfCells_ = transTable.nrOfCells_;
		root_ = transTable.root_;
	    }
	    

	private:
	    bits64 magicNumber_;
	    bits64 nrOfCells_;
	    bits64 root_;
	}; // class Header

	Header header_;

	StateId_t root_;
	

	/**
	 * The number of states. Note that this value is updated during construction
	 * only. If a table is loaded from a binary, this variable remains set to -1.
	*/
	size_t nrOfStates_;


	/// the first free cell of the array
	size_t firstFreeCell_;
	size_t sizeOfUsedCells_;


	/**
	 * resize the array of cells
	 */
	inline void allocCells( size_t newNrOfCells );

	inline StateId_t findSlot( const TempState_t& state );


  };

} // eon

#include "./TransTable_standard.tcc"


#endif
