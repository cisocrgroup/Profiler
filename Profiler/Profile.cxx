#include "Profile.h"
#include "Document/Document.h"
#include "LanguageModel.h"
#include "Pattern/ComputeInstruction.h"
#include "ProbabilityDistribution.h"
#include "Token/Token.h"
#include <utility>

using namespace OCRCorrection;

const std::wstring Profile::TokenType::NOT_NORMAL = L"notNormal";
const std::wstring Profile::TokenType::SHORT = L"short";
const std::wstring Profile::TokenType::DONT_TOUCH = L"dont_touch";
const std::wstring Profile::TokenType::NORMAL = L"normalAndLongTokens";

static const Profile::Pair&
mustGet(const Profile::Map& map, const Token& token);
static bool
skipCand(const csl::DictSearch::Interpretation& cand);

size_t
Profile::count(const Token& token) const
{
  return mustGet(this->types_, token).first;
}

const csl::DictSearch::CandidateSet&
Profile::candiates(const Token& token) const
{
  return mustGet(types_, token).second;
}

void
Profile::put(const Token& token, F cb)
{
  updateCounts(token);
  // skip short, notNormal etc...
  if (not token.isNormal()) {
    return;
  }
  auto f = types_.find(token.getWOCR_lc());
  if (f == types_.end()) {
    Pair p;
    cb(token, p.second);
    f = types_.emplace_hint(f, token.getWOCR_lc(), std::move(p));
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

void
Profile::profile(size_t i, bool last, const LanguageModel& lm)
{
  csl::ComputeInstruction computer;
  for (const auto& t : types_) {
    double sum = 0;
    std::vector<ProbabilityDistribution> ds;

    for (const auto& cand : t.second.second) {
      if (skipCand(cand)) {
        continue;
      }
      std::vector<csl::Instruction> ocrInstructions;
      const auto isUnknown = cand.getHistInstruction().isUnknown();
      // make shure that, if a token contains an unkown candidate,
      // the unkown candiate is the only candidate for this token.
      // a -> b = not a or b
      assert(not isUnknown or (t.second.second.size() == 1));
      computer.computeInstruction(
        cand.getWord(), t.first, &ocrInstructions, isUnknown);
      // ocrInstructions are a list of different possible ocr instructions.
      for (const auto& ins : ocrInstructions) {
        if (ins.size() > cand.getLevDistance()) {
          continue;
        }
        ProbabilityDistribution d;
        d.traces.ocr = ins;
        d.traces.hist = cand.getHistInstruction();
        d.langProb = lm.languageProbability(cand);
        d.ocrProb = lm.ocrTraceProbability(ins);
        using namespace std::rel_ops;
        if (ds.empty() || d.traces != ds.back().traces) {
          sum += d.combinedProbability();
          ds.push_back(std::move(d));
        }
      }
    }
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

bool
skipCand(const csl::DictSearch::Interpretation& cand)
{
  return cand.getWord().length() < 4 ||
         cand.getWord().find('-') != std::wstring::npos;
}
