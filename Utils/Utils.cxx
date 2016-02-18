#ifndef OCRC_UTILS_CXX
#define OCRC_UTILS_CXX OCRC_UTILS_CXX

#include <unistd.h>
#include <cstdlib>
#include "Utils.h"

namespace OCRCorrection {

    std::string Utils::getOCRCBase() {
	std::string ret = __FILE__;
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


        std::string Utils::utf8(std::wstring const& wstr)
        {
                std::string out;
                out.reserve(wstr.size());
                bool shifts = std::wctomb(NULL, 0);
                char buffer[MB_CUR_MAX];
                for (size_t i = 0; i < wstr.size(); ++i) {
                        int ret = std::wctomb(buffer, wstr[i]);
                        out.append(buffer, ret);
                }
                return out;
        }

        std::wstring Utils::utf8(std::string const& str)
        {
                const size_t n = str.size();
                std::wstring out(n + 1, 0);
                std::mbstowcs(&out[0], str.data(), n + 1);
                return out;
        }
}

#endif
