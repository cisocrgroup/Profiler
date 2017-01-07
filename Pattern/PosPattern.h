#ifndef CSL_POSPATTERN
#define CSL_POSPATTERN CSL_POSPATTERN

#include<sstream>
#include "Global.h"
#include "Pattern.h"


namespace csl {

    /**
     * @brief the Pattern class is simply enriched by a member variable indicating a position inside a word
     */
    class PosPattern : public Pattern {
    public:
	/**
	 * @brief creates an empty Pattern at position 0
	 */
	inline PosPattern();

	inline PosPattern( const std::wstring& left, const std::wstring& right, size_t position );
	static inline PosPattern Unknown() {
		return {Pattern::UNKNOWN, Pattern::UNKNOWN, 0};
	}

	inline bool operator==( PosPattern const& other ) const;

	inline void clear();

	/**
	 * @brief returns if pattern is "empty"
	 */
	inline bool empty() const;

	inline size_t getPosition() const;

	inline void setPosition( size_t p );

	inline void print( std::wostream& os = std::wcout ) const;

	inline std::wstring toString() const;

	/**
	 * @brief reads Pattern data structure from the given string.
	 * @param offset start at this position of the string. Default is 0
	 * @return A new offset pointing to the first character not used for the parse
	 */
	inline size_t parseFromString( std::wstring const& str, size_t offset = 0 );



    private:
	size_t position_;
    }; // class PosPattern

    PosPattern::PosPattern() :
	Pattern(),
	position_( 0 ) {
    }

    PosPattern::PosPattern( const std::wstring& left, const std::wstring& right, size_t position ) :
 	Pattern( left, right ),
 	position_( position ) {
    }

    bool PosPattern::operator==( PosPattern const& other ) const {
	return ( Pattern::operator==( other ) && ( getPosition() == other.getPosition() ) );
    }

    void PosPattern::clear() {
	Pattern::clear();
	position_ = 0;
    }

    inline bool PosPattern::empty() const {
	return ( Pattern::empty() &&
		 ( position_ == 0 )
	    );
    }

    size_t PosPattern::getPosition() const {
	return position_;
    }

    void PosPattern::setPosition( size_t p ) {
	position_ = p;
    }

    void PosPattern::print( std::wostream& os ) const {
	os << "(";
	Pattern::print( os );
	os << "," << getPosition() << ")";

    }

    std::wstring PosPattern::toString() const {
	//return std::wstring( L"(" ) + Pattern::toString() + L"," + swprintf( L"%ls", getPosition() ) + L")";
	std::wostringstream oss;
	print( oss );
	oss.flush();
	return oss.str();
    }

    inline size_t PosPattern::parseFromString( std::wstring const& str, size_t offset ) {
	size_t leftBracket = offset;
	if( str.at( leftBracket ) != '(' ) throw exceptions::badInput( "csl::PosPattern::parseFromString: Found no opening bracket" );
	offset += 1;

	size_t leftRightDelimiter = str.find( Pattern::leftRightDelimiter_, offset );
	if( leftRightDelimiter == str.npos ) throw exceptions::badInput( "csl::Pattern::parseFromString: Found no leftRightDelimiter" );

	offset = leftRightDelimiter  + 1;
	size_t comma = str.find( ',', offset );
	if( comma == str.npos ) throw exceptions::badInput( "csl::Pattern::parseFromString: Found no comma" );

	offset = comma  + 1;
	size_t rightBracket = str.find( ')', offset );
	if( rightBracket == str.npos ) throw exceptions::badInput( "csl::Pattern::parseFromString: Found no closing bracket" );


	setLeft( str.substr( leftBracket + 1, leftRightDelimiter - leftBracket - 1 ) );
	setRight( str.substr( leftRightDelimiter + 1, comma - leftRightDelimiter - 1 ) );
	wchar_t* dummy = 0;
	setPosition( wcstol( str.substr( comma + 1, rightBracket - comma - 1 ).c_str(), &dummy, 10 ) );

	return rightBracket + 1;
    }


} // eon

namespace std {
    inline wostream& operator<<( wostream& os, csl::PosPattern const& obj ) {
	obj.print( os );
	return os;
    }
}

#endif
