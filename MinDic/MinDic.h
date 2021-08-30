#ifndef CSL_MINDIC_H
#define CSL_MINDIC_H CSL_MINDIC_H

#include <fstream>
#include <vector>
#include <stack>
#include <errno.h>

#include "../Global.h"
#include "../iDictionary/iDictionary.h"
#include "../TransTable/TransTable.h"
#include "../Alphabet/Alphabet.h"
#include "./StateHash.h"

namespace csl {


    /**
     * @brief class MinDic performs the construction and usage of minimal acyclic
     * finite state automata for large finite dictionaries.
     *
     * It follows an algorithm described in
     * Jan Daciuk et. al., Incremental Construction of Minimal Acyclic
     * Finite-State Automata, 2000.
     *
     * Many ideas for the implementation are adapted from a C-program written
     * by Stoyan Mihov.
     *
     * Note that a MinDic inherits from TransTable and so offers all of TransTable's
     * interface to the guts of the automaton. However this might change in the future.
     *
     * @author Ulrich Reffle, <uli@cis.uni-muenchen.de>
     * @date Apr 2006
     *
     */
    template< class AnnType = int >
    class MinDic : public TransTable< TT_PERFHASH, uint16_t >
		 , public iDictionary< AnnType >
    {
    public:
	typedef TransTable< TT_PERFHASH, uint16_t > TransTable_t;

	/**
	 * @brief The datatype of the annotation that comes with each dictionary entry.
	 * Note that \c AnnType_t, currently,  has to be a trivial datatype: if you store
	 * an object x of type AnnType_t, the MinDic will store exactly sizeof(AnnType_t) bytes,
	 * beginning at address &x. Heap data will be lost.
	 */
	typedef AnnType AnnType_t;

	/**
	 * @brief This class provides a much easier interface to the MinDic if the guts of the automaton
	 * have to be explored state- and transition-wise.
	 *
	 * It is best to think of a State -object as a pawn in a board game. By using MinDic's getRootState()
	 * you receive a pawn positioned at the automaton's root state. From there you can either move it along-side
	 * a transition using walk() or request a new pawn using getTransitionTarget().
	 *
	 */
	class State {
	public:
	    /**
	     * @param a reference to the \c MinDic the \c State belongs to.
	     */
	    State( const MinDic< AnnType >& minDic ) :
		minDic_( &minDic ),
		dicPos_( minDic_->getRoot() ),
		perfHashValue_( 0 ) {
	    }

	    /**
	     * @param c
	     */
	    bool walk( wchar_t c ) {
		dicPos_ = minDic_->walkPerfHash( dicPos_, c, &perfHashValue_ );
		return isValid();
	    }

	    /*
	     * @brief If the state-object is 'walked' using a character c although the current state has no such
	     *        transition, the state becomes invalid. Some people like to introduce a "failure state" or "trap state",
	     *        which is non-final and has no outgoing transitions. In this sense, isValid() returns true
	     *        iff it refers to this trap state.
	     *
	     *        @see hasTransition
	     */
	    bool isValid() {
		return ( dicPos_ != 0 );
	    }

	    /**
	     * @brief returns true iff current state has a transition labelled with the given character.
	     * @param c
	     */
	    bool hasTransition( wchar_t c ) const {
		return ( minDic_->walk( dicPos_, c ) != 0 );
	    }

	    /**
	     *
	     */
	    State getTransTarget( wchar_t c ) const {
		size_t tmpPHValue = perfHashValue_;
		StateId_t newPos = minDic_->walkPerfHash( dicPos_, c, &tmpPHValue );
		return State( *minDic_, newPos, tmpPHValue );
	    }

	    /**
	     *
	     */
	    State getTransTarget( wchar_t const* str ) const {
		size_t tmpPHValue = perfHashValue_;
		StateId_t newPos = minDic_->walkStrPerfHash( dicPos_, str, &tmpPHValue );
		return State( *minDic_, newPos, tmpPHValue );
	    }


	    /**
	     * @brief returns a c-string containing all characters which are the label of a transition leaving
	     *        the current state
	     */
	    const wchar_t* getSusoString() const {
		return minDic_->getSusoString( dicPos_ );
	    }

	    /**
	     * @see perfHashValue_
	     */
	    size_t getPerfHashValue() const {
		return perfHashValue_;
	    }

	    /**
	     * @brief returns the internal id number of the curent state
	     */
	    StateId_t getStateID() const {
		return dicPos_;
	    }

	    /**
	     * @brief returns true iff the current state is a final state
	     */
	    bool isFinal() const {
		return minDic_->isFinal( dicPos_ );
	    }

	    /**
	     *
	     */
	    const AnnType& getAnnotation() {
		return minDic_->getAnnotation( getPerfHashValue() );
	    }

	private:

	    State( const MinDic< AnnType >& minDic, StateId_t dicPos, size_t perfHashValue ) :
		minDic_( &minDic ),
		dicPos_( dicPos ),
		perfHashValue_( perfHashValue ) {
	    }

	    /**
	     * @brief A pointer to the MinDic we are using
	     */
	    const MinDic< AnnType >* minDic_;

	    /**
	     * @brief The internal id of the current state
	     */
	    StateId_t dicPos_;

	    /**
	     * @brief The perfect hashing value
	     *
	     * Note that this vale does not only depend on the current state but also on the path
	     * from the root that was used to get there.
	     */
	    size_t perfHashValue_;

	}; // class State


	/**
	 * This iterator allows to traverse through all words of the language the MinDic describes.
	 * The words are travered in alphabetical order.
	 */
	class TokenIterator {
	private:
	    typedef std::pair< State, size_t > StackItem_t;
	public:
	    /**
	     * @brief creates an iterator that equals tokensEnd()
	     *        because of its empty currentString_
	     */
	    TokenIterator() {
	    }


	    TokenIterator( State const& rootState ) {
		if( ( rootState.getSusoString())[0] == 0 ) { // no transitions from root state
		    return; // iterator now equals to tokensEnd()
		}
		stack_.push( std::make_pair( rootState, 0 ) );
		State st = rootState;
		while( st.isValid() && ! st.isFinal() ) {
		    wchar_t nextChar = (st.getSusoString())[0]; // this might be \0
		    st.walk( nextChar );
		    ++( stack_.top().second ); // we used the first transition
		    if( nextChar ) {
			stack_.push( std::make_pair( st, 0 ) );
			currentString_ += nextChar;
		    }
		}
	    }

	    bool operator==( TokenIterator const& other ) const {
		return currentString_ == other.currentString_;
	    }

	    bool operator!=( TokenIterator const& other ) const {
		return !( *this == other );
	    }

	    /**
	     * @brief returns a reference to the current token. CAUTION: This reference becomes invalid
	     *        as soon as the iterator is moved forward.
	     */
	    std::wstring const& operator*() const {
		return currentString_;
	    }

	    std::wstring const* operator->() const {
		return &currentString_;
	    }

	    TokenIterator& operator++() {
		do {
		    //std::wcout << "At: " << stack_.top().first.getStateID() << "," << stack_.top().second << std::endl;
		    wchar_t nextChar = (stack_.top().first.getSusoString())[ stack_.top().second ];
		    if( nextChar ) { // more paths to go from this state
			++( stack_.top().second );
			State st = stack_.top().first.getTransTarget( nextChar );
			stack_.push( std::make_pair( st, 0 ) );
			currentString_ += nextChar;
			// std::wcout << "forward to " << stack_.top().first.getStateID() << std::endl;
		    }
		    else {
			stack_.pop();
			if( ! stack_.empty() ) {
			    currentString_.resize( currentString_.size() - 1 );
			    // std::wcout << "back to " << stack_.top().first.getStateID() << std::endl;
			}
		    }
		} while( ! ( stack_.empty() || // quit if stack is empty or if final state is reached for the 1st time
			     ( stack_.top().first.isFinal() && stack_.top().second == 0 )
			     )
		    );
		return *this;
	    }

	private:
	    std::wstring currentString_;
	    /**
	     * @brief the std::pair holds (first) a state and (second) the index in this state's susoString that is due to be
	     * processed nect
	     */
	    std::stack< StackItem_t > stack_;
	};


	/**
	 * @brief Create a new MinDic. An optional file-path as argument invokes a call to loadFromFile.
	 * @param a file containing a compiled MinDic. (optional; often: *.mdic)
	 */
	MinDic( const char* dicFile = 0 );


	/**
	 * @brief The copy constructor is NOT IMPLEMENTED at the moment
	 */
	MinDic( MinDic< AnnType_t > const& other );


	/**
	 * @name Lookup
	 */
	//@{

	/**
	 * Use this function to do a convenient lookup. If you don't need the annotation, just pass on
	 * a NULL-Pointer or omit the 2nd argument.
	 *
	 * @param[in] key a key of the dictionary
	 * @param[out] annotation a pointer where to store the found annotation. If NULL, then annotation is discarded
	 * @return true iff key was found in the dictionary
	 */
	inline bool lookup( const wchar_t* key, AnnType_t* annotation = 0 ) const;

	inline bool lookup( std::wstring const& key, AnnType_t* annotation = 0 ) const;

	inline bool hasPrefix( std::wstring const& prefix ) const;



	/**
	 * @deprecacted this method is renamed to lookup()
	 */
	inline bool getAnnotation( const wchar_t* key, AnnType_t* annotation ) const;


	inline const AnnType_t& getAnnotation( size_t perfHashValue ) const;

	/**
	 * Get a State object of the automaton's root/ start state.
	 * @return a State object of the automaton's root/ start state.
	 * @see State
	 */
	inline State getRootState() const {
	    return State( *this );
	}

	inline TokenIterator tokensBegin() const {
	    if( readyToRead() ) {
		return TokenIterator( getRootState() );
	    }
	    else {
		return tokensEnd();
	    }
	}

	inline TokenIterator tokensEnd() const {
	    return TokenIterator();
	}

	//@}


	/**
	 * @name Loading from /writing to hard disk
	 */
	//@{
	/**
	 * @brief Load a compiled MinDic from hard disk.
	 * @param a file containing a compiled MinDic. (often: *.mdic)
	 */
	inline void loadFromFile( const char* binFile );

	/**
	 * @brief Load a compiled MinDic from an open file stream.
	 * @param fi a c-style file pointer.
	 */
	inline void loadFromStream( FILE* fi );

	/**
	 * @brief dump MinDic automaton to a file in binary form.
	 * @param binFile File to write the automaton into.
	 */
	inline void writeToFile( char const* binFile ) const;

	/**
	 * @brief dump MinDic automaton to an open file stream
	 * @param fo a c-style file pointer.
	 */
	inline void writeToStream( FILE* fo ) const;

	//@}

	/**
	 * @name Construction of a new MinDic
	 */
	//@{
	/**
	 * @brief The funtion that actually executes the computation of the trie.
	 * @todo Should this method be public??? Maybe not ...
	 * @param txtFile The dictionary (including annotations) in txt format
	 */
	inline void compileDic( const char* txtFile );

	/**
	 * @brief This method parses an input line (provided as param str). If str has
	 *        an annotation attached to the key, this annotation is clipped from str
	 *        and stored in the param annotation
	 * @param[in/out] str
	 * @param[out] annotation
	 */
	inline void parseAnnotation( std::wstring* str, AnnType_t* annotation ) const;

	/**
	 * @brief prepares the object for construction of a new MinDic.
	 * Call before calling addToken() for the first time.
	 */
	inline void initConstruction();

	/**
	 * @brief Add one new entry. Keys must be inserted in alphabetical order.
	 */
	inline void addToken( const wchar_t* input, const AnnType_t& value );


	/**
	 * @brief Finalize the construction
	 * Call after calling addToken() for the last time.
	 */
	inline void finishConstruction();

	//@}

	/**
	 * @name Convenience, Information, Debug
	 */
	//@{

	inline void printDic() const;

	/// extracts the trie to stdout
	inline void printDic( StateId_t initState ) const;

	inline size_t getNrOfKeys() const;

	inline void doAnalysis() const;

	/**
	 * This method from TransTable is blocked for MinDic, not implemented here!
	 */
	inline int getFirstAnn( StateId_t state );

 	//@}

    protected:
	inline const AnnType_t& annotationsAt( size_t n ) const;

    private:

	mutable size_t count_; // is used for counting during printing

	// Those vars are used for construction
	TempState_t *tempStates_;
	StateHash< TransTable_t  >* hashtable_;

	wchar_t lastKey_[Global::lengthOfLongStr];

	AnnType_t* annotations_;
	size_t sizeOfAnnotationBuffer_;

	size_t nrOfKeys_;

	inline StateId_t replaceOrRegister( TempState_t& state );

	void printDic_rec( StateId_t pos, int depth, size_t perfHashValue ) const;

	static const bits64 magicNumber_ = 47247821;
	class Header {
	public:
	    bits64 getMagicNumber() const {
		return magicNumber_;
	    }
	    size_t getNrOfKeys() const {
		return static_cast< size_t >( nrOfKeys_ );
	    }

	    void set( const MinDic& minDic ) {
			magicNumber_ = minDic.magicNumber_;
			nrOfKeys_ = minDic.nrOfKeys_;
	    }
	private:
	    bits64 magicNumber_;
	    bits64 nrOfKeys_;
	};

	Header header_;

    };


} //eon

#include "./MinDic.tcc"

#endif
