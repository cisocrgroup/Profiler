#ifndef OCRCORRECTION_DOCUMENT_TCC__
#define OCRCORRECTION_DOCUMENT_TCC__

#include <locale>
#include <unicode/uchar.h>

namespace OCRCorrection {
    Token& Document::at( size_t i ) {
	return *( tokens_.at( i ) );
    }

    const Token& Document::at( size_t i ) const {
	return *( tokens_.at( i ) );
    }

    size_t Document::getNrOfTokens() const {
	return tokens_.size();
    }

    size_t Document::getNrOfPages() const {
	return pages_.size();
    }

    bool Document::empty() const {
	return ( getNrOfTokens() == 0 );
    }


    size_t Document::findBorder( std::wstring const& str, size_t offset, bool* isNormal ) {
	if( offset == str.length() ) {
	    *isNormal = false;
	    return std::wstring::npos;
	}
	*isNormal = isWord(str.at(offset));

	++offset;
	while( offset < str.length() && ( isWord(str.at(offset)) == *isNormal)) {
	    ++offset;
	}
	return offset;
    }


    size_t Document::getBorder( wchar_t const* pos, bool* isNormal ) {
	if( *pos == 0 ) {
	    *isNormal = false;
	    return 0;
	}
        const wchar_t *str = pos;
        *isNormal = isWord(*pos);
	size_t length = 1;
	++pos;
        while( *pos && (isWord(*pos) == *isNormal)) {
	    ++pos;
	    ++length;
	}
        // std::wcerr << "getBorder(\"" << str << "\", " << *isNormal << "): '"
        //            << std::wstring(str, length) << "'" << std::endl;
	return length;
    }

    void Document::cleanupWord( std::wstring* word, std::wstring* pre, std::wstring* post ) {
	std::wstring removeThem = L".;,:!?-_|¬\"'&^/()[]{}=# ";
	removeThem += 0x201e; // komisches Anführungszeichen unten
	removeThem += 0xbb;

	//std::wcerr<<"Vorher:"<<word<<std::endl;

	*pre = L"";
	*post = L"";

	while( !word->empty() && ( removeThem.find( word->at( 0 ) ) != removeThem.npos ) ) {
	    *pre += word->at( 0 );
	    word->erase( 0, 1 );
	}

	while( !word->empty() && ( removeThem.find( *( word->rbegin() ) ) != removeThem.npos ) ) {
	    *post = *post + *( word->rbegin() );
	    word->erase( word->end() - 1 );
	}

    }

        bool
        Document::isWord(wchar_t c) {
                //std::wcerr << "type('" << c << "'): " << u_charType(c);
                switch (u_charType(c)) {     // all cases fall through
                case U_UPPERCASE_LETTER:     // Lu
                case U_LOWERCASE_LETTER:     // Al
                case U_TITLECASE_LETTER:     // Lt
                case U_MODIFIER_LETTER:      // Lm
                case U_OTHER_LETTER:         // Lo
                case U_DECIMAL_DIGIT_NUMBER: // Nd
                case U_LETTER_NUMBER:        // Nl
                case U_OTHER_NUMBER:         // No
                case U_NON_SPACING_MARK:     // Mn
                        //std::wcerr << " = true" << std::endl;
                        return true;
                default:
                        //std::wcerr << " = false" << std::endl;
                        return false;
                }
        }
        bool
        Document::isSpace(wchar_t c) {
                return u_isspace(c);
        }
}
#endif /* OCRCORRECTION_DOCUMENT_TCC__ */
