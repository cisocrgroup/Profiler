#ifndef OCRC_TXTREADER_H
#define OCRC_TXTREADER_H OCRC_TXTREADER_H

#include "../Document/Document.h"

namespace OCRCorrection {


    class TXTReader {
    public:
	/**
	 * @param[in]  txtFile
	 * @param[out] document object that is filled by the parser
	 */
	void parse( char const* txtFile, Document* document );

	/**
	 * @brief This method parses a whole directory of txt files into a document.
	 *
	 * @param[in]  Directory containing txt files
	 * @param[in]  Directory containing the respective images
	 * @param[out] document object that is filled by the parser
	 */
	void parseDir( std::string const& txtDir, std::string const& imageDir, Document* doc );

    private:



    }; // class TXTReader







} // eon

#endif
