#include "Profile.h"
#include "Document/Document.h"
#include "Token/Token.h"

using namespace OCRCorrection;
static const Profile::Pair&
mustGet(const Profile::Map& map, const Token& token);

size_t
Profile::count(const Token& token) const
{
  return mustGet(this->map_, token).second;
}

const csl::DictSearch::CandidateSet&
Profile::candiates(const Token& token) const
{
  return mustGet(map_, token).first;
}

void
Profile::put(const Token& token, F cb)
{
  auto f = map_.find(token.getWOCR_lc());
  if (f == map_.end()) {
    Pair p;
    cb(token, p.first);
    f = map_.emplace_hint(f, token.getWOCR_lc(), std::move(p));
  }
  f->second.second++;
  // if (not token.isNormal()) {
  //   counter_[L"notNormal"]++;
  //   token.setSuspicious(token.getAbbyySpecifics().isSuspicious());
  // } else if (token.isShort()) {
  //   counter_[L]
  // }
}

const Profile::Pair&
mustGet(const Profile::Map& map, const Token& token)
{
  const auto f = map.find(token.getWOCR_lc());
  if (f == map.end()) {
    throw std::logic_error("invalid token lookup in profile");
  }
  return f->second;
}
