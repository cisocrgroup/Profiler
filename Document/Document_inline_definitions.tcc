#include <locale>
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
	*isNormal = std::isalnum( str.at( offset ), csl::CSLLocale::Instance() );

	++offset;
	while( offset < str.length() && ( std::isalnum( str.at( offset ), csl::CSLLocale::Instance() ) == *isNormal ) ) {
	    ++offset;
	}
	return offset;
    }


    size_t Document::getBorder( wchar_t const* pos, bool* isNormal ) {
	if( *pos == 0 ) {
	    *isNormal = false;
	    return 0;
	}
        std::locale loc = std::locale::global(std::locale(""));
	// *isNormal = std::isalnum( *pos, csl::CSLLocale::Instance() );
        *isNormal = std::isalnum( *pos, loc);
	size_t length = 1;
	++pos;
        //	while( *pos && ( std::isalnum( *pos, csl::CSLLocale::Instance() ) == *isNormal ) ) {
	while( *pos && ( std::isalnum( *pos, loc ) == *isNormal ) ) {
	    ++pos;
	    ++length;
	}
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


}
