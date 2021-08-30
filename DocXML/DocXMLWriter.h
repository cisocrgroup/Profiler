#ifndef OCRC_DOCXMLWRITER_H
#define OCRC_DOCXMLWRITER_H OCRC_DOCXMLWRITER_H

#include <string>
#include <locale>

#include "../Document/Document.h"

namespace OCRCorrection {

	class Document;
    std::wstring xml_escape( std::wstring const& input ); // forward declaration


    /**
     * @brief Dumps a Document-object to DocXML format
     */
    class DocXMLWriter {
    public:

	/**
	 * @brief writes a DocXML representation of the given document t othe given output stream
	 *
	 * @param[in]  doc     a const reference to a document
	 * @param[out] xml_out a reference to an output stream
	 *
	 * Note that all locale-dependent issues are in your hands. Be sure that your wostream-object
	 * treats special symbols, numbers etc the way you want and expect it.
	 */
	void writeXML( Document const& doc, std::wostream& xml_out ) const;


	void writeXML( Document const& doc, std::string const& filename ) const;


    private:
	using PageIterator = Document::const_PageIterator;
	using TokenIterator = Document::const_iterator;
	void writePages(PageIterator b, PageIterator e, size_t& count, std::wostream& xml_out) const;
	void writeTokens(TokenIterator b, TokenIterator e, size_t& count, std::wostream& xml_out) const;
	void writeAdaptiveTokens(const Document& doc, std::wostream& xml_out) const;
    };

    inline std::wstring xml_escape( std::wstring const& input ) {
	std::wstring str = input;
	size_t pos = 0;
	while( ( pos = str.find( '&', pos ) ) != std::wstring::npos ) {
	    str.replace( pos, 1, L"&amp;" );
	    ++pos;
	}
	pos = 0;
	while( ( pos = str.find( '>', pos ) ) != std::wstring::npos ) {
	    str.replace( pos, 1, L"&gt;" );
	    ++pos;
	}
	pos = 0;
	while( ( pos = str.find( '<', pos ) ) != std::wstring::npos ) {
	    str.replace( pos, 1, L"&lt;" );
	    ++pos;
	}
	return str;
    }

} // eon

#endif
