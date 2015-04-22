#ifndef _Included_jav_correctionBackend_document_Document_cxx
#define _Included_jav_correctionBackend_document_Document_cxx

#include "./JNITools.h"
#include "./jav_correctionBackend_document_Document.h"
#include<Document/Document.h>
#include<DocXML/DocXMLWriter.h>

JNIEXPORT jlong JNICALL Java_jav_correctionBackend_document_Document_getTokenPointer
(JNIEnv *env, jobject callerObj, jint index ) {
    // get Document-object
    jclass callerClass = env->GetObjectClass( callerObj );
    jfieldID documentPointerID = env->GetFieldID( callerClass, "pointer_", "J" );
    OCRCorrection::Document* document = (OCRCorrection::Document*)env->GetLongField( callerObj, documentPointerID );

    return (unsigned long) &(document->at( index ) );
}

JNIEXPORT jlong JNICALL Java_jav_correctionBackend_document_Document_getPagePointer
(JNIEnv *env, jobject callerObj, jint index ) {
    // get Document-object
    jclass callerClass = env->GetObjectClass( callerObj );
    jfieldID documentPointerID = env->GetFieldID( callerClass, "pointer_", "J" );
    OCRCorrection::Document* document = (OCRCorrection::Document*)env->GetLongField( callerObj, documentPointerID );

    return (unsigned long) &(document->pagesAt( index ) );
    
}


JNIEXPORT jint JNICALL Java_jav_correctionBackend_document_Document_getNrOfTokens
(JNIEnv *env, jobject callerObj ) {
    // get Document-object
    jclass callerClass = env->GetObjectClass( callerObj );
    jfieldID documentPointerID = env->GetFieldID( callerClass, "pointer_", "J" );
    OCRCorrection::Document* document = (OCRCorrection::Document*)env->GetLongField( callerObj, documentPointerID );

    return document->getNrOfTokens();
}

JNIEXPORT jint JNICALL Java_jav_correctionBackend_document_Document_getNrOfPages
(JNIEnv *env, jobject callerObj ) {
    // get Document-object
    jclass callerClass = env->GetObjectClass( callerObj );
    jfieldID documentPointerID = env->GetFieldID( callerClass, "pointer_", "J" );
    OCRCorrection::Document* document = (OCRCorrection::Document*)env->GetLongField( callerObj, documentPointerID );

    return document->getNrOfPages();
}


/*
 * Class:     jav_correctionBackend_document_Document
 * Method:    native_eraseToken
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_jav_correctionBackend_document_Document_native_1eraseToken
(JNIEnv* env, jobject callerObj, jint indexFrom, jint indexTo ) {
    // get Document-object
    jclass callerClass = env->GetObjectClass( callerObj );
    jfieldID documentPointerID = env->GetFieldID( callerClass, "pointer_", "J" );
    OCRCorrection::Document* document = (OCRCorrection::Document*)env->GetLongField( callerObj, documentPointerID );

    document->eraseToken( (int)indexFrom, (int)indexTo );
    
}


JNIEXPORT void JNICALL Java_jav_correctionBackend_document_Document_clear
(JNIEnv *env, jobject callerObj ) {
    // get Document-object
    jclass callerClass = env->GetObjectClass( callerObj );
    jfieldID documentPointerID = env->GetFieldID( callerClass, "pointer_", "J" );
    OCRCorrection::Document* document = (OCRCorrection::Document*)env->GetLongField( callerObj, documentPointerID );

    document->clear();
}

JNIEXPORT void JNICALL Java_jav_correctionBackend_document_Document_saveAs
(JNIEnv *env, jobject callerObj, jstring filename ) {
    std::wcerr << "OCRC::JNI::Document::saveAs: enter" << std::endl;
    // get Document-object
    jclass callerClass = env->GetObjectClass( callerObj );
    jfieldID documentPointerID = env->GetFieldID( callerClass, "pointer_", "J" );
    OCRCorrection::Document* document = (OCRCorrection::Document*)env->GetLongField( callerObj, documentPointerID );

    std::wcerr << "OCRC::JNI::Document::saveAs: enter" << std::endl;

    // get mbstring from java string
    char const* mbsFilename = (char const*) env->GetStringUTFChars( filename, 0 );

    OCRCorrection::DocXMLWriter writer;

    writer.writeXML( *document, mbsFilename );
    std::wcerr << "OCRC::JNI::Document::saveAs: xml written" << std::endl;
    

    env->ReleaseStringUTFChars( filename, (char*)mbsFilename );
    std::wcerr << "OCRC::JNI::Document::saveAs: bye  " << std::endl;

}

/*
 * Class:     jav_correctionBackend_document_Document
 * Method:    native_saveAsPlaintext
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_jav_correctionBackend_document_Document_native_1saveAsPlaintext
(JNIEnv *env, jobject callerObj, jstring filename ) {
    try { 
	// get Document-object
	jclass callerClass = env->GetObjectClass( callerObj );
	jfieldID documentPointerID = env->GetFieldID( callerClass, "pointer_", "J" );
	OCRCorrection::Document* document = (OCRCorrection::Document*)env->GetLongField( callerObj, documentPointerID );
	
	// get mbstring from java string
	char const* mbsFilename = (char const*) env->GetStringUTFChars( filename, 0 );
	
	std::wofstream of( mbsFilename );
	of.imbue( csl::CSLLocale::Instance() );
	document->dumpToPlaintext( of );
	of.close();
	
	env->ReleaseStringUTFChars( filename, (char*)mbsFilename );
    } catch( std::exception const& exc ) {
	OCRCorrection::JNITools::throwJavaExceptionFromCppException( env, exc );
    }

}

JNIEXPORT void JNICALL Java_jav_correctionBackend_document_Document_native_1saveAsPageSeparatedPlaintext
(JNIEnv *env, jobject callerObj, jstring filename) {
    try { 
	// get Document-object
	jclass callerClass = env->GetObjectClass( callerObj );
	jfieldID documentPointerID = env->GetFieldID( callerClass, "pointer_", "J" );
	OCRCorrection::Document* document = (OCRCorrection::Document*)env->GetLongField( callerObj, documentPointerID );
	
	// get mbstring from java string
	char const* mbsFilename = (char const*) env->GetStringUTFChars( filename, 0 );
	
	document->dumpToPageSeparatedPlaintext( mbsFilename );
	
	env->ReleaseStringUTFChars( filename, (char*)mbsFilename );
    } catch( std::exception const& exc ) {
	OCRCorrection::JNITools::throwJavaExceptionFromCppException( env, exc );
    }
}

#endif
