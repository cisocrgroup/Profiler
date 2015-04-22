#ifndef _Included_jav_correctionBackend_CorrectionSystem_cxx
#define _Included_jav_correctionBackend_CorrectionSystem_cxx

#include<exception>
#include<iostream>
#include "./JNITools.h"
#include "./jav_correctionBackend_CorrectionSystem.h"
#include "../CorrectionSystem/CorrectionSystem.h"
#include "../Exceptions.h"

JNIEXPORT void JNICALL Java_jav_correctionBackend_CorrectionSystem_initCorrectionSystem
(JNIEnv *env , jobject callerObj ) {
    std::wcerr<<"JNI::initCorrectionSystem"<<std::endl;
    jclass callerClass = env->GetObjectClass( callerObj );
    
    jfieldID pointerID = env->GetFieldID( callerClass, "pointer_", "J" );


    OCRCorrection::CorrectionSystem* cxx_pointer = new OCRCorrection::CorrectionSystem();

    env->SetLongField( callerObj, pointerID, (jlong)cxx_pointer );
}


JNIEXPORT jint JNICALL Java_jav_correctionBackend_CorrectionSystem_readConfiguration
(JNIEnv *env, jobject callerObj, jstring iniFile ) {
    // get CS-object
    jclass callerClass = env->GetObjectClass( callerObj );
    jfieldID csPointerID = env->GetFieldID( callerClass, "pointer_", "J" );
    OCRCorrection::CorrectionSystem* cs = (OCRCorrection::CorrectionSystem*)env->GetLongField( callerObj, csPointerID );
 
    // get mbstring from java string
    const char* mbsINIFile =  env->GetStringUTFChars( iniFile, 0 );

    try {
	cs->readConfiguration( mbsINIFile );
    }
    catch( std::exception& exc ) {
	std::wcerr << "JNI::CorrectionSystem::readConfiguration FAILED!" << std::endl;
	OCRCorrection::JNITools::throwJavaExceptionFromCppException( env, exc );
	return 0;
    }
    
    env->ReleaseStringUTFChars( iniFile, (char*)mbsINIFile );
    
    return 1;
}

/*
 * Class:     jav_correctionBackend_CorrectionSystem
 * Method:    native_loadDocument
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_jav_correctionBackend_CorrectionSystem_native_1loadDocument
(JNIEnv * env, jobject callerObj, jstring filename ) {

    // get CorrectionSystem-object
    jclass callerClass = env->GetObjectClass( callerObj );
    jfieldID candPointerID = env->GetFieldID( callerClass, "pointer_", "J" );
    OCRCorrection::CorrectionSystem* cs = (OCRCorrection::CorrectionSystem*)env->GetLongField( callerObj, candPointerID );
    
    // get mbstring from java string
    const csl::uchar* mbsFilename = (const csl::uchar*) env->GetStringUTFChars( filename, 0 );

    try {
	cs->loadDocument( (char*)mbsFilename );
	env->ReleaseStringUTFChars( filename, (char*)mbsFilename );
    }
    catch( std::exception const& exc ) {
	std::wcerr << "JNI::CorrectionSystem::loadDocument FAILED!" << std::endl;
	OCRCorrection::JNITools::throwJavaExceptionFromCppException( env, exc );
	return;
    }
}


JNIEXPORT void JNICALL Java_jav_correctionBackend_CorrectionSystem_native_1newDocument
(JNIEnv *env, jobject callerObj, jstring dirName, jstring imageDirName, jint ft) {
    std::wcerr << "JNI::CorrSys::newDocument WITH imageDir" << std::endl;


    if( ft != OCRCorrection::CorrectionSystem::ABBYY_XML_DIR ) {
	OCRCorrection::JNITools::throwJavaException( env, "c++::OCRCorrection::CorrectionSystem: For Fileytpes other than ABBYY_XML_DIR,  an additional string argument to specify the image directory is not necessary." );
    }

    // get CorrectionSystem-object
    jclass callerClass = env->GetObjectClass( callerObj );
    jfieldID candPointerID = env->GetFieldID( callerClass, "pointer_", "J" );
    OCRCorrection::CorrectionSystem* cs = (OCRCorrection::CorrectionSystem*)env->GetLongField( callerObj, candPointerID );

    // get mbstring from java string
    const csl::uchar* mbsDirName = (const csl::uchar*) env->GetStringUTFChars( dirName, 0 );
    const csl::uchar* mbsImageDirName = (const csl::uchar*) env->GetStringUTFChars( imageDirName, 0 );
    


    try {
	cs->newDocument( (char*)mbsDirName, (char*)mbsImageDirName, OCRCorrection::CorrectionSystem::ABBYY_XML_DIR );
	env->ReleaseStringUTFChars( dirName, (char*)mbsDirName );
	env->ReleaseStringUTFChars( imageDirName, (char*)mbsImageDirName );
    }
    catch( std::exception const& exc ) {
	std::wcerr << "JNI::CorrectionSystem::newDocument FAILED!" << std::endl;
	std::wstring wideWhat;
	csl::CSLLocale::string2wstring( exc.what(), wideWhat );
	std::wcout << "EXC: " << wideWhat << std::endl;
	OCRCorrection::JNITools::throwJavaExceptionFromCppException( env, exc );
    }

}



JNIEXPORT jlong JNICALL Java_jav_correctionBackend_CorrectionSystem_getDocumentPointer
( JNIEnv *env, jobject callerObj ) {
  //    printf( "enter Java_jav_correctionBackend_CorrectionSystem_getDocumentPointer\n" );
    // get CS-object
    jclass callerClass = env->GetObjectClass( callerObj );
    jfieldID csPointerID = env->GetFieldID( callerClass, "pointer_", "J" );
    OCRCorrection::CorrectionSystem* cs = (OCRCorrection::CorrectionSystem*)env->GetLongField( callerObj, csPointerID );

    return (unsigned long) cs->getDocumentPointer();
}




#endif
