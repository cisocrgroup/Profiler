#ifndef CSL_PATTERN_CXX
#define CSL_PATTERN_CXX CSL_PATTERN_CXX

#include "Pattern.h"
#include <unicode/uchar.h>

namespace csl {

wchar_t Pattern::leftRightDelimiter_ = L':';
const std::wstring Pattern::UNKNOWN = L"?";

Pattern::Pattern(const std::wstring& expr): left_(), right_() {
  const auto pos = expr.find(leftRightDelimiter_);
  if (pos == 0) {
    right_ = expr.substr(pos+1);
  } else if (pos == std::wstring::npos) {
    left_ = expr;
  } else {
    left_ = expr.substr(0, pos);
    right_ = expr.substr(pos+1);
  }
}

void
Pattern::print(std::wostream& os) const
{
  // os << getLeft() << leftRightDelimiter_ << getRight();
  for (wchar_t c : getLeft()) {
    if (u_charType(c) == U_NON_SPACING_MARK) {
      os << L'◌';
    }
    os << c;
  }
  os << leftRightDelimiter_;
  for (wchar_t c : getRight()) {
    if (u_charType(c) == U_NON_SPACING_MARK) {
      os << L'◌';
    }
    os << c;
  }
}

// all other methods are defined inline.
} // eon

#endif
