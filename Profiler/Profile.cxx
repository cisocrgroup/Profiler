#include "Profile.hxx"
#include "Document/Document.h"
#include "LanguageModel.hxx"
#include "Pattern/ComputeInstruction.h"
#include "PatternCounter/PatternCounter.h"
#include "Token/Token.h"
#include "WeightedCandidate.hxx"
#include <utility>

using namespace OCRCorrection;

const std::wstring Profile::TokenType::NOT_NORMAL = L"notNormal";
const std::wstring Profile::TokenType::SHORT = L"short";
const std::wstring Profile::TokenType::DONT_TOUCH = L"dont_touch";
const std::wstring Profile::TokenType::NORMAL = L"normalAndLongTokens";
const std::wstring Profile::TokenType::WAS_PROFILED = L"wasProfiled";
const std::wstring Profile::TokenType::UNKNOWN = L"unknown";
const std::wstring Profile::TokenType::GUESSED_HIST_TRACE_NONEMPTY =
  L"guessed_histTrace_nonempty";
const std::wstring Profile::TokenType::GUESSED_OCR_TRACE_NONEMPTY =
  L"guessed_ocrTrace_nonempty";

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
  ocrCharacterCount_ += token.getWOCR().size();
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
Profile::profile(bool first, const LanguageModel& lm)
{
  csl::ComputeInstruction computer;
  std::map<csl::Pattern, std::set<std::wstring>> ocrPatternsInWords;
  std::unordered_map<std::wstring, double> baseWordFrequency;
  PatternCounter histCounter, ocrCounter;

  for (const auto& t : types_) {
    double sum = 0;
    std::vector<WeightedCandidate> cs;

    // first run over all candiates
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
        WeightedCandidate c;
        c.traces.ocr = ins;
        c.traces.hist = cand.getHistInstruction();
        c.langProb = lm.languageProbability(cand);
        c.ocrProb = lm.ocrTraceProbability(ins);
        c.dictModule = cand.getDictModule().getName();
        c.word = cand.getWord();
        c.baseWord = cand.getBaseWord();
        using namespace std::rel_ops;
        if (cs.empty() || c.traces != cs.back().traces) {
          sum += c.combinedProbability();
          cs.push_back(std::move(c));
        }
      }
    }
    // now run over the calculated distributions
    for (auto& c : cs) {
      c.weight = c.combinedProbability() / sum;
      if (std::isnan(c.weight)) {
        if (c.traces.hist.isUnknown()) {
          c.weight = 1;
        } else if (first) {
          c.weight = lm.ocrPatternStartProbability();
        } else {
          c.weight = lm.ocrPatternSmoothingProbability();
        }
      }
      for (const auto& pat : c.traces.ocr) {
        ocrCounter.registerPattern(pat, c.weight);
        if (c.weight > 0.5) {
          ocrPatternsInWords[pat].insert(t.first);
        }
      }
      for (const auto& pat : c.traces.hist) {
        histCounter.registerPattern(pat, c.weight);
      }
      if (c.traces.hist.size() > 0) {
        counter_[TokenType::GUESSED_HIST_TRACE_NONEMPTY] +=
          (c.weight * t.second.first);
      }
      if (c.traces.ocr.size() > 0) {
        counter_[TokenType::GUESSED_OCR_TRACE_NONEMPTY] +=
          (c.weight * t.second.first);
      }
      ocrCounter.registerNGrams(c.word, c.weight);
      histCounter.registerNGrams(c.baseWord, c.weight);
      baseWordFrequency[c.baseWord] += (c.weight * t.second.first);
      counter_[TokenType::WAS_PROFILED] += (c.weight * t.second.first);
    }
    if (cs.empty()) {
      counter_[TokenType::UNKNOWN] += t.second.first;
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
