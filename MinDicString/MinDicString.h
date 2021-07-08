#ifndef FSDICT_MINDICSTRING_H
#define FSDICT_MINDICSTRING_H FSDICT_MINDICSTRING_H

#include "../Global.h"
#include "../MinDic/MinDic.h"
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
    class MinDicString : public MinDic<> {
    public:
	typedef MinDic<> MinDic_t;

	inline MinDicString();

	/**
	 * @brief Create a MinDicString object and load a dic from the specified file
	 */
	inline MinDicString( char const* dicFile );

	inline void setKeyValueDelimiter( uchar c );

	inline uchar const* lookup( wchar_t* key ) const;

	/**
	 * @brief return the string value annotated to the entry with the given perfect hash value
	 */
	inline uchar const* getAnnByPerfHashValue( size_t perfHashValue ) const;

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
	inline void printDic() const;

	inline void doAnalysis() const;

    private:
	inline void printDic_rec( int pos, int depth, size_t perfHashValue ) const;


	static const bits64 magicNumber_ = 2343572;

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

	    void set( const MinDicString& mds ) {
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
    }; // class MinDicString


} // namespace fsdict

#include "./MinDicString.tcc"

#endif
