#ifndef OCRC_PROFILE_HXX
#define OCRC_PROFILE_HXX

#include <functional>
#include <unordered_map>
#include <memory>

#include "../DictSearch/DictSearch.h"
#include "./WeightedCandidate.hxx"

namespace csl {
template <class T> class MinDic;
}

namespace OCRCorrection {
class Token;
class Profile;
class LanguageModel;
class PatternCounter;
class Profile;

class ProfileBuilder {
public:
  struct TokenType {
    static const std::wstring NOT_NORMAL, DONT_TOUCH, SHORT, NORMAL,
        WAS_PROFILED, UNKNOWN, GUESSED_HIST_TRACE_NONEMPTY,
        GUESSED_OCR_TRACE_NONEMPTY;
  };
  using F = std::function<void(const Token &, csl::DictSearch::CandidateSet &)>;
  struct Tuple {
    csl::DictSearch::CandidateSet origin;
    std::vector<WeightedCandidate> candidates;
    size_t n;
    bool has_correction;
  };
  using Map = std::unordered_map<std::wstring, Tuple>;

  ProfileBuilder(bool adatpive);
  void put(const Token &token, F f);
  void iteration(const LanguageModel &lm);
  std::shared_ptr<Profile> build();
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
  const bool adaptive_;
};

class Profile {
public:
  struct Val {
    std::vector<Candidate> candidates;
    size_t n;
  };

  typedef std::unordered_map<std::wstring, Val>::iterator iterator;
  typedef std::unordered_map<std::wstring, Val>::const_iterator const_iterator;
  iterator begin() {return map.begin();}
  iterator end() {return map.end();}
  const_iterator begin() const {return map.begin();}
  const_iterator end() const {return map.end();}

  const Val& get(const Token& token) const;


private:
  void put(const std::wstring& typ, size_t n,
	   const std::vector<WeightedCandidate>& candidates);
  friend class ProfileBuilder;
  std::unordered_map<std::wstring, Val> map;
};

} // namespace OCRCorrection

#endif // OCRC_PROFILE_HXX
