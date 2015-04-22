#ifndef _Included_CandExtractor_CXX
#define _Included_CandExtractor_CXX

#include "./JNITools.h"
#include "./jav_correctionBackend_candidateExtractor_CandidateExtractor.h"

#include <csl/Global.h>
#include <csl/MSMatch/MSMatch.h>
#include <csl/ResultSet/ResultSet.h>


JNIEXPORT void JNICALL Java_jav_correctionBackend_candidateExtractor_CandidateExtractor_initNativeCandExtractor
( JNIEnv* env, jobject callerObj, jint levDistance, jstring dicFile ) {
    jclass callerClass = env->GetObjectClass( callerObj );

    jfieldID msMatchPointerID = env->GetFieldID( callerClass, "msMatchPointer_", "J" );

    const char* dicFile_cstr = env->GetStringUTFChars( dicFile, 0 );
    csl::MSMatch<csl::FW_BW>* msMatch = new csl::MSMatch< csl::FW_BW >( levDistance, dicFile_cstr  );

    env->SetLongField( callerObj, msMatchPointerID, (unsigned long)msMatch );
    env->ReleaseStringUTFChars( dicFile, dicFile_cstr );
}

JNIEXPORT void JNICALL Java_jav_correctionBackend_candidateExtractor_CandidateExtractor_destroyNativeCandExtractor
( JNIEnv* env, jobject callerObj ) {
    jclass callerClass = env->GetObjectClass( callerObj );
    jfieldID msMatchPointerID = env->GetFieldID( callerClass, "msMatchPointer_", "J" );
    
    // delete the MSMatch-object
    delete( (csl::MSMatch<csl::FW_BW>*)env->GetLongField( callerObj, msMatchPointerID ) );
}


JNIEXPORT void JNICALL Java_jav_correctionBackend_candidateExtractor_CandidateExtractor_nativeQuery
( JNIEnv* env, jobject callerObj, jstring pattern, jint levDistance, jobject receiverObj ) {
    // get MSMatch-object
    jclass callerClass = env->GetObjectClass( callerObj );
    jfieldID msMatchPointerID = env->GetFieldID( callerClass, "msMatchPointer_", "J" );
    csl::MSMatch<csl::FW_BW>* msMatch = (csl::MSMatch<csl::FW_BW>*)env->GetLongField( callerObj, msMatchPointerID );

    // get the receiver-method. Here, type-safety is somewhat gone: whatever class has a suitable method receive(...) will do
    jclass receiverClass = env->GetObjectClass( receiverObj );
    jmethodID receiveID = env->GetMethodID( receiverClass, "receive", "(Ljava/lang/String;FF)V" );

    csl::ResultSet* resultSet = new csl::ResultSet();

    // get wcstring from java string
    wchar_t wcsPattern[csl::Global::lengthOfLongStr];
    const csl::uchar* utf8Pattern = (const csl::uchar*) env->GetStringUTFChars( pattern, 0 );
    mbstowcs( wcsPattern, (char*)utf8Pattern, csl::Global::lengthOfLongStr );
    env->ReleaseStringUTFChars( pattern, (char*)utf8Pattern );
    
    msMatch->query( wcsPattern, *resultSet );

    csl::uchar mbsResult[csl::Global::lengthOfLongStr];
    for( size_t i = 0; i < resultSet->getSize(); ++i ) {
	wcstombs( (char*)mbsResult, (*resultSet)[i].getStr(), csl::Global::lengthOfLongStr );
	jstring javaStr = env->NewStringUTF( (char*)mbsResult ); 
	env->CallVoidMethod( receiverObj, receiveID, javaStr, (jfloat)( (*resultSet)[i].getLevDistance() ), (jfloat)( (*resultSet)[i].getAnn() ) );
	env->ReleaseStringUTFChars( javaStr, 0 );
    }

    delete( resultSet );

}


JNIEXPORT jboolean JNICALL Java_jav_correctionBackend_candidateExtractor_CandidateExtractor_nativeLookup
( JNIEnv* env, jobject callerObj, jstring pattern ) {
    // get MSMatch-object
    jclass callerClass = env->GetObjectClass( callerObj );
    jfieldID msMatchPointerID = env->GetFieldID( callerClass, "msMatchPointer_", "J" );
    csl::MSMatch<csl::FW_BW>* msMatch = (csl::MSMatch<csl::FW_BW>*)env->GetLongField( callerObj, msMatchPointerID );

    // get wcstring from java string
    wchar_t wcsPattern[csl::Global::lengthOfLongStr];
    const csl::uchar* utf8Pattern = (const csl::uchar*) env->GetStringUTFChars( pattern, 0 );
    mbstowcs( wcsPattern, (char*)utf8Pattern, csl::Global::lengthOfLongStr );
    env->ReleaseStringUTFChars( pattern, (char*)utf8Pattern );

    const csl::MinDic<>& dic = msMatch->getFWDic();
    
    unsigned int dicpos = dic.walkStr( dic.getRoot(), wcsPattern );
    if( dicpos && dic.isFinal( dicpos ) ) {
	return 1;
    }
    else return 0;
}


// not used 
JNIEXPORT void JNICALL Java_CandExtractor_test
(JNIEnv * env, jobject caller, jobject caExObject ) {
    printf( "c++ says: test ...\n" );
    jclass caExClass = env->GetObjectClass( caExObject );
    jmethodID ceTest = env->GetMethodID( caExClass, "sayJuhuu", "()V" );
    
    env->CallVoidMethod( caExObject, ceTest );
}

#endif
