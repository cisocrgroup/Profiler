#ifndef CSL_TRANSTABLE_PERFHASH_H
#define CSL_TRANSTABLE_PERFHASH_H CSL_TRANSTABLE_PERFHASH_H


// forward declaration needed for the other help classes
namespace csl {

    template< typename InternalCharType__, typename SizeType__ >
    class TransTable< TT_PERFHASH, InternalCharType__, SizeType__ >;
}

#include<algorithm>
#include "./Cell.h"
#include "./TempState.h"
#include "../Hash/Hash.h"

namespace csl {

    template< typename InternalCharType__, typename SizeType__ >
    class TransTable< TT_PERFHASH, InternalCharType__, SizeType__ > {
    public:

	/******************** TYPEDEFS ********************/

	/**
	 * @brief This is the datatype used for storing transition labels in the TransTable
	 *
	 * Usually it is an unsigned type of 1-4 bytes (actually it's not really tested with anything but 2 bytes)
	 */
	typedef InternalCharType__ InternalChar_t ;

	/**
	 * @brief This is the type used for offsets in the sparse table
	 *
	 * Often it makes sense to use a 4byte type also on a 64bit machine to get portable binary files
	 * and/or to get a smaller representation
	 */
	typedef SizeType__ InternalSize_t ;

	/**
	 * StateId_t should be 8 bytes whenever 
	 * InternalSize_t is 8 bytes
	 */
	typedef InternalSize_t StateId_t;

	typedef TransTable< TT_PERFHASH, InternalChar_t, InternalSize_t > TransTable_t;
	typedef Cell< TT_PERFHASH, InternalChar_t, InternalSize_t > Cell_t;
	typedef TempState< TransTable_t > TempState_t;


	/**
	 * This class provides a much easier interface to the dic if the guts of the automaton
	 * have to be explored state- and transition-wise.
	 */
	class State {
	public:
	    /**
	     * @param a reference to the \c MinDic the \c State belongs to.
	     */
	    State( const TransTable_t& tt ) :
		transTable_( &tt ),
		dicPos_( transTable_->getRoot() ),
		perfHashValue_( 0 ) {

		//std::wcerr << "Create state on Automaton " << &tt << (tt.readyToRead()?L"yo": L"no") <<  std::endl;
	    }
	    
	    /**
	     * 
	     */
	    bool walk( wchar_t c ) {
		dicPos_ = transTable_->walkPerfHash( dicPos_, c, &perfHashValue_ );
		return isValid();
	    }

	    bool isValid() {
		return ( dicPos_ != 0 );
	    }

	    bool hasTransition( wchar_t c ) const {
		return transTable_->walk( dicPos_, c );
	    }

	    State getTransTarget( wchar_t c ) const {
		size_t tmpPHValue = perfHashValue_;
		StateId_t newPos = transTable_->walkPerfHash( dicPos_, c, &tmpPHValue );
		return State( *transTable_, newPos, tmpPHValue );
	    }

	    const wchar_t* getSusoString() const {
		return transTable_->getSusoString( dicPos_ );
	    }

	    size_t getPerfHashValue() const {
		return perfHashValue_;
	    }

	    StateId_t getStateID() const {
		return dicPos_;
	    }

	    int getAnnotation() const {
		return -1;
	    }

	    bool isFinal() const {
		return transTable_->isFinal( dicPos_ );
	    }
	    
	private:

	    State( const TransTable_t& transTable, StateId_t dicPos, size_t perfHashValue ) :
		transTable_( &transTable ),
		dicPos_( dicPos ),
		perfHashValue_( perfHashValue ) {
	    }

	    const TransTable_t* transTable_;
	    StateId_t dicPos_;
	    size_t perfHashValue_;
	}; // class State

	
	/******************** CONSTRUCTORS / DESTRUCTOR ********************/

	/**
	 *
	 */
	inline TransTable();

	/**
	 * destructor
	 */
	inline virtual ~TransTable();


	virtual bool readyToRead() const;
	virtual bool readyToWrite() const;

	/****************** BASIC ACCESS TO THE TRANSITION TABLE **********************/
	
	/**
	 * @brief perform one step inside the automaton
	 * @param state the state to start from
	 * @param c the character to walk with
	 * @return the state id reached by the transition. 0, if the transition does not exist
	 */
	inline StateId_t walk( StateId_t state, wchar_t c ) const;

	/**
	 * 
	 */
	inline StateId_t walkStr( StateId_t state, const wchar_t* str ) const;

	/**
	 * perform one step inside the automaton and keep track of the perfect hashing value
	 * Implemented for TOKDIC only
	 * @param state the state to start from
	 * @param c the character to walk with
	 * @param perfHashValue the perfHash value of the current transition is ADDED to this variable.
	 *        If transition does not exist, the perfHashValue is undefined
	 */
	inline StateId_t walkPerfHash( StateId_t state, wchar_t c, size_t* perfHashValue ) const;

	/**
	 * perform one step inside the automaton and keep track of the perfect hashing value
	 * Implemented for TOKDIC only
	 * @param state the state to start from
	 * @param str the string to walk with
	 * @param perfHashValue the perfHash values of the transitions on the way are ADDED to this variable.
	 *        If transition does not exist, the perfHashValue is undefined
	 */
	inline StateId_t walkStrPerfHash( StateId_t state, const wchar_t* str, size_t* perfHashValue ) const;

	/// @brief returns the root
	inline StateId_t getRoot() const;

	/**
	 * Get a State object of the automaton's root/ start state. 
	 * @return a State object of the automaton's root/ start state. 
	 * @see State
	 */
	inline State getRootState() const {
	    return State( *this );
	}
	
	/**
	 * @brief returns true iff state is marked as final
	 * @param state id of a state
	 */
	inline bool isFinal( StateId_t state ) const;

	/**
	 * @return the string containing labels of all existing outgoing transitions from state
	 */
	inline const wchar_t* getSusoString( StateId_t state ) const;

	/**
	 * @brief For a given key, output parameter tokID is set to n if key is the n-th token of the dictionary
	 *
	 * @param[in] key the key to look up
	 * @param[out] tokID if key was found, this variable will hold the respective index. Otherwise, it gets an undefined value.
	 * 
	 * @return true iff key was found
	 */
	inline bool getTokenIndex( const wchar_t* key, size_t* tokID ) const;


	/**
	 *
	 */
	inline InternalSize_t getStateAnnotation( StateId_t state ) const;



	/******************** COMPILATION OF A NEW TRANSTABLE ********************/

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
	 * @return the state id
	 */
	inline StateId_t storeTempState( TempState_t& state );

	/**
	 * declare a root (or start state) of the automaton.
	 */
	inline void setRoot( StateId_t rootId );
	

	/******************** SERIALIZATION ********************/

	/**
	 * Loads an automaton from a binary file.
	 * @param binFile filename of the binary storing the automaton
	 * @throw exceptions::badFileHandle
	 */
	inline bool loadFromFile( char const* binFile );

	inline void loadFromStream( FILE* fi );

	/**
	 * Dumps the automaton into a file
	 * @param binFile filename to dump the automaton into (usually .dic)
	 */
	inline void createBinary( char* binFile );

	inline void writeToFile( char* binFile );

	/**
	 * 
	 */
	void writeToStream( FILE* fo ) const;





	inline bool compareStates( const TempState_t& temp, StateId_t comp ) const;

	inline size_t count( StateId_t pos ) const;

	inline void printCells() const;
	inline void toDot() const;
	inline void doAnalysis() const;


	inline Cell_t* getCells() {
	    return cells_;
	}
	inline size_t getNrOfCells() const {
	    return nrOfCells_;
	}

	inline size_t getSizeOfUsedCells() const {
	    return sizeOfUsedCells_;
	}

	inline size_t getNrOfStates() const {
	    return nrOfStates_;
	}

    private:
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
		lengthOfSusoStrings_( 0 ),
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
	    size_t getLengthOfSusoStrings() const {
		return static_cast< size_t >( lengthOfSusoStrings_ );
	    }

	    void set( const TransTable_t& transTable ) {
		magicNumber_ = transTable.magicNumber_;
		nrOfCells_ = transTable.nrOfCells_;
		lengthOfSusoStrings_ = transTable.lengthOfSusoStrings_;
		root_ = transTable.root_;
	    }
	    

	private:
	    bits64 magicNumber_;
	    bits64 nrOfCells_;
	    bits64 lengthOfSusoStrings_;
	    bits64 root_;
	};

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

#include "./TransTable_perfHash.tcc"


#endif
