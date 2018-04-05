#include "Profile.h"
#include "Document/Document.h"
#include "Pattern/ComputeInstruction.h"
#include "Token/Token.h"

using namespace OCRCorrection;

const std::wstring Profile::TokenType::NOT_NORMAL = L"notNormal";
const std::wstring Profile::TokenType::SHORT = L"short";
const std::wstring Profile::TokenType::DONT_TOUCH = L"dont_touch";
const std::wstring Profile::TokenType::NORMAL = L"normalAndLongTokens";

static const Profile::Pair&
mustGet(const Profile::Map& map, const Token& token);

size_t
Profile::count(const Token& token) const
{
  return mustGet(this->map_, token).first;
}

const csl::DictSearch::CandidateSet&
Profile::candiates(const Token& token) const
{
  return mustGet(map_, token).second;
}

void
Profile::put(const Token& token, F cb)
{
  updateCounts(token);
  // skip short, notNormal etc...
  if (not token.isNormal()) {
    return;
  }
  auto f = map_.find(token.getWOCR_lc());
  if (f == map_.end()) {
    Pair p;
    cb(token, p.second);
    f = map_.emplace_hint(f, token.getWOCR_lc(), std::move(p));
  }
  f->second.first++;
}

void
Profile::updateCounts(const Token& token)
{
  if (not token.isNormal()) {
    counter_[TokenType::NOT_NORMAL]++;
    //    token.setSuspicious(token.getAbbyySpecifics().isSuspicious());
  } else if (token.isShort()) {
    counter_[TokenType::SHORT]++;
  } else if (token.isDontTouch()) {
    counter_[TokenType::DONT_TOUCH]++;
  } else {
    counter_[TokenType::NORMAL]++;
  }
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
