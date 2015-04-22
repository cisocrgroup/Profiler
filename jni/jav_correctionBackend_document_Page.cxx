#ifndef _Included_jav_correctionBackend_document_Page_cxx
#define _Included_jav_correctionBackend_document_Page_cxx _Included_jav_correctionBackend_document_Page_cxx

#include "./JNITools.h"
#include "./jav_correctionBackend_document_Page.h"
#include "../Document/Document.h"

JNIEXPORT jstring JNICALL Java_jav_correctionBackend_document_Page_getImageFile
(JNIEnv *env, jobject callerObj) {
    try {
	// get Page-object
	jclass callerClass = env->GetObjectClass( callerObj );
	jfieldID pagePointerID = env->GetFieldID( callerClass, "pointer_", "J" );
	OCRCorrection::Document::Page* page = (OCRCorrection::Document::Page*)env->GetLongField( callerObj, pagePointerID );

	jstring ret;
	OCRCorrection::JNITools::stringToJstring( env, ret, page->getImageFile() );
	return ret;
    } catch( std::exception exc ) {
	OCRCorrection::JNITools::throwJavaExceptionFromCppException( env, exc );
	jstring ret;
	OCRCorrection::JNITools::stringToJstring( env, ret, "" );
	return ret;
    }
}

JNIEXPORT jint JNICALL Java_jav_correctionBackend_document_Page_getOffsetBegin
( JNIEnv *env, jobject callerObj ) {
    // get Page-object
    jclass callerClass = env->GetObjectClass( callerObj );
    jfieldID pagePointerID = env->GetFieldID( callerClass, "pointer_", "J" );
    OCRCorrection::Document::Page* page = (OCRCorrection::Document::Page*)env->GetLongField( callerObj, pagePointerID );

    return page->getOffsetBegin();
}

JNIEXPORT jint JNICALL Java_jav_correctionBackend_document_Page_getOffsetEnd
( JNIEnv *env, jobject callerObj ) {
    // get Page-object
    jclass callerClass = env->GetObjectClass( callerObj );
    jfieldID pagePointerID = env->GetFieldID( callerClass, "pointer_", "J" );
    OCRCorrection::Document::Page* page = (OCRCorrection::Document::Page*)env->GetLongField( callerObj, pagePointerID );

    return page->getOffsetEnd();
}


#endif
