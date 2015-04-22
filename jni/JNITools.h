#ifndef OCRCORRECTION_JNITOOLS
#define OCRCORRECTION_JNITOOLS OCRCORRECTION_JNITOOLS

#include <string>
#include <csl/Global.h>
#include <csl/CSLLocale/CSLLocale.h>

#ifdef __CYGWIN__
#define __int64 unsigned long long
#endif

#include<jni.h>


namespace OCRCorrection {
    namespace JNITools {

	template< class PointerType_t >
	inline PointerType_t getObject( JNIEnv*& env, jobject& callerObj ) {
	    //jclass callerClass = env->GetObjectClass( callerObj );
	    //jfieldID tokenPointerID = env->GetFieldID( env->GetObjectClass( callerObj ), "pointer_", "J" );
	    //return (PointerType_t)env->GetLongField( callerObj, tokenPointerID );
	    return (PointerType_t)env->GetLongField( callerObj, env->GetFieldID( env->GetObjectClass( callerObj ), "pointer_", "J" ) );
	}

	inline void stringToJstring( JNIEnv*& env, jstring& jstring, std::wstring const& wstring ) {
	    // std::wcout<<"c++::JNITools::stringToJstring"<<std::endl; // DEBUG
	    unsigned char* mbstring = new unsigned char[wstring.length() * 4];
	    csl::UTF8_Codecvt u8_converter;
	
	    mbstate_t state = {0};
	    const wchar_t* fromNext = 0;
	    char* toNext = 0;
	    int result = u8_converter.out( state, wstring.c_str(), wstring.c_str() + wstring.length(), fromNext, (char*)mbstring, (char*)mbstring + wstring.length()*4, toNext );
	    if( result != csl::UTF8_Codecvt::ok ) {
		throw csl::exceptions::badInput( "JNITools::stringToJstring: encountered encoding error." );
	    }
	    *toNext = 0;

// 	wcstombs( (char*)mbstring, wstring.c_str(), 300 );
	
	    jstring = env->NewStringUTF( (char*)mbstring );
	    delete[](mbstring);

	}

	/**
	 * @brief This method converts a std::string into a std::wstring, just to be able to use the 
	 *        respective converter function above
	 */
	inline void stringToJstring( JNIEnv*& env, jstring& jstring, const std::string& narrowString ) {
	    std::wstring wideString;
	    csl::CSLLocale::string2wstring( narrowString, wideString );
	    return stringToJstring( env, jstring, wideString );
	}


	// get wcstring from java string
	inline void jstringToWcstring( JNIEnv*& env, std::wstring& wstring, jstring& jstring  ) {
	    csl::UTF8_Codecvt u8_converter;

	    const unsigned char* utf8String = (const unsigned char*) env->GetStringUTFChars( jstring, 0 );
	    size_t utf8StringLength = strlen( (char*)utf8String );
	    wstring.resize( utf8StringLength );

	    mbstate_t state = {0};
	    const char* fromNext = 0;
	    wchar_t* toNext = 0;
	    int result = u8_converter.in( state, 
					  (char const*)utf8String, 
					  (char const*)utf8String + utf8StringLength, 
					  fromNext, 
					  &( wstring[0] ), 
					  &( wstring[0] ) + wstring.size(), 
					  toNext
		);
	    if( result != csl::UTF8_Codecvt::ok ) {
		throw csl::exceptions::badInput( "JNITools::stringToJstring: encountered encoding error." );
	    }
	    *toNext = 0;

//	mbstowcs( &( wstring[0]), (const char*)utf8String, wstring.size() );

	    env->ReleaseStringUTFChars( jstring, (char*)utf8String );
	}



	/**
	 * @brief throws a JAVA exception of the given type with the given message
	 *
	 * taken from the JNI programmer's guide, p.75
	 */
	inline void throwExceptionByName( JNIEnv *env, char const* name, char const* msg ) {
	    std::wcerr << "jni::JNITools::throwExceptionByName: start" << std::endl;
	    jclass cls = env->FindClass( name );
	    /* If cls is NULL, an exception has already been thrown */
	    if (cls != NULL) {
		env->ThrowNew( cls, msg );
	    }
	    else { // cls == NULL
		std::wcerr << "jni::JNITools::throwExceptionByName: Cannot identify exception class" << std::endl;
	    }
	    /* free the local ref */
	    env->DeleteLocalRef( cls );
	}
	

	inline void throwJavaException( JNIEnv *env, char const* msg  ) { 
	    throwExceptionByName( env, "jav/exceptions/OCRCException", msg );
	}
	
	inline void throwJavaExceptionFromCppException( JNIEnv *env, std::exception const& exc  ) { 
	    throwExceptionByName( env, "jav/exceptions/OCRCException", exc.what() );
	}


    } // ns JNITools
} // ns OCRC

#endif
