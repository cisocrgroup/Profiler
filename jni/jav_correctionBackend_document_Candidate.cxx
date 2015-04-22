#ifndef _Included_jav_correctionBackend_document_Candidate_cxx
#define _Included_jav_correctionBackend_document_Candidate_cxx

#include "./JNITools.h"
#include "./jav_correctionBackend_document_Candidate.h"
#include "../Document/Document.h"

JNIEXPORT jstring JNICALL Java_jav_correctionBackend_document_Candidate_getStr
( JNIEnv * env, jobject callerObj ) {

    // get Cand-object
    csl::DictSearch::Interpretation* candidate = OCRCorrection::JNITools::getObject< OCRCorrection::Candidate* >( env, callerObj );
    
    jstring ret ;
    OCRCorrection::JNITools::stringToJstring( env, ret, candidate->getWord() );
    return ret;

}

JNIEXPORT jstring JNICALL Java_jav_correctionBackend_document_Candidate_getInterpretation
( JNIEnv * env, jobject callerObj ) {

    // get Cand-object
    OCRCorrection::Candidate* candidate = OCRCorrection::JNITools::getObject< OCRCorrection::Candidate* >( env, callerObj );
    
    jstring ret ;

    OCRCorrection::JNITools::stringToJstring( env, ret, candidate->toString() );
    return ret;
    
}

JNIEXPORT jfloat JNICALL Java_jav_correctionBackend_document_Candidate_getDlev
( JNIEnv * env, jobject callerObj ) {
    // get Cand-object
    OCRCorrection::Candidate* candidate = OCRCorrection::JNITools::getObject< OCRCorrection::Candidate* >( env, callerObj );

    return candidate->getDlev();
}

JNIEXPORT jfloat JNICALL Java_jav_correctionBackend_document_Candidate_getFrequency
( JNIEnv * env, jobject callerObj ) {
    // get Cand-object
    OCRCorrection::Candidate* candidate = OCRCorrection::JNITools::getObject< OCRCorrection::Candidate* >( env, callerObj );

    return candidate->getFrequency();
}

#endif
