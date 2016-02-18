#ifndef RESULTSET_H
#define RESULTSET_H RESULTSET_H

#include<cassert>
#include<iostream>
#include<vector>
#include<algorithm>
#include "../Global.h"
#include "../LevFilter/LevFilter.h"

namespace csl {

    /**
     * A container class for candidate sets extracted by a LevFilter object
     * @author Uli Reffle
     * @date 2006
     */
    class ResultSet : public LevFilter::CandidateReceiver {
    public:
	ResultSet() {
	    reset();
	    listBuffer_.reserve( Global::LevMaxNrOfResults );
	    list_.reserve( Global::LevMaxNrOfResults );
	}

	/**
	 * returns the current size of the ResultSet
	 *
	 * @return the current size of the ResultSet
	 */
	size_t getSize() const {
	    return list_.size();
	}

	/**
	 * add another item to the ResultSet
	 */
	void receive( const wchar_t* str, int levDistance, int annotation ) {
	    // printf( "receive: %ls\n", str );
	    if( listBuffer_.capacity() == listBuffer_.size() ) {
		listBuffer_.reserve( (size_t)( listBuffer_.capacity() * 2 ) );
		list_.reserve( (size_t)( list_.capacity() * 2 ) );
		list_.clear();
		for( std::vector< Item >::iterator it = listBuffer_.begin(); it != listBuffer_.end(); ++it ) {
		    list_.push_back( &( *it ) );
		}
		fprintf( stderr, "ResultSet realloc: capacity is now %lu\n", (unsigned long)listBuffer_.capacity() );
	    }
	    listBuffer_.push_back( Item( str, levDistance, annotation ) );
	    list_.push_back( &( listBuffer_[listBuffer_.size() - 1] ) );
	}
	
	/**
	 * delete all items from the list
	 */
	void reset() {
	    // printf( "ResultSet::reset\n" );
	    listBuffer_.clear();
	    list_.clear();
	}

	class Item {
	private:
	    wchar_t str_[Global::lengthOfWord];
	    size_t levDistance_;
	    int annotation_;
	public:

	    Item( const wchar_t* str, size_t levDistance, int ann ) :
		levDistance_( levDistance ),
		annotation_( ann )
		{
		    wcsncpy( str_, str, Global::lengthOfWord );
		}

	    /**
	     * copy constructor
	     */
	    Item( const Item& other ) :
		levDistance_( other.levDistance_ ),
		annotation_( other.annotation_ )
		{
		    wcsncpy( str_, other.str_, Global::lengthOfWord );
		}

	    /**
	     * Compares ONLY the string component
	     */
	    bool operator==( const Item& other ) const {
		return !( wcscmp( getStr(), other.getStr() ) );
	    }

	    void set( const wchar_t* str, size_t levDistance, int ann ) {
		wcscpy( str_, str );
		levDistance_ = levDistance;
		annotation_ = ann;
	    }
	    const wchar_t* getStr() const {
		return str_;
	    }
	    int getAnn() const {
		return annotation_;
	    }
	    int getLevDistance() const {
		return levDistance_;
	    }
	}; // class Item


	const Item& operator[]( int i ) const {
	    assert( list_.at( i ) );
	    return *list_[i];
	}


	/**
	 * 
	 */
	static bool cmp( const Item* a, const Item* b ) {
	    return wcscmp( a->getStr(), b->getStr() ) < 0;
	}

	static bool is_equal( const Item* a, const Item* b ) {
	    return( wcscmp( a->getStr(), b->getStr() ) == 0 );
	}

	void sort() {
	    std::sort( list_.begin(), list_.end(), cmp );
	}

	void sortUnique() {
	    sort();
	    std::unique( list_.begin(), list_.end(), is_equal );
	}
	
    private:
	std::vector< Item* > list_;
	std::vector< Item > listBuffer_;
    }; // class ResultSet

} // eon


#endif
