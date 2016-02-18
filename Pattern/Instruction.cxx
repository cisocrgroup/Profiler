#ifndef CSL_INSTRUCTION_CXX
#define CSL_INSTRUCTION_CXX CSL_INSTRUCTION_CXX

#include "./Instruction.h"
#include "Utils/Utils.h"

namespace csl {

    Instruction::Instruction() :
	status_(REGULAR) {
    }

    bool Instruction::operator==( Instruction const& other ) const {
	if( size() != other.size() ) return false;

	const_iterator this_it = begin();
	const_iterator other_it = other.begin();
	for( ; this_it != end(); ++this_it, other_it ) {
	    if( ! ( *this_it == *other_it ) ) return false;
	}

	return true;
    }

    void Instruction::setDestroyed(  bool b ) {
	status_ = b ? DESTROYED : REGULAR;
    }

    bool Instruction::isDestroyed() const {
	return ( status_ == DESTROYED );
    }

    void Instruction::print(  std::wostream& os ) const {
	os<<"[";
	for( const_iterator it = begin(); it != end(); ++it  ) {
	    it->print( os );
	}
	os<<"]";
    }

    std::wstring Instruction::toString() const {
	std::wstring ret = L"[";
	for( const_iterator it = begin(); it != end(); ++it  ) {
	    ret += it->toString();
	}
	ret += L"]";

	return ret;
    }

    size_t Instruction::parseFromString( std::wstring const& str, size_t offset ) {
	if( ! this->empty() ) throw exceptions::badInput( "csl::Instruction::parseFromString: Instruction not empty" );

	if( str.empty() ) throw exceptions::badInput( "csl::Instruction::parseFromString: Empty string as input" );

	if( str.at( offset ) != '[' ) {
	    throw exceptions::badInput(
		std::string( "csl::Instruction::parseFromString: Found no opening square bracket: string='" ) +
		OCRCorrection::Utils::utf8(str)
		);
	}
	offset += 1;

	csl::PosPattern pp;
	while( str.at( offset ) != ']' ) {
	    pp.clear();
	    offset = pp.parseFromString( str, offset );
	    push_back( pp );
	}
	if( str.at( offset ) != ']' ) throw exceptions::badInput( "csl::Instruction::parseFromString: Found no closing square bracket" );
	offset += 1;

	return offset;
    }

    void Instruction::applyTo( std::wstring* str, int direction ) const {
	if( direction == 1 ) {
	    // iterate through PosPatterns in reverse order: that way the positions remain intact
	    // even if the Patterns change the word length
	    for( const_reverse_iterator rit = rbegin(); rit != rend(); ++rit ) {
		// std::wcout<<"pos="<< rit->getPosition()<<"left="<<rit->getLeft()<<",llength="<< rit->getLeft().length()<<", right="<< rit->getRight()<<std::endl; // DEBUG

		Pattern pat = *rit;
		pat.strip(); // get rid of word-boundary markers
		str->replace( rit->getPosition(), pat.getLeft().length(), pat.getRight() );
	    }
	}
	else if( direction == -1 ) {
	    // iterate through PosPatterns in standard, left-to-right order: that way the positions remain intact
	    // even if the Patterns change the word length
	    for( const_iterator it = begin(); it != end(); ++it ) {
		//std::wcout<<"pos="<< it->getPosition()<<"left="<<it->getLeft()<<",length="<< it->getLeft().length()<<", right="<< it->getRight()<<std::endl; // DEBUG
		Pattern pat = *it;
		pat.strip(); // get rid of word-boundary markers
		str->replace( it->getPosition(), pat.getRight().length(), pat.getLeft() );
	    }
	}
	else {
	    throw exceptions::cslException( "csl::Instruction::applyTo: invalid direction" );
	}

    }

    bool Instruction::containsPattern( Pattern const& pat ) const {
	for( const_iterator it = begin(); it != end(); ++it  ) {
	    if( pat == *it ) return true;
	}
	return false;
    }


} // eon

#endif
