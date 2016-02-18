#ifndef CSL_MINDICSTRING_H
#define CSL_MINDICSTRING_H CSL_MINDICSTRING_H

#include "../Global.h"
#include "../FBDic/FBDic.h"
#include "../Hash/Hash.h"
#include <sys/stat.h>

namespace csl {

    /**
     * @briefThis class provides the storage and lookup of huge dictionaries whose entries are annotated
     * with a string-value of arbitrary length.
     *
     * Currently the keys of the dictionaries are represented as wide strings, while the annotations are
     * stored as utf-8 sequences. This makes the data structure smaller, and in some contexts also faster.
     * But, now that the whole rest of the library has been changed to wide strings, this is definitely
     * a bit out-dated.
     *
     * @todo Change to consistent use of wstring
     *
     * @author Uli Reffle
     * @date 2006-2010
     */
    class FBDicString : public FBDic<> {
    public:
	typedef FBDic<> FBDic_t;

	inline FBDicString();

	/**
	 * @brief Create a FBDicString object and load a dic from the specified file
	 */
	inline FBDicString( char const* dicFile );

	inline void setKeyValueDelimiter( uchar c );

	inline uchar const* lookup( wchar_t* key ) const;

	/**
	 * @brief return the string value annotated to the entry with the given perfect hash value in the FW dic
	 */
	inline uchar const* getAnnByPerfHashValue_FW( size_t perfHashValue ) const;

	/**
	 * @brief return the string value annotated to the entry with the given perfect hash value in the BW dic
	 */
	inline uchar const* getAnnByPerfHashValue_BW( size_t perfHashValue ) const;

	/**
	 * @brief return the string value at the specified offset in the annotation buffer
	 *
	 * 
	 */
	inline uchar const* getAnnByOffset( size_t offset ) const;


	inline void loadFromFile( char const* dicFile );
	inline void loadFromStream( FILE* fi );

	inline void writeToFile( char const* dicFile ) const;
	inline void writeToStream( FILE* fo ) const;

	inline void initConstruction();
	inline void finishConstruction();
	inline void compileDic( const char* lexFile );

	inline void doAnalysis() const;

    private:


	static const bits64 magicNumber_ = 432432456;

	class Header {
	public:
	    Header() : 
		magicNumber_( 0 ), 
		sizeOfAnnStrings_( 0 ) {
	    }

	    bits64 getMagicNumber() const {
		return magicNumber_;
	    }
	    size_t getSizeOfAnnStrings() const {
		return sizeOfAnnStrings_;
	    }
	    
	    void set( const FBDicString& mds ) {
		magicNumber_ = mds.magicNumber_;
		sizeOfAnnStrings_ = mds.sizeOfAnnStrings_;
	    }
	private:
	    bits64 magicNumber_;
	    bits64 sizeOfAnnStrings_;
	}; // class Header

	Header header_;

	uchar* annStrings_;
	size_t sizeOfAnnStrings_;

	Hash< uchar >* annHash_;

	uchar keyValueDelimiter_;	

	mutable size_t count_; // is used for counting during printing
    }; // class FBDicString


} // namespace csl

#include "./FBDicString.tcc"

#endif
