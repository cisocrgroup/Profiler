#ifndef CSL_TRANSTABLE_CELL_PERFHASH_H
#define CSL_TRANSTABLE_CELL_PERFHASH_H CSL_TRANSTABLE_CELL_PERFHASH_H

/**
   Represents one unit of the sparse table implementation in TransTable  
   \author Uli Reffle <uli@reffle.de>
   \date Jan 2007
*/

#include <cassert>
#include <iostream>
#include "../Global.h"
#include "./TransTable.h"

namespace csl {

    /**
     * In this implementation, the member type_ encodes the function of the cell:
     * 0: free cell
     * 1: transition cell
     * 2: state cell
     * 3: final state cell
     */
    template< typename InternalCharType__, typename SizeType__ >
    class Cell< TT_PERFHASH, InternalCharType__, SizeType__ > {
    public:
	typedef TransTable< TT_PERFHASH, InternalCharType__, SizeType__ > TransTable_t;
	typedef SizeType__ StateId_t;
	typedef SizeType__ InternalSize_t;
	typedef InternalCharType__ InternalChar_t;
	
	/* type is bit-coded: 
	 * 0  = 00000000: empty
	 * 1  = 00000001: transition
	 * 2  = 00000010: is a state
	 * 4  = 00000100: is final (this should obviously always be 110)
	 */
	typedef short CellType;
	enum {TRANS=1,STATE=2,FINALSTATE=4};


	Cell() : key_( 0 ),
		 type_( 0 ),
		 value_( 0 ) {
	    // do nothing else
	}

	void setKey( wchar_t key ) {
	    assert(key_ == 0);
	    if( key > std::numeric_limits< InternalChar_t >::max() ) {
		throw exceptions::outOfRange( "Cell<TT_PERFHASH>::setKey() : key is too big for chosen InternatChar_t" );
	    }
	    key_ = key;
	}

	wchar_t getKey() const {
	    return key_;
	}

	void setValue( InternalSize_t value ) {
//	assert(value_ == 0); // can not be asserted when changeTransitionTarget() is used
	    if( value > std::numeric_limits< InternalSize_t >::max() ) {
		 throw exceptions::outOfRange( "Cell<TT_PERFHASH>::setValue() : value is too big for chosen InternalSize_t" );
	    }
	    value_ = value;
	}

	InternalSize_t getValue() const {
	    return value_;
	}

	void setTrans( wchar_t key, StateId_t value, InternalSize_t thirdValue ) {
	    assert( isEmpty() );
	    setType( TRANS );
	    setKey( key );
	    setValue( value );
	    setThirdValue( thirdValue );
	}

	/**
	 * @return true iff cell is still empty
	 */
	bool isEmpty() const {
	    return type_ == 0;
	}

	/**
	 * @return true iff cell is marked as transition
	 */
	bool isTransition() const {
	    return isOfType( TRANS );
	}

	/**
	 * @return true iff cell is marked as state
	 */
	bool isState() const {
	    return isOfType( STATE );
	}

	/**
	 * @return true iff cell is marked as final state
	 */
	bool isFinalState() const {
	    return isOfType( FINALSTATE );
	}

	void setState() {
	    setType( STATE );
	}

	void setFinal() {
	    assert( isOfType( STATE ) );
	    setType( FINALSTATE );
	}

	void setThirdValue( InternalSize_t thirdValue ) {
	    if( thirdValue > std::numeric_limits< InternalSize_t >::max() ) {
		throw exceptions::outOfRange( "Cell<TT_PERFHASH>::setThirdValue : value is too big for chosen InternalSize_t" );
	    }
	    thirdValue_ = thirdValue;
	}

	InternalSize_t getThirdValue() const {
	    return thirdValue_;
	}

	void print() const {
	    std::cout<<"k="<<getKey()<<", v="<<getValue()<<std::endl;
	}

    private:
	void setType( CellType type ) {
	    type_ |=  type;
	}

	bool isOfType( CellType t ) const {
	    return ( ( type_ & t ) != 0 );
	}

	InternalChar_t key_;
	char type_;
	StateId_t value_;
	InternalSize_t thirdValue_;
    };

} // eon

#endif
