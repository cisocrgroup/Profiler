#ifndef OCRC_UTILS_H
#define OCRC_UTILS_H OCRC_UTILS_H

#include<csl/Global.h>
#include<string>
#include<map>
#include<vector>

#include <Exceptions.h>

namespace OCRCorrection {


    class Utils {
    public:
	/**
	 * @brief A convenience method for querying a std::map.
	 */
	template< typename KeyT, typename ValueT >
	inline static ValueT const& queryConstMap( std::map< KeyT, ValueT > const& theMap, KeyT const& key, ValueT const& defaultValue ) {
	    typename std::map< KeyT, ValueT >::const_iterator it = theMap.find( key );
	    return ( it != theMap.end() )? it->second : defaultValue;
	}

	static std::string getOCRCBase();


	/**
	 * @brief returns a normalized copy of the given filePath
	 *
	 * This includes a lot of string-copying.
	 */
	static std::string normalizeFilePath( std::string const& str );

    };



}

#endif
