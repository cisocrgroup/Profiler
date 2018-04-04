#include "Cache.h"
#include "Document/Document.h"
#include "Token/Token.h"

using namespace OCRCorrection;
using pair = std::pair<csl::DictSearch::CandidateSet, size_t>;
using map = std::unordered_map<std::wstring, pair>;
static const pair&
mustGet(const map& map, const Token& token);

size_t
Cache::count(const Token& token) const
{
  return mustGet(this->map_, token).second;
}

const csl::DictSearch::CandidateSet&
Cache::candiates(const Token& token) const
{
  return mustGet(map_, token).first;
}

void
Cache::put(const Token& token, F cb)
{
  auto f = map_.find(token.getWOCR_lc());
  if (f == map_.end()) {
    pair p;
    cb(token, p.first);
    f = map_.emplace_hint(f, token.getWOCR_lc(), std::move(p));
  }
  f->second.second++;
}

const pair&
mustGet(const map& map, const Token& token)
{
  const auto f = map.find(token.getWOCR_lc());
  if (f == map.end()) {
    throw std::logic_error("invalid token lookup in cache");
  }
  return f->second;
}
