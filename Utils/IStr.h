#ifndef OCRCorrection_IStr_hpp__
#define OCRCorrection_IStr_hpp__

#include <cstring>
#include <string>

namespace OCRCorrection {
	struct IStrCmp {
		bool operator()(const std::string& a,
				const std::string& b) const noexcept {
			return strcasecmp(a.data(), b.data()) < 0;
		}
	};
	struct IStrEq {
		bool operator()(const std::string& a,
				const std::string& b) const noexcept {
			return strcasecmp(a.data(), b.data()) == 0;
		}
	};
}

#endif // OCRCorrection_IStr_hpp__
