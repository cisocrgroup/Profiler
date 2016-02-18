#ifndef CSL_INTERPRETATION_H
#define CSL_INTERPRETATION_H CSL_INTERPRETATION_H

#include "./Instruction.h"

namespace csl {
    /**
     * @brief This class is something like a container class to store one interpretation of some
     * string as the spelling variant (and/or erroneous variant) of some other string.
     *
     * Usually this class will be the answer object (or part thereof) of a query to ErrDic or Vaam.
     *
     * @author Ulrich Reffle 2008
     */
    class Interpretation {
    public:

	/**
	 * @name Constructors/ Destructors
	 */
	// @{

	Interpretation();

	// @} // END Constructors/ Destructors


	/**
	 * @brief set up a comparison based on the sum of levenshtein or pattern edits.
	 *
	 * Give the levenshtein operations a marginally higher punishment, so that
	 * if the sums are equal, the one with less lev. operations and more pattern 
	 * operations will win.
	 */
	bool operator<( Interpretation const& other ) const;

	/**
	 * @name Getters
	 */
	// @{

	/**
	 * @brief returns a reference to the plain string
	 */
	std::wstring& getWord();

	/**
	 * @brief returns a const reference to the plain string
	 */
	std::wstring const& getWord() const;

	/**
	 * @brief returns the so-called @c baseWord from the dictionary, that was changed into a variant with the given @c Instruction
	 *
	 * If the instruction is empty, then @c getWord() == @c getBaseWord()
	 */
	std::wstring const& getBaseWord() const;

	/**
	 * @brief returns the levenshtein distance to the query
	 */
	size_t getLevDistance() const;

	/**
	 * @brief returns the score that was annotated with the @c baseWord in the dictionary.
	 */
	size_t getBaseWordScore() const;

	/**
	 * @brief returns the instruction that was used to turn @c baseWord into @c word 
	 * @deprecated Use getHistInstruction instead!
	 */
	Instruction& getInstruction();

	/**
	 * @brief const version of getInstruction()
	 * @deprecated Use getHistInstruction instead!
	 */
	Instruction const& getInstruction() const;

	/**
	 * @brief More verbose name for getInstruction().
	 */
	Instruction& getHistInstruction();

	/**
	 * @brief const version of getHistInstruction()
	 */
	Instruction const& getHistInstruction() const;
	
	// @} // END Getters
	
	/**
	 * @name Setters
	 */
	// @{

	void setWord( std::wstring const& w );

	void setBaseWord( std::wstring const& w );

	void setLevDistance( size_t dist );

	void setBaseWordScore( size_t score );

	/**
	 * @brief erase all values from the object and bring it into a state as if just created.
	 */
	void clear();

	// @} // END Setters

	size_t parseFromString( std::wstring const& str, size_t offset = 0 );

	/**
	 * @name Pretty-print
	 */
	// @{
	/**
	 * @brief prints a string-representation of the interpretation to stdout or to another std::wstream 
	 * specified as argument.
	 */
	void print( std::wostream& os = std::wcout ) const;
	
	std::wstring toString() const;

	/**
	 * @brief prints another useful format of the interpretation, more in the INL tradition
	 */
	void print_v2( std::wostream& os = std::wcout ) const;

	// @} // END Pretty-Print


    private:
	/// the suggested correction candidate
	std::wstring word_;

	/// the underlying word in the modern dictionary
	std::wstring baseWord_;

	/// the corresponding Instruction: word = baseWord + instruction
	Instruction instruction_;

	Instruction instructionOCR_;

	/**
	 * @brief 
	 *
	 */
	size_t levDistance_;

	/**
	 * @brief A score for the baseWord as was annotated in the dictionary or some other resource
	 *
	 * Most likely this would be a frequency score.
	 */
	int baseWordScore_;


	    
    }; // class Interpretation


}

namespace std {
    inline wostream& operator<<( wostream& os, csl::Interpretation const& obj ) {
	obj.print( os );
	return os;
    }
}

#endif
