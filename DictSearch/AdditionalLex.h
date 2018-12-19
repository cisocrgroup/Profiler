
#ifndef CSL_AdditionalLex_hpp__
#define CSL_AdditionalLex_hpp__

#include "AdaptiveLex.h"
#include "DictSearch.h"
#include <unordered_set>

namespace OCRCorrection {
class Document;
}

namespace csl {
class AdditionalLex : public csl::DictSearch::iDictModule
{
public:
  using DictModule = csl::DictSearch::iDictModule;
  using Receiver = csl::DictSearch::iResultReceiver;

  AdditionalLex(const std::string& path, size_t rank, size_t max_lev);
  virtual ~AdditionalLex() noexcept override = default;
  virtual const std::wstring& getName() const override { return name_; }
  virtual bool query(const std::wstring& q, Receiver* res) override;

private:
  static void add_candidate(const std::wstring& gt,
                            size_t lev,
                            Receiver& receiver);
  static std::unordered_set<std::wstring> read(const std::string& path);

  const std::wstring name_;
  std::unordered_set<std::wstring> lex_;
  std::unordered_map<std::wstring, std::vector<std::pair<std::wstring, size_t>>>
    cache_;
  Levenshtein lev_;
  const size_t max_lev_;
};
}

#endif // CSL_AdditionalLex_hpp__
