#ifndef OCRC_UTILS_CXX
#define OCRC_UTILS_CXX OCRC_UTILS_CXX

#include <unistd.h>
#include "Utils.h"
#include <csl/CSLLocale/CSLLocale.h>

namespace OCRCorrection {

    std::string Utils::getOCRCBase() {
	std::string ret = __FILE__;
	std::wstring wide_ret;
	csl::CSLLocale::string2wstring( ret, wide_ret );

	size_t pos = ret.find( "cxx/Utils/Utils.cxx" );
	if( pos == std::string::npos ) {
	    throw OCRCException( "OCRC::utils::getOCRCBase: could not create OCRCBase " );
	}
	ret.resize( pos );
	return ret;
    }


    std::string Utils::normalizeFilePath( std::string const& str ) {
	std::string path( str );
	if( path.at( 0 ) == '~' ) {
	    path.replace( 0, 1, getenv( "HOME" ) );
	}
	else if( path.at( 0 ) == '/' ) {
	    // absolute file-path; do nothing
	}
	else {
	    // relative file-path; add current working directory as prefix
	    char* cwd_cstr = getcwd( 0, 0 );

	    std::string cwd( cwd_cstr );
	    cwd += '/';
	    path.replace( 0, 0, cwd );
	}

	return path;
    }




}

#endif
