#include "Profile.hxx"
#include "Document/Document.h"
#include "GlobalProfile/GlobalProfile.h"
#include "LanguageModel.hxx"
#include "Pattern/ComputeInstruction.h"
#include "PatternCounter/PatternCounter.h"
#include "Token/Token.h"
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

static bool
skipCand(const csl::DictSearch::Interpretation& cand);
static bool
isInsOrDel(const csl::Pattern& pat);
static std::unique_ptr<csl::MinDic<float>>
makeMinDic(const std::map<std::wstring, double>& m);

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
    Tuple t;
    cb(token, std::get<1>(t));
    f = types_.emplace_hint(f, token.getWOCR_lc(), std::move(t));
  }
  std::get<0>(f->second)++;
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
Profile::iteration(const LanguageModel& lm)
{
  std::map<csl::Pattern, std::set<std::wstring>> ocrPatternsInWords;
  std::map<std::wstring, double> baseWordFrequency;
  PatternCounter histCounter, ocrCounter;

  csl::ComputeInstruction computer;
  computer.connectPatternProbabilities(
    lm.globalProfile().ocrPatternProbabilities_);
  lm.globalProfile().dictDistribution_.clear();
  lm.frequencyList().connectPatternProbabilities(
    &lm.globalProfile().histPatternProbabilities_);

  for (auto& t : types_) {
    double sum = 0;
    std::get<2>(t.second).clear();

    // first run over all candiates
    for (const auto& cand : std::get<1>(t.second)) {
      if (skipCand(cand)) {
        continue;
      }
      std::vector<csl::Instruction> ocrInstructions;
      const auto isUnknown = cand.getHistInstruction().isUnknown();
      // make shure that, if a token contains an unkown candidate,
      // the unkown candiate is the only candidate for this token.
      // a -> b = not a or b
      assert(not isUnknown or (std::get<2>(t.second).size() == 1));
      computer.computeInstruction(
        cand.getWord(), t.first, &ocrInstructions, isUnknown);
      // ocrInstructions are a list of different possible ocr instructions.
      for (const auto& ins : ocrInstructions) {
        if (ins.size() > cand.getLevDistance()) {
          continue;
        }
        WeightedCandidate c;
        c.cand = cand;
        c.traces.ocr = ins;
        c.traces.hist = cand.getHistInstruction();
        c.langProb = lm.languageProbability(cand);
        c.ocrProb = lm.ocrTraceProbability(ins);
        c.dictModule = &cand.getDictModule();
        c.word = cand.getWord();
        c.baseWord = cand.getBaseWord();
        using namespace std::rel_ops;
        if (std::get<2>(t.second).empty() ||
            c.traces != std::get<2>(t.second).back().traces) {
          sum += c.combinedProbability();
          std::get<2>(t.second).push_back(std::move(c));
        }
      }
    }
    // now run over the calculated distributions
    for (auto& c : std::get<2>(t.second)) {
      c.weight = c.combinedProbability() / sum;
      if (std::isnan(c.weight)) {
        if (c.traces.hist.isUnknown()) {
          c.weight = 1;
        } else if (iteration_ == 0) { // first iteration
          c.weight = lm.ocrPatternStartProbability();
        } else {
          c.weight = lm.ocrPatternSmoothingProbability();
        }
      }
      const double weight = c.weight * std::get<0>(t.second);
      for (const auto& pat : c.traces.ocr) {
        ocrCounter.registerPattern(pat, weight);
        // Check for the individual weight of the candidate.
        if (c.weight > 0.5) {
          ocrPatternsInWords[pat].insert(t.first);
        }
      }
      for (const auto& pat : c.traces.hist) {
        histCounter.registerPattern(pat, weight);
      }
      if (c.traces.hist.size() > 0) {
        counter_[TokenType::GUESSED_HIST_TRACE_NONEMPTY] += weight;
      }
      if (c.traces.ocr.size() > 0) {
        counter_[TokenType::GUESSED_OCR_TRACE_NONEMPTY] += weight;
      }
      ocrCounter.registerNGrams(c.word, weight);
      histCounter.registerNGrams(c.baseWord, weight);
      baseWordFrequency[c.baseWord] += weight;
      counter_[TokenType::WAS_PROFILED] += weight;
      lm.globalProfile().dictDistribution_[c.dictModule->getName()].frequency +=
        weight;
    }
    if (std::get<2>(t.second).empty()) {
      counter_[TokenType::UNKNOWN] += std::get<0>(t.second);
    }
  }
  updateGlobalHistPatterns(lm, histCounter);
  updateGlobalOCRPatterns(lm, ocrCounter);
  updateDictDistributions(lm);
  updateBaseWordFrequencies(lm, baseWordFrequency);
  iteration_++;
}

void
Profile::finish()
{
  using std::begin;
  using std::end;
  using std::get;
  for (auto& t : types_) {
    // discard candidate set
    get<1>(t.second).clear();
    // sort candidates by (vote) weight
    std::sort(begin(get<2>(t.second)),
              end(get<2>(t.second)),
              [](const WeightedCandidate& a, const WeightedCandidate& b) {
                // sort in descending order of vote weigths
                return b.weight < a.weight;
              });
    for (auto cand : get<2>(t.second)) {
      cand.cand.setVoteWeight(cand.weight);
      cand.cand.setDictModule(*cand.dictModule);
    }
  }
}

void
Profile::setCorrection(Token& token) const
{
  const auto f = types_.find(token.getWOCR_lc());
  if (f == types_.end()) {
    return;
  }
  for (const auto& cand : std::get<2>(f->second)) {
    token.addCandidate(cand.cand);
  }
}

void
Profile::updateGlobalHistPatterns(const LanguageModel& lm,
                                  const PatternCounter& counts) const
{
  lm.clearHistPatternProbabilities();
  for (const auto& count : counts) {
    if (skipCutoff(count.second, lm.histPatternCutoff())) {
      continue;
    }
    auto sp = count.first;
    sp.strip();
    lm.globalProfile().histPatternProbabilities_.setWeight(
      count.first,
      count.second / counts.getNGramCount(sp.getLeft()),
      count.second);
  }
}

void
Profile::updateGlobalOCRPatterns(const LanguageModel& lm,
                                 const PatternCounter& counts) const
{
  lm.clearOCRPatternProbabilities();
  for (const auto& count : counts) {
    if (isInsOrDel(count.first)) {
      continue;
    }
    if (skipCutoff(count.second, lm.ocrPatternCutoff())) {
      continue;
    }
    double denom = 0;
    if (count.first.getLeft().empty()) {
      denom = ocrCharacterCount_;
    } else {
      denom = counts.getNGramCount(count.first.getLeft());
    }
    if (denom == 0) {
      continue;
    }
    if ((count.second / denom) > 1e-25) {
      lm.globalProfile().ocrPatternProbabilities_.setWeight(
        count.first, count.second / denom, count.second);
    }
  }
}

void
Profile::updateDictDistributions(const LanguageModel& lm) const
{
  for (auto& d : lm.globalProfile().dictDistribution_) {
    d.second.proportion =
      d.second.frequency / counter_.at(TokenType::WAS_PROFILED);
  }
}

void
Profile::updateBaseWordFrequencies(const LanguageModel& lm,
                                   const std::map<std::wstring, double>& m)
{
  mindic_ = makeMinDic(m);
  lm.frequencyList().setNrOfTrainingTokens(counter_[TokenType::WAS_PROFILED]);
  lm.frequencyList().connectBaseWordFrequency(mindic_.get());
}

std::unique_ptr<csl::MinDic<float>>
makeMinDic(const std::map<std::wstring, double>& m)
{
  std::unique_ptr<csl::MinDic<float>> dic;
  dic.reset(new csl::MinDic<float>());
  dic->initConstruction();
  for (const auto& e : m) {
    dic->addToken(e.first.data(), e.second);
  }
  dic->finishConstruction();
  return std::move(dic);
}

bool
Profile::skipCutoff(double count, double cutoff) const
{
  return count <= (cutoff * (counter_.at(TokenType::WAS_PROFILED) / 1000));
}

bool
isInsOrDel(const csl::Pattern& pat)
{
  return pat.getLeft().size() + pat.getRight().size() == 1;
}

bool
skipCand(const csl::DictSearch::Interpretation& cand)
{
  return cand.getWord().length() < 4 ||
         cand.getWord().find('-') != std::wstring::npos;
}
