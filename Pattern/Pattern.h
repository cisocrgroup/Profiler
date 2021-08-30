#ifndef CSL_PATTERN_H
#define CSL_PATTERN_H CSL_PATTERN_H

#include <string>
#include <iostream>

#include "../Global.h"

namespace csl {

    /**
     * @brief Pattern holds one string substitution alpha --> beta which is allowed in a certain context.
     *
     * The Pattern object simply has two string components left and right. If both are empty, the pattern is
     * something like a "neutral element", or a dummy. Then, empty() returns true. It is sometimes needed where
     * the implementation expects a Pattern without there really being one.
     *
     * @author Ulrich Reffle, 2008
     */
    class Pattern {
    public:
	    static const std::wstring UNKNOWN;
	/**
	 * @brief Constructs an empty pattern with left == right == ""
	 */
	inline Pattern();

      /**
       * Create a pattern from a simple l:r expression.
       */
      Pattern(const std::wstring& expr);

	/**
	 * @brief A very self-explanatory constructor.
	 */
	inline Pattern( const std::wstring& left, const std::wstring& right );

	inline bool operator== ( const Pattern& other ) const {
	    return ( ( getLeft() == other.getLeft() ) &&
		     ( getRight() == other.getRight() ) );
	}

	inline bool operator!= ( const Pattern& other ) const {
	    return ! ( *this == other );
	}


	/**
	 * @brief creates an order relation on Pattern objects, mainly to be able t ostore them in std::set etc.
	 *
	 * The order is equivalent to alphabetical order of left sides, in case of equality of the right sides.
	 */
	inline bool operator< ( const Pattern& other ) const {
	    if( getLeft() < other.getLeft() ) {
		return true;
	    }
	    else if( getLeft() > other.getLeft() ) {
		return false;
	    }
	    else return ( getRight() < other.getRight() );
	}

	inline void clear() {
	    left_.clear();
	    right_.clear();
	}

	inline bool isUnknown() const noexcept {
		return getLeft() == UNKNOWN or getRight() == UNKNOWN;
	}

	/**
	 * @brief get the left side of the pattern
	 * @return the left side of the pattern
	 */
	inline const std::wstring& getLeft() const;

	/**
	 * @brief get the right side of the pattern
	 * @return the right side of the pattern
	 */
	inline const std::wstring& getRight() const;

	/**
	 * @brief returns if pattern is "empty"
	 */
	inline bool empty() const {
	    return ( left_.empty() && right_.empty() );
	}

	inline void setLeft( std::wstring const& left );

	inline void setRight( std::wstring const& right );

	/**
	 * @brief Get rid of wordBegin- and wordEnd-markers
	 *
	 */
	inline void strip();

	void print( std::wostream& os = std::wcout ) const;

	inline std::wstring toString() const;

	inline static void setLeftRightDelimiter( wchar_t c ) {
	    leftRightDelimiter_ = c;
	}


	/**
	 * @brief This value determines which symbol is used as delimiter between left and right
	 *        pattern side. NOTE THAT, once you change this, it will affect the output of patterns
	 *        as well as the parsing from pattern files etc.
	 *
	 * This value is initialized with a default value in Pattern.cxx
	 */
	static wchar_t leftRightDelimiter_;

    private:

	/**
	 * @brief The left pattern side
	 */
	std::wstring left_;

	/**
	 * @brief The right pattern side
	 */
	std::wstring right_;

    }; // class Pattern


    Pattern::Pattern() {
    }

    Pattern::Pattern( const std::wstring& left, const std::wstring& right ) :
	left_( left ),
	right_( right ) {
    }

    const std::wstring& Pattern::getLeft() const {
	return left_;
    }

    const std::wstring& Pattern::getRight() const {
	return right_;
    }

    inline void Pattern::setLeft( std::wstring const& left ) {
	left_ = left;
    }

    inline void Pattern::setRight( std::wstring const& right ) {
	right_ = right;
    }

    inline void Pattern::strip() {
	if( (! left_.empty()) && ( left_.at( 0 ) == Global::wordBeginMarker ) ) left_.erase( 0, 1 );
	if( (! left_.empty()) && ( left_.at( left_.size()-1 ) == Global::wordEndMarker ) ) left_.erase( left_.size()-1, 1 );
	if( (! right_.empty()) && ( right_.at( 0 ) == Global::wordBeginMarker ) ) right_.erase( 0, 1 );
	if( (! right_.empty()) && ( right_.at( right_.size()-1 ) == Global::wordEndMarker ) ) right_.erase( right_.size()-1, 1 );
    }

    std::wstring Pattern::toString() const {
	return getLeft() + leftRightDelimiter_ + getRight();
    }


} // eon


#endif
