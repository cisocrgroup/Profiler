#ifndef OCRC_PROFILE_HXX
#define OCRC_PROFILE_HXX

#include "DictSearch/DictSearch.h"
#include "WeightedCandidate.hxx"
#include <functional>
#include <unordered_map>

namespace csl {
template <class T> class MinDic;
}

namespace OCRCorrection {
class Token;
class Profile;
class LanguageModel;
class PatternCounter;

class Profile {
public:
  struct TokenType {
    static const std::wstring NOT_NORMAL, DONT_TOUCH, SHORT, NORMAL,
        WAS_PROFILED, UNKNOWN, GUESSED_HIST_TRACE_NONEMPTY,
        GUESSED_OCR_TRACE_NONEMPTY;
  };
  using F = std::function<void(const Token &, csl::DictSearch::CandidateSet &)>;
  using Tuple = std::tuple<size_t, csl::DictSearch::CandidateSet,
                           std::vector<WeightedCandidate>>;
  using Map = std::unordered_map<std::wstring, Tuple>;

  Profile() = default;
  void put(const Token &token, F f);
  void iteration(const LanguageModel &lm);
  void finish();
  void setCorrection(Token &token) const;
  int iteration() const { return iteration_; }

private:
  void updateGlobalHistPatterns(const LanguageModel &lm,
                                const PatternCounter &counts) const;
  void updateGlobalOCRPatterns(const LanguageModel &lm,
                               const PatternCounter &counts) const;
  void updateDictDistributions(const LanguageModel &lm) const;
  void updateCounts(const Token &token);
  void updateBaseWordFrequencies(const LanguageModel &lm,
                                 const std::map<std::wstring, double> &m);
  bool skipCutoff(double count, double cutoff) const;

  Map types_;
  std::map<std::wstring, double> counter_;
  std::unique_ptr<csl::MinDic<float>> mindic_;
  size_t ocrCharacterCount_;
  int iteration_;
};
} // namespace OCRCorrection

#endif // OCRC_PROFILE_HXX
