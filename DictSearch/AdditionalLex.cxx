#include "AdditionalLex.h"
#include "Document/Document.h"
#include "Utils/IStr.h"
#include <memory>
#include <system_error>
#include <unordered_set>

using namespace csl;

////////////////////////////////////////////////////////////////////////////////
AdditionalLex::AdditionalLex(const std::string& path,
                             size_t rank,
                             size_t max_lev)
  : DictModule(rank)
  , name_(L"additional-lex")
  , lex_(read(path))
  , cache_()
  , lev_()
  , max_lev_(max_lev)
{}

////////////////////////////////////////////////////////////////////////////////
bool
AdditionalLex::query(const std::wstring& q, Receiver* receiver)
{
  // std::wcerr << "(AdditionalLex) query: " << q << "\n";
  const auto i = cache_.find(q);
  if (i != end(cache_)) {
    for (const auto& p : i->second) {
      add_candidate(p.first, p.second, *receiver);
    }
    return true;
  }
  // not cached
  bool res = false;
  for (const auto& cand : lex_) {
    const auto k = lev_(q, cand);
    if (k <= max_lev_) {
      res = true;
      add_candidate(cand, k, *receiver);
      cache_[q].emplace_back(cand, k);
    }
  }
  return res;
}

////////////////////////////////////////////////////////////////////////////////
void
AdditionalLex::add_candidate(const std::wstring& gt,
                             size_t lev,
                             Receiver& receiver)
{
  csl::Interpretation i;
  i.setWord(gt.data());
  i.setBaseWord(gt.data());
  i.setBaseWordScore(0);
  i.setLevDistance(lev);
  // std::wcerr << "(AdditionalLex) Interpretation: " << i << "\n";
  static_cast<csl::iInterpretationReceiver&>(receiver).receive(i);
}

////////////////////////////////////////////////////////////////////////////////
std::unordered_set<std::wstring>
AdditionalLex::read(const std::string& path)
{
  std::unordered_set<std::wstring> set;
  std::wifstream is(path);
  if (not is.good()) {
    throw std::invalid_argument("cannot open: " + path);
  }
  std::wstring word;
  while (is >> word) {
    std::transform(word.begin(), word.end(), word.begin(), ::towlower);
    // std::wcerr << "(AdditionalLex) additional lexicon entry: " << word <<
    // "\n";
    set.insert(word);
  }
  is.close();
  return set;
}
