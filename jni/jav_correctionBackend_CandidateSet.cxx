#ifndef _Included_jav_correctionBackend_CandidateSet_cxx
#define _Included_jav_correctionBackend_CandidateSet_cxx


#include "./JNITools.h"
#include "./jav_correctionBackend_CandidateSet.h"
#include<csl/DictSearch/DictSearch.h>


/*
 * Class:     jav_correctionBackend_CandidateSet
 * Method:    native_constructor
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_jav_correctionBackend_CandidateSet_native_1constructor
(JNIEnv * env, jobject callerObj ) {
    jclass callerClass = env->GetObjectClass( callerObj );
    jfieldID pointerID = env->GetFieldID( callerClass, "pointer_", "J" );

    csl::DictSearch::CandidateSet* cxx_pointer = new csl::DictSearch::CandidateSet();
    env->SetLongField( callerObj, pointerID, (jlong)cxx_pointer );

    return (jlong) cxx_pointer;
}


JNIEXPORT void JNICALL Java_jav_correctionBackend_CandidateSet_native_1finalize
(JNIEnv *env , jobject callerObj ) {
    jclass callerClass = env->GetObjectClass( callerObj );
    
    jfieldID pointerID = env->GetFieldID( callerClass, "pointer_", "J" );
    csl::DictSearch::CandidateSet* cs = (csl::DictSearch::CandidateSet*)env->GetLongField( callerObj, pointerID );
    delete( cs );
    
}

JNIEXPORT jint JNICALL Java_jav_correctionBackend_CandidateSet_getNrOfCandidates
(JNIEnv *env , jobject callerObj ) {
    // get c++-object
    jclass callerClass = env->GetObjectClass( callerObj );
    jfieldID candPointerID = env->GetFieldID( callerClass, "pointer_", "J" );
    csl::DictSearch::CandidateSet const* cs = (csl::DictSearch::CandidateSet const*)env->GetLongField( callerObj, candPointerID );

    return cs->size();
}

JNIEXPORT jlong JNICALL Java_jav_correctionBackend_CandidateSet_getCandidatePointer
(JNIEnv *env , jobject callerObj, jint index ) {
    // get c++-object
    jclass callerClass = env->GetObjectClass( callerObj );
    jfieldID candPointerID = env->GetFieldID( callerClass, "pointer_", "J" );
    csl::DictSearch::CandidateSet const* cs = (csl::DictSearch::CandidateSet const*)env->GetLongField( callerObj, candPointerID );

    return (jlong) &( cs->at( index ) );
}


#endif
