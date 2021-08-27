#ifndef OCRC_ALIGNED_TXT_WRITER_H
#define OCRC_ALIGNED_TXT_WRITER_H OCRC_ALIGNED_TXT_WRITER_H

#include<Document/Document.h>

namespace OCRCorrection {

    class AlignedTXTWriter {
    public:
	void writeAlignedTXT( Document const& doc ) {
	    if( doc.hasPages() ) {
		std::wcerr << "OCRC::AlignedTXTWriter: WARNING: Document has page-information,"
			   << "which must be ignored for alignged txt output." << std::endl;
	    }
	    for( Document::const_iterator token = doc.begin(); token != doc.end(); ++token ) {

	    }
	}
    };

} // eon


#endif
