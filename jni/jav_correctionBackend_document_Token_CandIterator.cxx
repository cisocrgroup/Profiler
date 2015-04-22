#ifndef _Included_jav_correctionBackend_document_Token_CandIterator_cxx
#define _Included_jav_correctionBackend_document_Token_CandIterator_cxx _Included_jav_correctionBackend_document_Token_CandIterator_cxx


#include "./JNITools.h"
#include "./jav_correctionBackend_document_Token_CandIterator.h"
#include "../Document/Document.h"


/*
 * Class:     jav_correctionBackend_document_Token_CandIterator
 * Method:    getCandidatePointer
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_jav_correctionBackend_document_Token_00024CandIterator_getCandidatePointer
(JNIEnv *env, jobject jCallerObject, jlong candItemPointer ) {
    // get candItem-object
    OCRCorrection::Token::CandidateChain* candItem = (OCRCorrection::Token::CandidateChain*)candItemPointer;

    return (jlong)( &( candItem->getCandidate() ) );
}

/*
 * Class:     jav_correctionBackend_document_Token_CandIterator
 * Method:    getNextItemPointer
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_jav_correctionBackend_document_Token_00024CandIterator_getNextItemPointer
(JNIEnv *env, jobject jCallerObject, jlong candItemPointer ) {
    // get candItem-object
    OCRCorrection::Token::CandidateChain* candItem = (OCRCorrection::Token::CandidateChain*)candItemPointer;

    return (jlong)candItem->getNext();
}


#endif
