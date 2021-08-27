#ifndef OCRCORRECTION_CREATEXML_H
#define OCRCORRECTION_CREATEXML_H OCRCORRECTION_CREATEXML_H

#include<stdexcept>
#include "../Document/Document.h"
#include "../AlphaScore/AlphaScore.h"
#include "../Stopwatch.h"
#include "../MSMatch/MSMatch.h"


namespace OCRCorrection {

    /**
     *
     *
     */
    class CreateXML {
    public:
	CreateXML();

	~CreateXML();

	/**
	 * @brief Create a new document object from the given text file
	 */
	void newDocument( const char* filename );

	/**
	 * @brief Do this before calling newDocument()
	 */
	void initMSMatch( size_t levDistance, char const* FBDic );

	/**
	 * needed by jni
	 */
	Document* getDocumentPointer();

    private:
	Document document_;
	csl::MSMatch< csl::FW_BW >* msMatch_;
	const csl::MinDic<>* dictionary_;
    };


} // ns OCRCorrection

#endif
