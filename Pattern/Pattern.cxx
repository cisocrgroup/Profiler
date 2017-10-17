#ifndef CSL_PATTERN_CXX
#define CSL_PATTERN_CXX CSL_PATTERN_CXX

#include <unicode/uchar.h>
#include "Pattern.h"

namespace csl {

    wchar_t Pattern::leftRightDelimiter_ = L':';
    const std::wstring Pattern::UNKNOWN = L"?";

void Pattern::print(std::wostream& os) const
{
	// os << getLeft() << leftRightDelimiter_ << getRight();
	for (wchar_t c: getLeft()) {
                if (u_charType(c) == U_NON_SPACING_MARK) {
			os << L'◌';
		}
		os << c;
	}
	os << leftRightDelimiter_;
	for (wchar_t c: getRight()) {
                if (u_charType(c) == U_NON_SPACING_MARK) {
			os << L'◌';
		}
		os << c;
	}
}

    // all other methods are defined inline.
} // eon

#endif
