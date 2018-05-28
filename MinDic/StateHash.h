#ifndef CSL_STATEHASH_H
#define CSL_STATEHASH_H CSL_STATEHASH_H

#include <TransTable/TransTable.h>


namespace csl {

    /**
     * @brief Statehash is a hashtable to store states of a MinDic during dictionary construction.
     * This allows for an efficient check if a new state is equivalent to an existing one.
     *
     * @author Ulrich Reffle 2006-2010
     */
    template< typename TransTableType >
    class StateHash {
    public:
	typedef TransTableType TransTable_t;

	/**
	 * @brief The constructor
	 */
	StateHash(TransTable_t& trans);

	~StateHash();

	/**
	 * @brief Inserts a state into the hashtable. This method does not check for duplicates!
	 *
	 * This method does not check if an equivalent state is already in the hashtable - this must
	 * be done before, using StateHash::findState().
	 */
	template< typename TempState >
	void insert( const TempState& state, StateId_t compId ) {
	    size_t slot = hashcode( state );
	    ChainLink* newCL = new ChainLink();
	    (*newCL).value = compId;
	    (*newCL).next = table_[slot];
	    table_[slot] = newCL;
	}

	/**
	 * @brief This method checks for a given TempState, if an equivalent state in the TransTable has been
	 *        added to the hashtable before. If so, the respective state-id in the transtable is returned.
	 *        Otherwise, 0 is returned.
	 *
	 * @return If a state equivalent to the given one exists, the respective transtable-state-id is returned.
	 *          Otherwise, 0 is returned.
	 *
	 * The comparison of a TempState and a state in the transTable is delegated to class TransTable, because the
	 * comparison depends on the TransTable type.
	 *
	 */
	template< typename TempState >
	size_t findState(const TempState& state) {
	    ChainLink* ch = table_[hashcode(state)];
	    while( ch && ! trans_.compareStates( state, ch->value ) ) {
		ch = ch->next;
	    }
	    return ( ch )? ch->value : 0;
	}


    private:
	/**
	 * @brief A reference to the TransTable object.
	 */
	TransTable_t& trans_;

	/**
	 * @brief Represents one element of the chain which is situated in each slot of the hashtable
	 */
	class ChainLink {
	public:
	    StateId_t value;
	    ChainLink* next;
	    ChainLink() {
		value = 0;
		next = NULL;
	    }
	};

	/**
	 * @brief The hash table is stored here.
	 *
	 * It is an array of ChainLink-Pointers: Empty slots contain a 0-pointer. Filled slot
	 * contain a pointer to a ChainLink-object, whose "next" attribute may point to further ChainLinks.
	 */
	ChainLink** table_;

	static const int HASHC1 = 257;

	/**
	 * @brief The hashtable has static size, specified here
	 *
	 * This is certainly not ideal, but works fine for the moment.
	 */
	static const size_t tableSize_ = (size_t)( 1ll<<25 ) - 1;

	/**
	 * @brief the hashcode function computes a slot in the hashtable, given a TempState object.
	 *
	 * First the state object is translated into a natural number a simple way. The modulo operation
	 * ensures that the returned number is smaller than the table size.
	 */
	template< typename TempState >
	int hashcode(const TempState& state) {
	    int h = (state.isFinal())? 0 : Global::maxNrOfChars;
	    h += state.getAnnotation();

	    for( typename TempState::const_TransIterator it = state.transBegin();
		 it != state.transEnd();
		 ++it ) {
		h = h*HASHC1 + it->getLabel() + Global::maxNrOfChars * it->getTarget();
	    }
	    return (abs(h) % tableSize_);
	}

    }; // class StateHash


    template< typename TransTable_t >
    StateHash< TransTable_t >::StateHash( TransTable_t& trans ) : trans_( trans ) {
	try {
	    table_ = new ChainLink*[tableSize_];
	    memset( table_, 0, tableSize_ * sizeof(ChainLink*) );
	} catch( std::bad_alloc& e ) {
	    std::cout<<"csl::StateHash: Could not allocate hashtable: " <<  e.what() << std::endl;
	    throw e;
	}
    }

    template< typename TransTable_t >
    StateHash< TransTable_t >::~StateHash() {
	ChainLink* cur = 0;
	ChainLink* next = 0;
	for( size_t i=0; i < tableSize_ ; ++i ) {
	    cur = table_[i];
	    while( cur != 0 ) {
		//std::wcout<< "i=" <<i << ", cur="<< cur << ", next="<< cur->next << std::endl;
		next = (*cur).next;
		delete( cur );
		cur = next;
	    }
	}
    }


} // eon

#endif
