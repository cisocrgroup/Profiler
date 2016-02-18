#ifndef OCRC_UTILS_H
#define OCRC_UTILS_H OCRC_UTILS_H

#include"../Global.h"
#include<string>
#include<map>
#include <sstream>
#include<vector>

#include <Exceptions.h>

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
                 * convert a utf8 encoded multibyte string to a wide string
                 */
                static std::wstring utf8(std::string const& str);

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
