#ifndef CSL_HASH_H
#define CSL_HASH_H CSL_HASH_H

#include<vector>

#include "../Global.h"

namespace csl {

    /**
     * Hash provides a hash table which stores and finds c-strings. The class is
     * designed to be used for hashing suso-strings for fast traversable dictionaries -
     *
     * Inserted strings are stored in a buffer here in Hash, the hash table stores pointers
     * into that buffer.
     *
     * @author Uli Reffle, <uli@reffle.de>
     */
    template< class charType_t__ >
    class Hash {
    public:
	typedef charType_t__ charType_t;
	/**
	 * constructor
	 *
	 * @param[in/out] keyBuffer a reference to a c-string, where the keys are stored. 
	 * @param[in] estimatedNrOfKeys an estimation that helps to choose the right size of the hashtable
	 * @param[in/out] lengthOfKeyBuffer
	 * @todo think about the heuristics concerning the buffer initialization size
	 */
	inline Hash( size_t estimatedNrOfKeys, charType_t*& keyBuffer, size_t& lengthOfKeyBuffer );
	
	/**
	 * insert a key into the hash table
	 * @param key
	 */
	inline size_t findOrInsert( const charType_t* key );
	
	/**
	 * look up a key in the hash table
	 * @param key
	 * @return a value >0 iff the key was found, 0 otherwise
	 */
	inline size_t find(const charType_t* key ) const;
	
	inline void reset();

	inline size_t getTableSize() const;

	inline size_t getLengthOfKeyStrings() const;
    private:
	/**
	 * @return a const reference to the hashtable
	 */
	inline const std::vector< uint_t >& getTable() const;

	/**
	 * computes a hashcode for a given string.
	 * The computation is based on the ascii-values so that no Alph-object
	 * is necessary.
	 * @return the hashcode for str
	 */
	inline size_t getHashCode( const charType_t* key ) const;

	inline const charType_t* getStringAt( size_t offset ) const;

	inline charType_t* getStringAt( size_t offset );

	inline void reallocKeyBuffer( size_t size );


	inline int strcmpTemplate( const charType_t* w1, const charType_t* w2 ) const;
	inline charType_t* strncpyTemplate( charType_t* w1, const charType_t* w2, size_t n ) const;
	inline size_t strlenTemplate( const charType_t* w ) const;

	/**
	 * the data structure holding the actual table
	 */
	std::vector< size_t > table_;

	/**
	 * counts the number of inserted keys
	 */
	uint_t nrOfKeys_;

	/**
	 * the buffer holding all the keys
	 * This is just a reference to the buffer handed over from the client
	 */
	charType_t*& keyBuffer_;

	/**
	 * size of keyBuffer_
	 */
	size_t& lengthOfKeyBuffer_;

	/**
	 * offset just past the last terminating \\0 of keyBuffer_
	 * actual used size of the buffer is: lengthOfKeyStrings_ * sizeof(char16)
	 */
	size_t lengthOfKeyStrings_;

	/**
	 * We use closed hashing with linear probing. HASHC1 and HASHC2 are the two
	 * parameters determining the hash-function.
	 */
	/*@{*/
	static const int HASHC1 = 257;
	static const int HASHC2 = 107;
	/*@}*/
    };
    
    template< class charType_t >
    inline Hash< charType_t >::Hash( size_t estimatedNrOfKeys, charType_t*& keyBuffer, size_t& lengthOfKeyBuffer ) : 
	table_( estimatedNrOfKeys * 20, 0 ),
	nrOfKeys_( 0 ),
	keyBuffer_( keyBuffer ),
	lengthOfKeyBuffer_( lengthOfKeyBuffer ),
	lengthOfKeyStrings_( 1 )
    {
	if( keyBuffer_ == 0 ) {
	    if( lengthOfKeyBuffer_ != 0 ) {
		throw exceptions::LogicalError( "Hash-Constructor: non-compatible buffer / size" );
	    }
	    reallocKeyBuffer( estimatedNrOfKeys * 10 );
	}
    }
    
    template< class charType_t >
    inline size_t Hash< charType_t >::find( const charType_t* key ) const {
	uint_t slot = getHashCode( key );
	while( table_.at( slot ) != 0 &&  // hash-slot non-empty
	       strcmpTemplate( key, getStringAt( table_.at( slot ) ) ) ) { // no match
	    slot = ( slot + HASHC2 ) % getTableSize();
	}
	return table_.at( slot );
    }

    template< class charType_t >
    inline size_t Hash< charType_t >::findOrInsert( const charType_t* key ) {
	uint_t slot = getHashCode( key );
	while( table_.at( slot ) != 0 &&  // hash-slot non-empty
	       strcmpTemplate( key, getStringAt( table_.at( slot ) ) ) ) { // no match
	    slot = ( slot + HASHC2 ) % getTableSize();
	}

	if( table_.at( slot ) == 0 ) { // key wasn't found
	    size_t lengthOfKey = strlenTemplate( key );
	    // resize keyBuffer_ if necessary
	    while( ( lengthOfKeyStrings_ + lengthOfKey + 1 ) > lengthOfKeyBuffer_ ) {
		reallocKeyBuffer( (int) ( lengthOfKeyBuffer_ * 1.5 ) );
	    }
	    strncpyTemplate( ( keyBuffer_ + lengthOfKeyStrings_ ), key, lengthOfKey + 1 );
	    table_.at( slot ) = lengthOfKeyStrings_;
	    lengthOfKeyStrings_ += lengthOfKey + 1;
	    // std::wcout << "Create: " << key << " at slot " << slot << ", buf_offset " << table_.at( slot ) << std::endl;
	}
	else {
	    //std::wcout << "Found: " << key << " at slot " << slot << ", buf_offset " << table_.at( slot ) << std::endl;
	}
	return table_.at( slot );
    }

    template< class charType_t >
    inline const charType_t* Hash< charType_t >::getStringAt( size_t offset ) const {
	return ( keyBuffer_ + offset );
    }

    template< class charType_t >
    inline charType_t* Hash< charType_t >::getStringAt( size_t offset ) {
	return ( keyBuffer_ + offset );
    }

    template< class charType_t >
    inline void Hash< charType_t >::reset() {
	lengthOfKeyStrings_ = 0;
	nrOfKeys_ = 0;
	memset( &table_[0], 0, table_.size() * sizeof(uint_t) );
    }

    template< class charType_t >
    inline size_t Hash< charType_t >::getTableSize() const {
		return table_.size();
    }

    template< class charType_t >
    inline size_t Hash< charType_t >::getLengthOfKeyStrings() const {
	return lengthOfKeyStrings_;
    }

    /**
     * @todo FIND A GOOD NUMBER FOR THE HASHING !!!
     */
    template<>
    inline size_t Hash< uchar >::getHashCode( const charType_t* str ) const {
	size_t h = 0;
	while( *str ) {
	    h = h * 257 + *str; // MIND THIS NUMBER
	    ++str;
	}
	return (size_t)( h % getTableSize() );
    }

    template<>
    inline size_t Hash< wchar_t >::getHashCode( const charType_t* str ) const {
	uint_t h = 0;
	while( *str ) {
	    h = h * 65537 + *str; // MIND THIS NUMBER
	    ++str;
	}
	return ( h % getTableSize() );
    }

    template< class charType_t >
    inline const std::vector< uint_t >& Hash< charType_t >::getTable() const {
	return table_;
    }

    template< class charType_t >
    inline void Hash< charType_t >::reallocKeyBuffer( size_t newSize ) {
	keyBuffer_ = (charType_t*) realloc( keyBuffer_, newSize * sizeof(charType_t) );
	lengthOfKeyBuffer_ = newSize;
	if( keyBuffer_ == 0 ) {
	    throw exceptions::cslException( "csl::Hash: Hash realloc: Out of memory." );
	}
    }

    template<>
    inline int Hash< wchar_t >::strcmpTemplate( const charType_t* w1, const charType_t* w2 ) const {
	return wcscmp( w1, w2 );
    }

    template<>
    inline wchar_t* Hash< wchar_t >::strncpyTemplate( charType_t* w1, const charType_t* w2, size_t n ) const {
		return wcsncpy( w1, w2, n );
    }

    template<>
    inline size_t Hash< wchar_t >::strlenTemplate( const charType_t* w ) const {
	return wcslen( w );
    }

    template<>
    inline int Hash< uchar >::strcmpTemplate( const charType_t* w1, const charType_t* w2 ) const {
	return strcmp( (char*)w1, (char*)w2 );
    }

    template<>
    inline uchar* Hash< uchar >::strncpyTemplate( charType_t* w1, const charType_t* w2, size_t n ) const {
	return (uchar*)strncpy( (char*)w1, (const char*)w2, n );
    }

    template<>
    inline size_t Hash< uchar >::strlenTemplate( const charType_t* w ) const {
	return strlen( (char*)w );
    }

} // namespace csl

#endif
