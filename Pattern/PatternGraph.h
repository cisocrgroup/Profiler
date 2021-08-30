#ifndef CSL_PATTERNGRAPH_H
#define CSL_PATTERNGRAPH_H CSL_PATTERNGRAPH_H

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <queue>
#include <string>
#include <vector>

#include "../Global.h"
#include "../Getopt/Getopt.h"
#include "../Pattern/PatternSet.h"

namespace csl {

    /**
     * @brief A specialised variant of the Aho-Corasick trie to search
     * in the set of left or right sides of a PatternSet.
     *
     * For the sake of simplicity and efficiency, the implementation details of
     * class \c PatternSet are used here: Internally we use simple \c size_t variables
     * to refer to a \c Pattern instead of using the official and public \c PatternRef
     * interface.
     *
     * @author Ulrich Reffle, 2008
     */
    class PatternGraph : public PatternSet {
    public:
	class Alphabet {

	public:

	    void initConstruction() {
		custom2std_.push_back( 0 ); // let 0 map to 0
	    }

	    void addChar( wchar_t c ) {
		if( ( std2custom_.size() > (size_t)c ) && std2custom_.at( c ) ) {
		    //std::wcout<<c<<L" already in alphabet"<<std::endl;

		}
		else {
		    if( (size_t)c >= std2custom_.size()  ) {
			std2custom_.resize( c + 1, 0 );
		    }
		    std2custom_.at( c ) = custom2std_.size(); // that's the first free slot of custom2std_
		    custom2std_.push_back( c );
		    //std::wcout<<"add "<<c<<L" to the alphabet"<<std::endl;
		}
	    }

	    void finishConstruction() {
		// sort the custom alphabet
		std::sort( custom2std_.begin(), custom2std_.end() );
		// update std2custom_ accordingly
		size_t n = 0;
		for( std::vector< wchar_t >::iterator it = custom2std_.begin(); it != custom2std_.end(); ++it ) {
		    std2custom_.at( *it ) = n;
		    //std::wcout<<"custom2std_.at("<< n <<" ) = "<<*it<<std::endl;
		    ++n;
		}

		n = 0;
		for( std::vector< wchar_t >::iterator it = std2custom_.begin(); it != std2custom_.end(); ++it ) {
		    //std::wcout<<"std2custom_.at("<< n<<" ) = "<<(size_t)*it<<std::endl;
		    ++n;
		}
	    }

	    wchar_t encode( wchar_t c ) const {
//		return std2custom_.at( c );
		return ( std2custom_.size() > (size_t)c ) ? std2custom_.at( c ) : 0;
	    }

	    wchar_t decode( wchar_t c ) const {
		return custom2std_.at( c ); // here we take the risky road
		// return ( custom2std_.size() > (size_t)c ) ? custom2std_.at( c ) : 0;
	    }

	    size_t size() const {
		return custom2std_.size();
	    }

	private:
	    std::vector< wchar_t > std2custom_;
	    std::vector< wchar_t > custom2std_;
	};

	// a string and an index into the list of patterns (from PatternSet)
	typedef std::vector< std::pair< std::wstring, size_t > > Replacements_t;


	class InternalState {
	public:
	    InternalState( Alphabet const& alph ) :
		alph_( &alph ), errorLink_( 0 ), prefixLength_( 0 ), isFinal_( false ) {

		transitions_.resize( alph_->size(), 0 );
	    }

	    /**
	     * Returns the target stateIndex when walking with char c (Only in the trie, no errorlinks!).
	     *
	     * @return the stateIndex of the follow-up state when walking in the trie with char c. ( 0 for fail state)
	     */
	    inline size_t getTransTarget( wchar_t c ) const {
		//std::wcout<<"walk with "<<c<<"("<<(size_t)alph_->encode( c )<<")"<<std::endl;
		return transitions_.at( alph_->encode( c ) );
	    }

	    inline void setTransition( wchar_t c, size_t targetStateIndex ) {
		if( targetStateIndex == 0 ) {
		    throw std::runtime_error( "csl::PatternGraph::setTransition: Removing a transition (set target to 0 ) is not supported" );
		}
		if( transitions_.at( alph_->encode( c ) ) == 0 ) {
		    allLabels_ += c;
		}
		transitions_.at( alph_->encode( c ) ) = targetStateIndex;
	    }


	    class TransIterator {
	    public:
		TransIterator( InternalState const* myIntState, size_t labelIndex = 0 ) : myInternalState_( myIntState ),
							     labelIndex_( labelIndex ) {
		}

		TransIterator& operator++() {
		    ++labelIndex_;
		    return *this;
		}
		bool operator==( TransIterator const& other ) const {
		    return ( ( myInternalState_ == other.myInternalState_ ) && ( labelIndex_ == other.labelIndex_ ) );

		}
		bool operator!=( TransIterator const& other ) const {
		    return !( *this == other );
		}

		std::pair< wchar_t, size_t > operator*() {
		    return std::make_pair( myInternalState_->allLabels_.at( labelIndex_ ),
				      myInternalState_->getTransTarget( myInternalState_->allLabels_.at( labelIndex_ ) ) );
		}

	    private:
		InternalState const* myInternalState_;
		/**
		 * @brief current position in the string of all labels
		 */
		size_t labelIndex_;

	    }; // class TransIterator

	    Alphabet const* alph_; // it's a pointer because the object needs to work with the assignment-operator
	    std::vector< size_t > transitions_;
	    Replacements_t replacements_;
	    size_t errorLink_;
	    std::wstring allLabels_;
	    int prefixLength_;
	    bool isFinal_;
	}; // class InternalState



	class State {
	public:
	    typedef InternalState::TransIterator iterator;

	    inline State() :
		myGraph_( 0 ),
		stateIndex_( 0 ) {

	    }

	    inline State( const PatternGraph& myGraph, size_t stateIndex ) :
		myGraph_( &myGraph ),
		stateIndex_( stateIndex ) {
	    }

	    inline bool operator==( State const& other ) {
		return( ( myGraph_ == other.myGraph_  ) && // compare pointers
			( stateIndex_ == other.stateIndex_ ) );
	    }

	    inline State& operator=( State const& other ) {
		myGraph_ = other.myGraph_;
		stateIndex_ = other.stateIndex_;
		return *this;
		}

	    /**
	     * walk in the AC-Trie. Use error links when needed.
	     *
	     * @return the "distance" from the root to the target state
	     */
	    inline bool walk( wchar_t c );

	    inline bool walkErrorLink();

	    /**
	     * walk in the trie only - do not use error links
	     * if walk is not possible, stay where you are
	     *
	     * @return true if an appropriate transition was found
	     */
	    inline bool walkStrict( wchar_t c );

	    /**
	     * This is equivalent to a 'walk' call, only that the state itself
	     * remains unchanged (-->const), but a copy of the target state
	     */
	    inline State getTransTarget( wchar_t c ) const;

	    /**
	     * @return an iterator positioned at the first of the state's transitions
	     */
	    inline iterator transBegin() const;

	    /**
	     * @return a past-the-end iterator for the state's transitions
	     */
	    inline iterator transEnd() const;

	    /**
	     * @return the "distance" from the root to the target state
	     */
	    inline size_t getPrefixLength() const;

	    inline bool isFinal() const;

	    /**
	     * @return a reference to the list of replacements attached to the state (which must be final)
	     */
	    inline const Replacements_t& getReplacements() const;


	    /**
	     * @return true iff the current state is not the failstate
	     */
	    inline bool isValid() const;

	    inline void addTransition( wchar_t c, const State& target );
	    size_t getStateIndex() const;

	private:
	    friend class PatternGraph;
	    friend class InternalState;

	    const PatternGraph* myGraph_;
	    size_t stateIndex_;

	}; // class State


	enum Direction {FORWARD, BACKWARD};
	enum IndexSide {INDEX_LEFT, INDEX_RIGHT};



	inline PatternGraph( Direction dir = FORWARD, IndexSide indexSide = INDEX_LEFT );

	inline void loadPatterns( const char* patternFile );

	inline State getRoot() const;

	inline size_t getNrOfPatterns() const;

	inline void toDot() const;


	// *** PRIVATE of PATTERNGRAPH ***
    private:


	Direction direction_;

	/**
	 * @brief Specifies if the left or right side of the pattern shall be indexed
	 *        in the trie
	 */
	IndexSide indexSide_;

	inline void addErrorLinks();

	/**
	 * Add a new state and return its index in the vector
	 */
	inline State newState();


	Alphabet alph_;

	/**
	 * Don't forget the realloc-trouble here !!!
	 */
	std::vector< InternalState > states_;

	size_t nrOfPatterns_;

    };



    /********** IMPL   STATE  ********************/

    bool PatternGraph::State::walk( wchar_t c ) {
//	std::wcout<<"Reading "<<c<<", beginning at state "<<getStateIndex()<<std::endl; // DEBUG
	while( isValid() && ( ! walkStrict( c ) ) ) {
	    stateIndex_ = myGraph_->states_.at( getStateIndex() ).errorLink_;
	    // std::wcout<<"  Retreat to "<<getStateIndex()<<std::endl; // DEBUG
	}
	if( ! isValid() ) {
	    stateIndex_ = 1;
	}
	return isValid(); // btw, this should always be true
    }

    bool PatternGraph::State::walkStrict( wchar_t c ) {
	if( size_t newIndex =  myGraph_->states_.at( stateIndex_ ).getTransTarget( c ) ) {
	    stateIndex_ = newIndex;
	    return true;
	}
	else {
	    return false;
	}
    }

    bool PatternGraph::State::walkErrorLink() {
	stateIndex_ = myGraph_->states_.at( getStateIndex() ).errorLink_;
	return isValid();
    }

    inline PatternGraph::State PatternGraph::State::getTransTarget( wchar_t c ) const {
	State newState = *this;
	newState.walk( c);
	return newState;
    }

    inline size_t PatternGraph::State::getPrefixLength() const {
	return myGraph_->states_.at( stateIndex_ ).prefixLength_;
    }


    inline size_t PatternGraph::State::getStateIndex() const {
	return stateIndex_;
    }

    inline PatternGraph::State::iterator PatternGraph::State::transBegin() const {
	return InternalState::TransIterator( &( myGraph_->states_.at( stateIndex_ ) ), 0 );
    }

    inline PatternGraph::State::iterator PatternGraph::State::transEnd() const {
	return InternalState::TransIterator( &( myGraph_->states_.at( stateIndex_ ) ), myGraph_->states_.at( stateIndex_ ).allLabels_.size() );
    }

    inline bool PatternGraph::State::isFinal() const {
	return myGraph_->states_.at( stateIndex_ ).isFinal_;
    }

    inline bool PatternGraph::State::isValid() const {
	return ( stateIndex_ != 0 );
    }

    inline PatternGraph::Replacements_t const& PatternGraph::State::getReplacements() const {
	return myGraph_->states_.at( stateIndex_ ).replacements_;
    }

//     inline const std::wstring& PatternGraph::State::getWord() const {
// 	return myGraph_->states_.at( stateIndex_ ).word_;
//     }


    /********* IMPL  PATTERNGRAPH *****************/

    PatternGraph::PatternGraph( Direction dir, IndexSide indexSide ) :
	direction_( dir ),
	indexSide_( indexSide ),
	nrOfPatterns_( 0 ) {
    }

    inline PatternGraph::State PatternGraph::getRoot() const {
	return State( *this, 1 );
    }


    inline size_t PatternGraph::getNrOfPatterns() const {
	return nrOfPatterns_;
    }


    inline void PatternGraph::loadPatterns( const char* patternFile ) {

	// Use the base class method to read the patterns from the file
	PatternSet::loadPatterns( patternFile );


	// 1st pass: build alphabet
	alph_.initConstruction();
	for( PatternList_t::const_iterator pattern = patternList().begin() + 1; // skip 1st pattern: it's the empty pattern!
	     pattern != patternList().end();
	     ++pattern ) {
	    std::wstring const& indexed = ( indexSide_ == INDEX_LEFT )? pattern->getLeft() : pattern->getRight();
	    for( std::wstring::const_iterator c = indexed.begin(); c != indexed.end(); ++c ) {
		alph_.addChar( *c );
	    }
	}
	alph_.finishConstruction();

	states_.push_back( InternalState( alph_ ) ); // failure state at position 0
	states_.push_back( InternalState( alph_ ) ); // root at position 1

	size_t patternCount = 0;
	std::wstring indexed, replacement;
	for( PatternList_t::const_iterator pattern = patternList().begin() + 1; // skip 1st pattern: it's the empty pattern!
	     pattern != patternList().end();
	     ++pattern ) {

	    std::wstring const& indexedRef = ( indexSide_ == INDEX_LEFT )? pattern->getLeft() : pattern->getRight();
	    std::wstring const& replacementRef = ( indexSide_ == INDEX_LEFT )? pattern->getRight() : pattern->getLeft();

	    if( direction_ == BACKWARD ) {
		Global::reverse( indexedRef, &indexed );
		Global::reverse( replacementRef, &replacement );
	    }
	    else {
		indexed = indexedRef;
		replacement = replacementRef;
	    }


	    State state = getRoot();

	    // *** find common prefix ***
	    std::wstring::const_iterator c = indexed.begin(); // c must be visible outside the for-loop
	    for( ;
		 c != indexed.end() && state.walkStrict( *c );
		 ++c ) {
		// do nothing else
	    }

	    // *** add suffix ***
	    // prevent the vector from re-allocating during the process
	    states_.reserve( states_.size() + ( indexed.end() - c ) );

	    State lastState = state;
	    for( ;
		 c != indexed.end();
		 ++c ) {
		State newSt = newState();
		size_t newPrefixLength = lastState.getPrefixLength();
		if( (*c != Global::wordBeginMarker ) && (*c != Global::wordEndMarker ) ) ++newPrefixLength; // begin and end-markers do not contribute to prefixLength

		states_.at( newSt.getStateIndex() ).prefixLength_ = newPrefixLength;
		states_.at( lastState.getStateIndex() ).setTransition( *c, newSt.getStateIndex() );
		lastState = newSt;
	    }
	    states_.at( lastState.getStateIndex() ).isFinal_ = true;

	    states_.at( lastState.getStateIndex() ).replacements_.push_back( std::make_pair( replacement, pattern - patternList().begin() ) );
	}

	addErrorLinks();

	// toDot(); // DEBUG

    }

    inline void PatternGraph::addErrorLinks() {

	states_.at( 1 ).errorLink_ = 0;
	std::queue< size_t > queue;
	queue.push( getRoot().getStateIndex() );

	while( ! queue.empty() ) {
	    size_t parent = queue.front();
	    queue.pop();

	    for( std::wstring::const_iterator c = states_.at( parent ).allLabels_.begin();
		 c !=  states_.at( parent ).allLabels_.end();
		 ++c ) {
		queue.push( states_.at( parent ).getTransTarget( *c ) );

		size_t current = queue.back();
		size_t parentBack = parent;
		size_t back = 0;

		while( ( back == 0 ) && ( parentBack != 0 ) ) {
		    parentBack = states_.at( parentBack ).errorLink_;
		    back = states_.at( parentBack ).getTransTarget( *c );
		}
		if( parentBack == 0 ) back = 1;

		if( states_.at( back ).isFinal_ ) {
		    states_.at( current ).isFinal_ = true;
		    // copy all replacements of 'back' to 'current'
		    states_.at( current ).replacements_.insert( states_.at( current ).replacements_.end(),
								states_.at( back ).replacements_.begin(),
								states_.at( back ).replacements_.end() );
		}
		states_.at( current ).errorLink_ = back;
	    }
	}
    } // method PatternGraph::addErrorLinks

    inline PatternGraph::State PatternGraph::newState() {
	states_.resize( states_.size() + 1, InternalState( alph_ ) );
	return State( *this, states_.size() -1 );
    }

    inline void PatternGraph::toDot() const {
	std::wcout<< "Digraph PatternGraph { //DOTCODE"<<std::endl
		  <<"rankdir=LR; //DOTCODE"<<std::endl
		  <<"ordering=out; //DOTCODE"<<std::endl;


	size_t count = 0;
	for( std::vector< InternalState >::const_iterator st = states_.begin() ; st != states_.end(); ++st ) {
	    std::wcout<<count<<"[label=\""<<count<<"\",peripheries="<< ( (st->isFinal_)? "2" : "1" ) << "] // DOTCODE" << std::endl;

	    if( st->isFinal_ ) {
		std::wcout << count << "->" << count << "[color=\"#dddddd\", label=\"";
		for( Replacements_t::const_iterator r = st->replacements_.begin(); r != st->replacements_.end(); ++r ) {
		    std::wcout << "(" << this->at(r->second).toString() << ")";
		}
		std::wcout << "\"] // DOTCODE\n";
	    }

	    for( std::wstring::const_iterator c = st->allLabels_.begin();
		 c !=  st->allLabels_.end();
		 ++c ) {


		std::wcout << count << "->" << st->getTransTarget( *c ) << "[label=\"" << *c <<"\"] // DOTCODE" << std::endl;
	    }
	    std::wcout << count << "->" << st->errorLink_ << "[color=red, constraint=false] // DOTCODE" << std::endl;


	    ++count;
	}

	std::wcout << "} // DOTCODE" << std::endl;
    }


} // namespace csl

#endif
