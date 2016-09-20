#ifndef CSL_GLOBAL_H
#define CSL_GLOBAL_H CSL_GLOBAL_H

#ifndef WIN32
#include<stdint.h> // Here, uint_32 etc. are defined
#endif

#include<stdlib.h> // Defines e.g. the exit codes for programs
#include<stdio.h>
#include<cassert>
#include<cassert>
#include<cstring>
#include<string>
#include<exception>
#include<stdexcept>
#include<ios>
#include<locale>
#include<vector>

#define debug(s) (std::cout<<"DEBUG: "<<s<<std::endl)


#ifdef WIN32
#pragma warning(disable:4996) // wcsncpy deprecated
#pragma warning(disable:4251) // strange warning about std::vector

#define CSL_DECLSPEC __declspec(dllexport)
#else
#define CSL_DECLSPEC
#endif

#if defined(_MSC_VER)
typedef signed __int8 int8_t;
typedef signed __int16 int16_t;
typedef signed __int32 int32_t;
typedef signed __int64 int64_t;
typedef unsigned __int8 uint8_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;
#endif



/**
   This namespace covers a system of modules for string processing.
   @todo
   - rename typedefs to "fasel_t"
*/
namespace csl {
    typedef unsigned short char16;
    typedef wchar_t char32;

    typedef unsigned char uchar;

    typedef unsigned int uint_t;

    typedef int bits32;

    typedef unsigned long long ulong_t;
    typedef unsigned long long bits64;

    typedef unsigned int StateId_t;

    typedef enum {
	/// just 4 bytes per transition (currently not implemented)
	SLIM,

	/// 8 bytes per transition, multiple values can be stored inside states
	BASIC,

	/**
	 * 12 bytes per transition;
	 * perfect hashing (ph) provides offset for a single value,
	 * "SuperSonic(suso) Mode" provides for each state a string of available transition-labels
	 * designed to hold huge token dictionaries with some frequency (or other) score
	 */
	TOKDIC
    } CellType ;

    typedef enum {
	STANDARD,
	FW_BW
    } MSMatchMode;

    namespace Global {

#ifdef CSL_ROOTDIR
	static const std::string cslRootDir( CSL_ROOTDIR );
#endif

	static const size_t maxNrOfChars = 65536;

	// those lengths are the maximum size of bytes from the input stream
	// static const size_t lengthOfWord = 64;
	static const size_t lengthOfWord = 128;
	static const size_t lengthOfStr = 3000;
	static const size_t lengthOfLongStr = 5000;

	static const int LevDEA_maxDistance = 3;

	static const wchar_t keyValueDelimiter = '#';
	static const uchar valuesDelimiter = ',';

	static const wchar_t wordBeginMarker = L'^';
	static const wchar_t wordEndMarker = L'$';

	// For more results, the program should not break but do a (maybe not so efficient) realloc
	static const int LevMaxNrOfResults = 200;

	class Perm {
	public:
	    static const int tokenDelimiter = ',';
	    static const int permuteDelimiter = '%';
	    static const int noPermuteDelimiter = '$';
	    static const size_t maxNrOfTokens = 32;
	    static const size_t maxCandsPerToken = 200;
	    static const size_t maxNrOfResults = 100;
	};

	/**
	 * @brief Three modes are offered for case handling:
	 * @see setCaseMode
	 */
	enum CaseMode { asIs,        /**< don't change anything, take query as it comes*/
			toLower,     /**< change query to lower case and return all candidates lower case */
			restoreCase  /**< do search in lower case, but upcase first character of all cands if query was uppercase */
	};


	static const void reverse(const wchar_t* str, wchar_t* newStr) {
	    size_t len = wcslen( str );
	    size_t i = 0;
	    while(i < len) {
		newStr[len-1-i] = str[i];
		++i;
	    }
	    newStr[len] = 0;
	}

	/**
	 * write the reversed \c str to \c newStr
	 * @param[in] str some string
	 * @param[out] the resukt is written to this string
	 */
	static const void reverse( const std::wstring& str, std::wstring* newStr ) {
	    if( &str == newStr ) {
		// throw exceptions::cslException( "Global::reverse: source string may not be equal to target string" );
	    }
	    size_t len = str.length();
	    newStr->resize( len );
	    size_t i = 0;
	    while(i < len) {
			newStr->at( len-1-i ) = str.at( i );
			++i;
	    }
	}

	static const void printBits( bits64 n ) {
	    int i;
	    for ( i = 63;i >= 0;--i ) {
		if ( ( i % 10 ) == 0 ) printf( "%d", i / 10 );
		else if ( ( i % 10 ) == 5 ) printf( "|" );
		else printf( " " );
	    }
	    printf( "\n" );
	    for ( i = 63;i >= 0;--i ) {
		printf( "%u", ( unsigned int )( 1 & ( n >> i ) ) );
	    }
	    printf( "\n" );
	}

    }

    namespace exceptions {

	class cslException : public std::exception {
	public:
	    cslException() {
	    }

	    cslException( const std::string& msg ) : msg_( msg ) {
	    }

	    virtual ~cslException() throw() {}

	    virtual const char* what() const throw() {
		return msg_.c_str();
	    }

	private:
	    const std::string msg_;
	};

	class LogicalError : public cslException {
	public:
	    LogicalError() {}
	    LogicalError( const std::string msg ) : cslException( msg ) {}
	};

	class AlphabetError : public cslException {
	public:
	    AlphabetError() {}
	    AlphabetError( const std::string msg ) : cslException( msg ) {}
	};

	class badInput : public cslException {
	public:
	    badInput() {}
	    badInput( const std::string msg ) : cslException( msg ) {}
	};

	class badDictFile : public cslException {
	public:
	    badDictFile() {}
	    badDictFile( const std::string msg ) : cslException( msg ) {}
	};

	class badFileHandle : public cslException {
	public:
	    badFileHandle() {}
	    badFileHandle( const std::string msg ) : cslException( msg ) {}
	};

	class invalidLevDistance : public cslException {
	public:
	    invalidLevDistance() {}
	    invalidLevDistance( const std::string msg ) : cslException( msg ) {}
	};

	class bufferOverflow : public cslException {
	public:
	    bufferOverflow() {}
	    bufferOverflow( const std::string msg ) : cslException( msg ) {}
	};

	class outOfRange : public cslException {
	public:
	    outOfRange() {}
	    outOfRange( const std::string msg ) : cslException( msg ) {}
	};

    };



    /**
     * @deprecated Use the implementation in csl::CSLLocale instead!!
     */
    // inline void string2wstring( std::string const& str, std::wstring& wstr, std::locale const& loc ) {
    // 	std::codecvt< wchar_t, char, std::mbstate_t > const& ccvt = std::use_facet< std::codecvt< wchar_t, char, std::mbstate_t > >( loc );

    // 	wstr.resize( str.size() * ccvt.max_length() ); // this makes no sense?! str.size() would be enough
    // 	std::codecvt< wchar_t, char, std::mbstate_t >::state_type state;
    // 	char const* fromNext = 0;
    // 	wchar_t* toNext = 0;
    // 	ccvt.in( state,
    // 		 str.c_str(),
    // 		 str.c_str()+str.size(),
    // 		 fromNext,
    // 		 (wchar_t*)wstr.
    // 		 c_str(),
    // 		 (wchar_t*)wstr.c_str() + wstr.size(),
    // 		 toNext
    // 	    );

    // 	wstr.resize( toNext - wstr.c_str() );
    // }

    // /**
    //  * @deprecated Use the implementation in csl::CSLLocale instead!!
    //  */
    // inline void string2wstring( std::string const& str, std::wstring& wstr ) {
    // 	std::locale loc( CSL_UTF8_LOCALE );
    // 	string2wstring( str, wstr, loc );
    // }


    /**
     * @brief Split a string using the given delimiter and collect the pieces in the given vector.
     *
     * @param[in]  str the input string
     * @param[in]  delimiter The delimiter symbol
     * @param[out] A pointer to the result vector
     */
    template< typename char_t >
    size_t split( std::basic_string< char_t > const& str, char_t delimiter, std::vector< std::basic_string< char_t > >* pieces ) {
	size_t pos = 0;
	size_t delimPos = str.npos;
	while( ( delimPos = str.find_first_of( delimiter, pos ) ) != str.npos ) {
	    pieces->push_back( str.substr( pos, delimPos - pos ) );
	    pos = delimPos + 1 ;
	}
	pieces->push_back( str.substr( pos ) );
	return pieces->size();
    }


} // eon


#endif
