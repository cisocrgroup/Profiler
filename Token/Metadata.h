#ifndef OCRCorrection_Metadata_hpp__
#define OCRCorrection_Metadata_hpp__

#include <map>
#include "Utils/IStr.h"

namespace OCRCorrection {
	class Metadata {
	public:
		const std::wstring& operator[](const std::string& s) const {
			return map_.at(s);
		}
		std::wstring& operator[](const std::string& s) {
			return map_[s];
		}
		bool has(const std::string& s) const noexcept {
			return map_.count(s);
		}
		void erase(const std::string& s) {
			map_.erase(s);
		}

	private:
		std::map<std::string, std::wstring, IStrCmp> map_;
	};
}

#endif // OCRCorrection_Metadata_hpp__
