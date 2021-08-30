#ifndef OCRC_UTILS_H
#define OCRC_UTILS_H OCRC_UTILS_H

#include <algorithm>
#include <string>
#include <map>
#include <sstream>
#include <vector>

#include"../Global.h"
#include "../Exceptions.h"

namespace OCRCorrection {
        class Utils {
        public:
                /**
                 * @brief A convenience method for querying a std::map.
                 */
                template< typename KeyT, typename ValueT >
                inline static ValueT const&
                queryConstMap( std::map< KeyT, ValueT > const& theMap,
                               KeyT const& key, ValueT const& defaultValue ) {
                        typename std::map< KeyT, ValueT >::const_iterator it =
                                theMap.find( key );
                        return ( it != theMap.end() )? it->second : defaultValue;
                }

                static std::string getOCRCBase();

                /**
                 * @brief returns a normalized copy of the given filePath
                 *
                 * This includes a lot of string-copying.
                 */
                static std::string normalizeFilePath( std::string const& str );

                /**
                 * convert a wide string to utf8
                 */
                static std::string utf8(std::wstring const& wstr);

		/**
		 * convert wstring to lower case
		 */
		static std::wstring tolower(const std::wstring& str) {
			return tolower(str.data(), str.size());
		}
		static std::wstring tolower(const wchar_t* str) {
			return tolower(str, wcslen(str));
		}
		static std::wstring tolower(const wchar_t* str, size_t n) {
			std::wstring res(n, 0);
			std::transform(str, str + n, begin(res), towlower);
			return res;
		}

                /**
                 * convert a utf8 encoded multibyte string to a wide string
                 */
                static std::wstring utf8(std::string const& str) {
                        return utf8(str.data(), str.size());
                }
                static std::wstring utf8(const char *str) {
			if (not str)
				return std::wstring();
                        return utf8(str, strlen(str));
                }
                static std::wstring utf8(const char *str, size_t n);

                /**
                 * convert string to num
                 */
                template<class T>
                static T toNum(std::string const& str)
                {
                        T res;
                        std::stringstream strstr(str);
                        strstr >> res;
                        if (strstr.rdstate() != (std::wistringstream::eofbit)) {
                                throw std::runtime_error("Could not convert: " + str);
                        }
                        return res;
                }

                /**
                 * convert wstring to float
                 */
                template<class T>
                static T toNum(std::wstring const& str)
                {
                        T res;
                        std::wstringstream wstrstr(str);
                        wstrstr >> res;
                        if (wstrstr.rdstate() != (std::wistringstream::eofbit)) {
                                throw std::runtime_error("Could not convert: " + utf8(str));
                        }
                        return res;
                }
        };
}

#endif
