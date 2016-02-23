#ifndef CSL_ALPHABET_H
#define CSL_ALPHABET_H CSL_ALPHABET_H

#include <iostream>
#include<string>
#include<vector>

#include "../Global.h"

namespace csl {

    /**
     *
     * @author Uli Reffle
     * @date 2007
     */
    class CSL_DECLSPEC Alphabet {
    private:
	std::vector< wchar_t > allChars_;

	const bits64 magicNumber_;

	class Header {
	public:
	    bits64 magicNumber_;
	    bits64 size_;
	};

	Header header_;

	std::vector<bool> hasChar_;
	
    public:


	Alphabet();

	typedef std::vector<wchar_t>::const_iterator ConstCharIterator;
	const ConstCharIterator begin() const {
	    return allChars_.begin();
	}
	
	ConstCharIterator end() const {
	    return allChars_.end();
	}


	/**
	 * Adds a new char to the Alphabet
	 */
	void addChar( wchar_t c );

	bool hasChar( wchar_t c ) const;

	void initConstruction();

	void finishConstruction();

	/**
	 * Loads a dumped Alphabet from the given stream
	 */
	void loadFromStream( FILE* fi );
	
	/**
	 * Dumps the Alphabet in its current state to the given stream
	 */
	void writeToStream( FILE* fo ) const;

	/**
	 * Returns the alphabet size
	 */
	inline size_t size() const {
	    return allChars_.size();
	}
    };

} // end of namespace csl
#endif
