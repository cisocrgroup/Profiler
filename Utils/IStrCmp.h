#ifndef OCRCorrection_IStrCmp_hpp__
#define OCRCorrection_IStrCmp_hpp__

#include <cstring>

namespace OCRCorrection {
	struct IStrCmp {
		bool operator()(const std::string& a,
				const std::string& b) const noexcept {
			return strcasecmp(a.data(), b.data()) < 0;
		}
	};
}

#endif // OCRCorrection_IStrCmp_hpp__
