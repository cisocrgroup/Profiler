#ifndef _Included_jav_correctionBackend_document_Token_cxx
#define _Included_jav_correctionBackend_document_Token_cxx

#include<iostream>
#include "./jav_correctionBackend_document_Token.h"
#include "../Document/Document.h"
#include "./JNITools.h"

JNIEXPORT jint JNICALL Java_jav_correctionBackend_document_Token_getCoordinate_1left
(JNIEnv *env, jobject jCallerObj) {
	// get Token-object
  try {
	OCRCorrection::Token* token = OCRCorrection::JNITools::getObject< OCRCorrection::Token* >( env, jCallerObj );
	const OCRCorrection::TokenImageInfoBox* b = token->getTokenImageInfoBox();
	if( b == NULL ) {
	  return -1;
	}
	else {
	  return b->getCoordinate_Left();
	}
  } catch( std::exception exc ) {
      	OCRCorrection::JNITools::throwJavaExceptionFromCppException( env, exc ); 
	return -1;
  }
}

JNIEXPORT jint JNICALL Java_jav_correctionBackend_document_Token_getCoordinate_1right
(JNIEnv *env, jobject jCallerObj) {
  // get Token-object
  try {
    OCRCorrection::Token* token = OCRCorrection::JNITools::getObject< OCRCorrection::Token* >( env, jCallerObj );
    const OCRCorrection::TokenImageInfoBox* b = token->getTokenImageInfoBox();
    if( b == NULL ) {
      return -1;
    }
    else {
      return b->getCoordinate_Right();
    }
  } catch( std::exception exc ) {
    	OCRCorrection::JNITools::throwJavaExceptionFromCppException( env, exc ); 
	return -1;
  }
}

JNIEXPORT jint JNICALL Java_jav_correctionBackend_document_Token_getCoordinate_1top
(JNIEnv *env, jobject jCallerObj) {
  // get Token-object
  try {
    OCRCorrection::Token* token = OCRCorrection::JNITools::getObject< OCRCorrection::Token* >( env, jCallerObj );
    const OCRCorrection::TokenImageInfoBox* b = token->getTokenImageInfoBox();
    if( b == NULL ) {
      return -1;
    }
    else {
      return b->getCoordinate_Top();
    }
  } catch( std::exception exc ) {
    	OCRCorrection::JNITools::throwJavaExceptionFromCppException( env, exc ); 
	return -1;
  }
}

JNIEXPORT jint JNICALL Java_jav_correctionBackend_document_Token_getCoordinate_1bottom
(JNIEnv *env, jobject jCallerObj) {
  // get Token-object
  try {
    OCRCorrection::Token* token = OCRCorrection::JNITools::getObject< OCRCorrection::Token* >( env, jCallerObj );
    const OCRCorrection::TokenImageInfoBox* b = token->getTokenImageInfoBox();
    if( b == NULL ) {
      return -1;
    }
    else {
      return b->getCoordinate_Bottom();
    }
  } catch( std::exception exc ) {
      	OCRCorrection::JNITools::throwJavaExceptionFromCppException( env, exc ); 
	return -1;
  }
}

JNIEXPORT jboolean JNICALL Java_jav_correctionBackend_document_Token_hasTokenImageInfoBox
(JNIEnv * env, jobject jCallerObj ) {
    OCRCorrection::Token* token = OCRCorrection::JNITools::getObject< OCRCorrection::Token* >( env, jCallerObj );
    OCRCorrection::TokenImageInfoBox const* b = token->getTokenImageInfoBox();
    if( b != NULL ) {
	return JNI_TRUE;
    }
    else {
	return JNI_FALSE;
    }
}

JNIEXPORT jstring JNICALL Java_jav_correctionBackend_document_Token_getImageFileName
(JNIEnv *env, jobject jCallerObj) {
  try {
    // get Token-object
    OCRCorrection::Token* token = OCRCorrection::JNITools::getObject< OCRCorrection::Token* >( env, jCallerObj );
    OCRCorrection::Document::Page const& page = token->getDocument().pagesAt( token->getPageIndex() );
    jstring ret;
    OCRCorrection::JNITools::stringToJstring( env, ret, page.getImageFile() );

    return ret;
  } catch( std::exception exc ) {
      	OCRCorrection::JNITools::throwJavaExceptionFromCppException( env, exc );
	jstring ret;
	OCRCorrection::JNITools::stringToJstring( env, ret, "" );
	return ret;
  }
}

JNIEXPORT void JNICALL Java_jav_correctionBackend_document_Token_setWCorr
(JNIEnv *env, jobject jCallerObject, jstring w ) {
	// get Token-object
	OCRCorrection::Token* token = OCRCorrection::JNITools::getObject< OCRCorrection::Token* >( env, jCallerObject );

	std::wstring wstr;
	OCRCorrection::JNITools::jstringToWcstring( env, wstr, w );
	token->setWCorr( wstr.c_str() );

}

JNIEXPORT jstring JNICALL Java_jav_correctionBackend_document_Token_getWCorr
( JNIEnv *env, jobject jCallerObject ) {
	// get Token-object
	OCRCorrection::Token* token = OCRCorrection::JNITools::getObject< OCRCorrection::Token* >( env, jCallerObject );

	jstring ret;
	OCRCorrection::JNITools::stringToJstring( env, ret, token->getWCorr() );
	return ret;
}

JNIEXPORT jstring JNICALL Java_jav_correctionBackend_document_Token_getWOCR
( JNIEnv *env, jobject jCallerObject ) {
	// get Token-object
	OCRCorrection::Token* token = OCRCorrection::JNITools::getObject< OCRCorrection::Token* >( env, jCallerObject );

	jstring ret;
	OCRCorrection::JNITools::stringToJstring( env, ret, token->getWOCR() );
	return ret;

}

/*
 * Class:     jav_correctionBackend_document_Token
 * Method:    getWOCR_lc
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_jav_correctionBackend_document_Token_getWOCR_1lc
( JNIEnv *env, jobject jCallerObject ) {
	// get Token-object
	OCRCorrection::Token* token = OCRCorrection::JNITools::getObject< OCRCorrection::Token* >( env, jCallerObject );

	jstring ret;
	OCRCorrection::JNITools::stringToJstring( env, ret, token->getWOCR_lc() );
	return ret;

}



JNIEXPORT jstring JNICALL Java_jav_correctionBackend_document_Token_getWDisplay
( JNIEnv *env, jobject jCallerObject ) {
	// get Token-object
	OCRCorrection::Token* token = OCRCorrection::JNITools::getObject< OCRCorrection::Token* >( env, jCallerObject );

	jstring ret;
	OCRCorrection::JNITools::stringToJstring( env, ret, token->getWDisplay() );
	return ret;

}


/*
 * Class:     jav_correctionBackend_document_Token
 * Method:    getTopCandidatePointer
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_jav_correctionBackend_document_Token_getTopCandidatePointer
(JNIEnv *env, jobject jCallerObject ) {
	// get Token-object
	OCRCorrection::Token* token = OCRCorrection::JNITools::getObject< OCRCorrection::Token* >( env, jCallerObject );

	if( token->hasTopCandidate() ) {
	    return (jlong)&( token->getTopCandidate() );
	}
	else {
	    return 0;
	}
}

JNIEXPORT jboolean JNICALL Java_jav_correctionBackend_document_Token_isCorrected
(JNIEnv *env, jobject jCallerObject ) {
	// get Token-object
	OCRCorrection::Token* token = OCRCorrection::JNITools::getObject< OCRCorrection::Token* >( env, jCallerObject );

	return token->isCorrected();

}

JNIEXPORT void JNICALL Java_jav_correctionBackend_document_Token_setCorrected
(JNIEnv *env, jobject jCallerObject, jboolean b ) {
	// get Token-object
	OCRCorrection::Token* token = OCRCorrection::JNITools::getObject< OCRCorrection::Token* >( env, jCallerObject );

	token->setCorrected( b );
}

JNIEXPORT jboolean JNICALL Java_jav_correctionBackend_document_Token_isSuspicious
(JNIEnv *env, jobject jCallerObject) {
	// get Token-object
	OCRCorrection::Token* token = OCRCorrection::JNITools::getObject< OCRCorrection::Token* >( env, jCallerObject );

	return token->isSuspicious();
}

JNIEXPORT jboolean JNICALL Java_jav_correctionBackend_document_Token_isNormal
(JNIEnv *env, jobject jCallerObject) {
	// get Token-object
	OCRCorrection::Token* token = OCRCorrection::JNITools::getObject< OCRCorrection::Token* >( env, jCallerObject );

	return token->isNormal();

}

JNIEXPORT jint JNICALL Java_jav_correctionBackend_document_Token_getIndexInDocument
(JNIEnv *env, jobject jCallerObject) {
	// get Token-object
	OCRCorrection::Token* token = OCRCorrection::JNITools::getObject< OCRCorrection::Token* >( env, jCallerObject );

	return token->getIndexInDocument();
}

JNIEXPORT jint JNICALL Java_jav_correctionBackend_document_Token_getPageIndex
(JNIEnv *env, jobject jCallerObj) {
  // get Token-object
  OCRCorrection::Token* token = OCRCorrection::JNITools::getObject< OCRCorrection::Token* >( env, jCallerObj );

  return token->getPageIndex();
}


/*
 * Class:     jav_correctionBackend_document_Token
 * Method:    native_handleSplit
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_jav_correctionBackend_document_Token_native_1handleSplit
  (JNIEnv *env, jobject jCallerObject, jstring str) {
    // get Token-object
    OCRCorrection::Token* token = OCRCorrection::JNITools::getObject< OCRCorrection::Token* >( env, jCallerObject );

    
    std::wstring wstr;
    OCRCorrection::JNITools::jstringToWcstring( env, wstr, str );
    return token->handleSplit( wstr );
    


}


/*
 * Class:     jav_correctionBackend_document_Token
 * Method:    native_mergeRight
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_jav_correctionBackend_document_Token_native_1mergeRight__
(JNIEnv *env, jobject jCallerObject) {
	// get Token-object
	OCRCorrection::Token* token = OCRCorrection::JNITools::getObject< OCRCorrection::Token* >( env, jCallerObject );

	return token->mergeRight();
}


/*
 * Class:     jav_correctionBackend_document_Token
 * Method:    native_mergeRight
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_jav_correctionBackend_document_Token_native_1mergeRight__I
(JNIEnv *env, jobject jCallerObject, jint n ) {
	// get Token-object
	OCRCorrection::Token* token = OCRCorrection::JNITools::getObject< OCRCorrection::Token* >( env, jCallerObject );

	return token->mergeRight( n );
}


/*
 * Class:     jav_correctionBackend_document_Token
 * Method:    native_mergeHyphenation
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_jav_correctionBackend_document_Token_native_1mergeHyphenation
(JNIEnv *env, jobject jCallerObject) {

	// get Token-object
	OCRCorrection::Token* token = OCRCorrection::JNITools::getObject< OCRCorrection::Token* >( env, jCallerObject );

	token->mergeHyphenation();
}

/*
 * Class:     jav_correctionBackend_document_Token
 * Method:    getCandItemPointer
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_jav_correctionBackend_document_Token_getCandItemPointer
(JNIEnv *env, jobject jCallerObject) {
	// get Token-object
	OCRCorrection::Token* token = OCRCorrection::JNITools::getObject< OCRCorrection::Token* >( env, jCallerObject );

	return (jlong) token->getCandItem();
}

/*
 * Class:     jav_correctionBackend_document_Token
 * Method:    getNrOfCandidates
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_jav_correctionBackend_document_Token_getNrOfCandidates
(JNIEnv *env, jobject jCallerObject) {
	// get Token-object
	OCRCorrection::Token* token = OCRCorrection::JNITools::getObject< OCRCorrection::Token* >( env, jCallerObject );

	return (jlong) token->getNrOfCandidates();
}


#endif

