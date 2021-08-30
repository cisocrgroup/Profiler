#ifndef OCRC_ALIGNED_TXT_READER_H
#define OCRC_ALIGNED_TXT_READER_H OCRC_ALIGNED_TXT_READER_H

#include <dirent.h>
#include <locale>

#include "../Document/Document.h"

namespace OCRCorrection {


    /**
     * @brief A reader for aligned files of OCR and Groundtruth, of the form <wOrig>\t<wOCR>\n.
     *        This format is used for evaluations.
     *
     * As mentioned above, the expected file format is one line per word:
     * <wOrig>\t<wOCR>\n
     *
     * To indicate newlines, use
     * ###NEWLINE###\t###NEWLINE###\n
     *
     * At the CIS align tool ocr_align_dirs.pl, use the flag --with-newlines in order to keep newlines
     * and code them in the way specified above.
     */
    class AlignedTXTReader {
    public:

	AlignedTXTReader();
	
	/**
	 * @param[in]  xmlFile
	 * @param[out] document object that is filled by the parser
	 */
	void parseSingleFile( std::string const& xmlFile, Document* document );
	
	/**
	 * @brief This method parses a whole directory of align_txt files into a document.
	 *
	 * @param[in]  Directory containing aligned txt files
	 * @param[in]  Directory containing the respective images
	 * @param[out] document object that is filled by the parser
	 */
	void parseDir( std::string const& init_xmlDir, std::string const& imageDir, Document* doc );
	
    private:
	/**
	 * @param[in]  xmlFile
	 * @param[out] document object that is filled by the parser
	 */
	void parse( std::string const& xmlFile, Document* document );


	std::locale locale_;

    }; // class AlignedTXTReader

} // eon


#endif
