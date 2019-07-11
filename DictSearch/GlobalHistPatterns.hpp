#ifndef CSL_GLOBAL_HIST_PATTERNS_HPP__
#define CSL_GLOBAL_HIST_PATTERNS_HPP__

#include <string>
#include <vector>

namespace csl {
class GlobalHistPatterns {
public:
  static GlobalHistPatterns &instance();
  const std::vector<std::wstring> &getHistPatterns() const;
  void addHistPattern(const std::wstring &pattern);

private:
  GlobalHistPatterns() : patterns_() {}
  ~GlobalHistPatterns() = default;
  GlobalHistPatterns(const GlobalHistPatterns &) = default;
  GlobalHistPatterns &operator=(const GlobalHistPatterns &) = default;
  std::vector<std::wstring> patterns_;

  static GlobalHistPatterns INSTANCE;
};
} // namespace csl

#endif // CSL_GLOBAL_HIST_PATTERNS_HPP__
