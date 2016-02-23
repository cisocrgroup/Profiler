#ifndef CSL_TRANSTABLE_STATE
#define CSL_TRANSTABLE_STATE CSL_TRANSTABLE_STATE

#include <vector>
#include <cassert>
#include <iostream>
#include "../Alphabet/Alphabet.h"
#include "../Global.h"
namespace csl {

    /**
       TempState is to be used to construct a state of the automaton bit by bit. Once it is certain that no more outgoing edges need to be added, it can be stored into the Automaton. Edges can point to already-stored edges only.

       @author Uli Reffle <uli@reffle.de>
       @date 2005
    */

    template< typename TransTable_t__ >
    class TempState {
    private:
	typedef TransTable_t__ TransTable_t;
	typedef typename TransTable_t__::InternalChar_t InternalChar_t;
	typedef typename TransTable_t__::InternalSize_t InternalSize_t;

	/**
	 * Represents one transition of the TempState
	 */
	class Transition {
	public:
	    Transition( wchar_t label, InternalSize_t target, InternalSize_t phNumber) {
		label_ = label;
		target_ = target;
		phNumber_ = phNumber;
	    }
	    wchar_t getLabel() const { return label_; }
	    uint_t getTarget() const { return target_; }
	    InternalSize_t getPhNumber() const { return phNumber_; }

	    // we need this method whenever the state is marked as final
	    void incPhNumber() { ++phNumber_; }
	private:
	    wchar_t label_;
	    InternalSize_t target_;
	    InternalSize_t phNumber_;
	}; // class Transition

	std::vector<Transition> transitions_;

	std::vector<wchar_t> susoString_;

	InternalSize_t phSum_;

	bool isFinal_;

	InternalSize_t annotation_;


	class Iterator {
	public:
	    Iterator( TempState& myTempState ) : myTempState_( myTempState ),
						       index_( 0 ) {
	    }
 
	    bool isValid() const {
		return ( index_ < ( myTempState_.transitions_.size() ) );
	    }

	    Transition& operator*() {
		return myTempState_.transitions_.at( index_ );
	    }

	    Transition* operator->() {
		return &( myTempState_.transitions_.at( index_ ) );
	    }

	    Transition& operator++() {
		++index_;
		return myTempState_.transitions_.at( index_ );
	    }
	    

	private:
	    TempState& myTempState_;
	    InternalSize_t index_;
	}; // class Iterator


    public:

	typedef typename std::vector< Transition >::iterator TransIterator;
	typedef typename std::vector< Transition >::const_iterator const_TransIterator;

	TransIterator transBegin() {
	    return transitions_.begin();
	}

	TransIterator transEnd() {
	    return transitions_.end();
	}

	const_TransIterator transBegin() const {
	    return transitions_.begin();
	}

	const_TransIterator transEnd() const {
	    return transitions_.end();
	}


	TempState() : phSum_( 0 ),
		      isFinal_( false )
	    {

	    reset();
	}

	~TempState() {
	}
	
	inline InternalSize_t getPhValue() const {
	    return phSum_ + ( isFinal() ? 1 : 0 );
	}

	/**
	 * add an outgoing edge
	 *
	 * @param label
	 * @param target
	 */
	inline void addTransition( wchar_t label, InternalSize_t target, InternalSize_t targetPhNumber ) {
	    // assert that new labels are coming in alphabetical order
	    // that's important for transStr_
	    if( ( transitions_.size() > 0 ) &&
		( transitions_.at( transitions_.size() - 1 ).getLabel() >= label ) ) {

		throw exceptions::cslException("TempState: new transition violating alphabetical order");
	    }
	    
	    transitions_.push_back( Transition( label, target, getPhValue() ) );
	    susoString_.at( susoString_.size() - 1 ) = label;
	    susoString_.push_back( 0 );

	    phSum_ += targetPhNumber;
	}

	/**
	 * reset the state for re-use
	 */
	inline void reset() {
	    transitions_.clear();

	    susoString_.clear();
	    susoString_.push_back( 0 );
	    
	    annotation_ = 0;
	    isFinal_ = false;
	    phSum_ = 0;
	}

	inline void setAnnotation( InternalSize_t annotation ) {
	    annotation_ = annotation;
	}

	inline InternalSize_t getAnnotation() const {
	    return annotation_;
	}

	inline const wchar_t* getSusoString() {
	    susoString_.push_back( 0 );  // make susoString_ a \0-terminated string
	    return &( susoString_.at( 0 ) );
	}

	/**
	 * mark the state as final
	 */
	void setFinal() {
	    if( ! isFinal_ ) {
		for( TransIterator it = transBegin(); it != transEnd(); ++it ) {
		    (*it).incPhNumber();
		}
	    }
	    isFinal_ = true;
	}
	
	/**
	 * returns true iff state is marked as final
	 * @returns true iff state is marked as final
	 */
	bool isFinal() const {
	    return isFinal_;
	}

    }; // class TempState

} //eon

#endif
