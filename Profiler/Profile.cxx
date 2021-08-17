#include "Profile.hxx"
#include "Document/Document.h"
#include "GlobalProfile/GlobalProfile.h"
#include "LanguageModel.hxx"
#include "Pattern/ComputeInstruction.h"
#include "PatternCounter/PatternCounter.h"
#include "Token/Token.h"
#include <utility>

using namespace OCRCorrection;

const std::wstring ProfileBuilder::TokenType::NOT_NORMAL = L"notNormal";
const std::wstring ProfileBuilder::TokenType::SHORT = L"short";
const std::wstring ProfileBuilder::TokenType::DONT_TOUCH = L"dont_touch";
const std::wstring ProfileBuilder::TokenType::NORMAL = L"normalAndLongTokens";
const std::wstring ProfileBuilder::TokenType::WAS_PROFILED = L"wasProfiled";
const std::wstring ProfileBuilder::TokenType::UNKNOWN = L"unknown";
const std::wstring ProfileBuilder::TokenType::GUESSED_HIST_TRACE_NONEMPTY =
    L"guessed_histTrace_nonempty";
const std::wstring ProfileBuilder::TokenType::GUESSED_OCR_TRACE_NONEMPTY =
    L"guessed_ocrTrace_nonempty";

static bool skipCand(const csl::DictSearch::Interpretation &cand);
static bool isInsOrDel(const csl::Pattern &pat);
static std::unique_ptr<csl::MinDic<float>>
makeMinDic(const std::map<std::wstring, double> &m);

////////////////////////////////////////////////////////////////////////////////
ProfileBuilder::ProfileBuilder(bool adaptive)
    : types_(), counter_(), mindic_(), ocrCharacterCount_(), iteration_(),
      adaptive_(adaptive) {}

void ProfileBuilder::put(const Token &token, F cb) {
  updateCounts(token);
  // skip short, notNormal etc...
  if (not token.isNormal()) {
    return;
  }
  auto f = types_.find(token.getWOCR_lc());
  // Create a new entry into the types map.
  if (f == types_.end()) {
    Tuple t;
    cb(token, t.origin);
    t.n = 0;
    t.has_correction = adaptive_ and token.has_metadata("correction");
    f = types_.emplace_hint(f, token.getWOCR_lc(), std::move(t));
  }
  // Update to correction if adaptive and the token has a correction
  // and if the existing entry has not yet the adaptive candidate set.
  if (token.has_metadata("correction") and adaptive_ and not f->second.has_correction) {
    f->second.origin.clear();
    cb(token, f->second.origin);
    f->second.has_correction = true;
  }
  // Update type counts.
  f->second.n++;
  ocrCharacterCount_ += token.getWOCR().size();
  // std::wcerr << "ocrCharacterCount: " << ocrCharacterCount_ << "\n";
  // std::wcerr << f->first << ": " << std::get<1>(f->second).size()
  //            << " suggestions\n";
}

void ProfileBuilder::updateCounts(const Token &token) {
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

void ProfileBuilder::iteration(const LanguageModel &lm) {
  // std::map<csl::Pattern, std::set<std::wstring>> ocrPatternsInWords;
  std::map<std::wstring, double> baseWordFrequency;
  PatternCounter histCounter, ocrCounter;
  lm.globalProfile().dictDistribution_.clear();
  counter_[TokenType::WAS_PROFILED] = 0;
  counter_[TokenType::UNKNOWN] = 0;
  counter_[TokenType::GUESSED_HIST_TRACE_NONEMPTY] = 0;
  counter_[TokenType::GUESSED_OCR_TRACE_NONEMPTY] = 0;
  int ntype = 0;
  for (auto &t : types_) {
    ntype++;
    // std::wcerr << t.second.n << " " << t.first << " (" << ntype << "/"
    // 	       << types_.size() << ") " << t.second.candidates.size()
    // 	       << " candidates (" << t.second.origin.size() << ")" << std::endl;
    // In the first iteration, all candidates are computed from
    // the origin of the tokens.  After the first iteration,
    // the candidates in origin are removed and therefore the
    // loop is skipped.
    for (const auto& cand: t.second.origin) {
      if (skipCand(cand)) {
        continue;
      }
      std::vector<csl::Instruction> ocrInstructions;
      const auto isUnknown = cand.getHistInstruction().isUnknown();
      // Make shure that if a token contains an unkown candidate,
      // the unkown candiate is the only candidate for this token:
      // a -> b = not a or b
      assert(not isUnknown or (t.second.candidates.size() == 1));
      lm.computer().computeInstruction(cand.getWord(), t.first, &ocrInstructions, isUnknown);
      // std::wcerr << "ocrInstructions.size(): "
      //            << ocrInstructions.size() << std::endl;
      if (ocrInstructions.empty()) {
        // std::wcerr << "SKIPPING EMPTY OCR INSTRUCTIONS: " << cand << "\n";
        continue;
      }
      // ocrInstructions are a list of different possible ocr instructions.
      for (const auto &ins : ocrInstructions) {
        if (ins.size() > cand.getLevDistance()) {
          continue;
        }

        // Create weighte candidate for the computed ocr instructions.
	WeightedCandidate c;
	c.cand = cand;
	c.traces.ocr = ins;
	c.traces.hist = cand.getHistInstruction();
	c.cand.setOCRTrace(ins);
	using namespace std::rel_ops;
	if (t.second.candidates.empty() or c.traces != t.second.candidates.back().traces) {
	  t.second.candidates.push_back(std::move(c));
	}
      }
    }

    // We are done after the first iteration; clear the original
    // candidate set and calculate the weights using the weighted
    // candidates.
    t.second.origin = csl::DictSearch::CandidateSet();
    // std::wcerr << t.second.n << " " << t.first << " (" << ntype << "/"
    // 	       << types_.size() << ") " << t.second.candidates.size()
    // 	       << " candidates (" << t.second.origin.size() << ")"
    //         << std::endl;

    // We iterate over all weighted candidades and update the probabilities.
    double sum = 0;
    size_t n = 0;
    for (auto& cand: t.second.candidates) {
        // Update the probabilities.
        cand.langProb = lm.languageProbability(cand.cand);
        cand.ocrProb = lm.ocrTraceProbability(cand.cand.getOCRTrace());
	sum += cand.combinedProbability();
	n++;
    }
    // Update global historical and ocr pattern probabilities.
    // std::wcerr << "N: " << n << "\n";
    // std::wcerr << "SUM: " << sum << "\n";
    const double norm = 1 / sum;
    for (auto &c: t.second.candidates) {
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
      const double weight = c.weight * t.second.n;
      for (const auto &pat : c.traces.ocr) {
        ocrCounter.registerPattern(pat, weight);
        // Check for the individual weight of the candidate.
        // if (c.weight > 0.5) {
        //   ocrPatternsInWords[pat].insert(t.first);
        // }
      }
      for (const auto &pat : c.traces.hist) {
        histCounter.registerPattern(pat, weight);
      }
      if (c.traces.hist.size() > 0) {
        counter_[TokenType::GUESSED_HIST_TRACE_NONEMPTY] += weight;
      }
      if (c.traces.ocr.size() > 0) {
        counter_[TokenType::GUESSED_OCR_TRACE_NONEMPTY] += weight;
      }
      const auto word = c.cand.getWord();
      const auto baseWord = c.cand.getBaseWord();
      ocrCounter.registerNGrams(word, weight);
      histCounter.registerNGrams(baseWord, weight);
      baseWordFrequency[baseWord] += weight;
      counter_[TokenType::WAS_PROFILED] += weight;
      lm.globalProfile().dictDistribution_[c.cand.getDictModule().getName()].frequency += weight;
    }
    if (t.second.candidates.empty()) {
      counter_[TokenType::UNKNOWN] += t.second.n;
    }
  }
  updateGlobalHistPatterns(lm, histCounter);
  updateGlobalOCRPatterns(lm, ocrCounter);
  updateDictDistributions(lm);
  updateBaseWordFrequencies(lm, baseWordFrequency);
  iteration_++;
  lm.frequencyList().connectPatternProbabilities(
      &lm.globalProfile().histPatternProbabilities_);
}

std::shared_ptr<Profile> ProfileBuilder::build() {
  using std::begin;
  using std::end;
  using std::get;
  for (auto &t : types_) {
    // Sort candidates by (vote) weight.
    std::sort(begin(t.second.candidates), end(t.second.candidates),
              [](const WeightedCandidate &a, const WeightedCandidate &b) {
                // Sort in descending order of vote weigths.
                return b.weight < a.weight;
              });
    // Remove non unique suggestions.
    std::set<std::wstring> seen;
    auto it = std::remove_if(begin(t.second.candidates), end(t.second.candidates),
                             [&](const WeightedCandidate &c) {
                               if (seen.count(c.cand.getWord())) {
                                 return true;
                               }
                               seen.insert(c.cand.getWord());
                               return false;
                             });
    t.second.candidates.erase(it, end(t.second.candidates));
    for (auto &cand : t.second.candidates) {
      cand.cand.setVoteWeight(cand.weight);
    }
  }
  auto profile = std::make_shared<Profile>();
  for (auto& t: types_) {
    profile->put(t.first, t.second.n, t.second.candidates);
    t.second = Tuple(); // Clear value from types map.
  }
  return profile;
}

void ProfileBuilder::setCorrection(Token &token) const {
  const auto f = types_.find(token.getWOCR_lc());
  if (f == types_.end()) {
    return;
  }
  for (const auto &cand : f->second.candidates) {
    token.addCandidate(cand.cand);
  }
}

void ProfileBuilder::updateGlobalHistPatterns(const LanguageModel &lm,
                                       const PatternCounter &counts) const {
  lm.clearHistPatternProbabilities();
  for (const auto &count : counts) {
    if (skipCutoff(count.second, lm.histPatternCutoff())) {
      continue;
    }
    auto sp = count.first;
    sp.strip();
    lm.globalProfile().histPatternProbabilities_.setWeight(
        count.first, count.second / counts.getNGramCount(sp.getLeft()),
        count.second);
  }
}

void ProfileBuilder::updateGlobalOCRPatterns(const LanguageModel &lm,
                                      const PatternCounter &counts) const {
  lm.clearOCRPatternProbabilities();
  for (const auto &count : counts) {
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

void ProfileBuilder::updateDictDistributions(const LanguageModel &lm) const {
  for (auto &d : lm.globalProfile().dictDistribution_) {
    d.second.proportion =
        d.second.frequency / counter_.at(TokenType::WAS_PROFILED);
  }
}

void ProfileBuilder::updateBaseWordFrequencies(
    const LanguageModel &lm, const std::map<std::wstring, double> &m) {
  mindic_ = makeMinDic(m);
  lm.frequencyList().connectBaseWordFrequency(mindic_.get());
  double nr = counter_.at(TokenType::WAS_PROFILED);
  // Pathological case nr < 1.0 but at least one token.
  // Maybe setNrOfTrainingTokens should take a double instead.
  if (nr < 1.0 and not types_.empty()) {
    nr = 1.0;
  }
  lm.frequencyList().setNrOfTrainingTokens(nr);
}

std::unique_ptr<csl::MinDic<float>>
makeMinDic(const std::map<std::wstring, double> &m) {
  std::unique_ptr<csl::MinDic<float>> dic;
  dic.reset(new csl::MinDic<float>());
  dic->initConstruction();
  for (const auto &e : m) {
    dic->addToken(e.first.data(), e.second);
  }
  dic->finishConstruction();
  // return std::move(dic);
  return dic; // <- this should be moved because of copy elision
}

bool ProfileBuilder::skipCutoff(double count, double cutoff) const {
  return count <= (cutoff * (counter_.at(TokenType::WAS_PROFILED) / 1000));
}

bool isInsOrDel(const csl::Pattern &pat) {
  return pat.getLeft().size() + pat.getRight().size() == 1;
}

bool skipCand(const csl::DictSearch::Interpretation &cand) {
  return cand.getWord().length() < 4 ||
         cand.getWord().find('-') != std::wstring::npos;
}

static const Profile::Val EMPTY_PROFILE_VAL = Profile::Val();

const Profile::Val& Profile::get(const Token& token) const {
  auto f = map.find(token.getWOCR_lc());
  if (f == map.end()) {
    return EMPTY_PROFILE_VAL;
  }
  return f->second;
}

void Profile::put(const std::wstring& typ, size_t n,
		  const std::vector<WeightedCandidate>& candidates) {
  map[typ] = Val();
  map[typ].n = n;
  for (auto& cand: candidates) {
    map[typ].candidates.push_back(std::move(cand.cand));
  }
}
