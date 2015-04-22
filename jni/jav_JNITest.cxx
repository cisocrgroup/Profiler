#ifndef _Included_jav_JNITest_cxx
#define _Included_jav_JNITest_cxx


#include "./JNITools.h"

#include "./jav_JNITest.h"

#include<iostream>

JNIEXPORT void JNICALL Java_jav_JNITest_voidMethod
(JNIEnv * env, jobject obj ) {
    std::wcout<<"JNITest: voidMethod()"<<std::endl;

}


/*
 * Class:     jav_JNITest
 * Method:    getsAndReturnsInt
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_jav_JNITest_getsAndReturnsInt
(JNIEnv * env, jobject obj, jint i ) {
    return i;
}

/*
 * Class:     jav_JNITest
 * Method:    returnsString
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_jav_JNITest_returnsString
(JNIEnv * env, jobject obj ) {
    std::wstring wstr( L"TEST12345" );
    jstring jstr;

    OCRCorrection::JNITools::stringToJstring( env, jstr, wstr );

    return jstr;
}



/*
 * Class:     jav_JNITest
 * Method:    getsAndReturnsString
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_jav_JNITest_getsAndReturnsString
(JNIEnv * env, jobject obj, jstring jstr_in ) {
    std::wstring wstr;
    OCRCorrection::JNITools::jstringToWcstring( env, wstr, jstr_in );
    std::wcout << "getsAndReturnsString:; String arrived at c++ as " << wstr << std::endl;
    jstring jstr_out;
    OCRCorrection::JNITools::stringToJstring( env, jstr_out, wstr );
    
    return jstr_out;
}



#endif
