#ifndef OCRC_LANGUAGE_MODEL_HXX
#define OCRC_LANGUAGE_MODEL_HXX

namespace csl {
class Pattern;
class Interpretation;
class Instruction;
}

namespace OCRCorrection {
class GlobalProfile;
class FrequencyList;

class LanguageModel
{
public:
  LanguageModel() = default;
  double ocrTraceProbability(const csl::Instruction& ocrtrace) const;
  double languageProbability(const csl::Interpretation& cand) const;
  double ocrPatternProbability(const csl::Pattern& pat) const;
  double histPatternProbability(const csl::Pattern& pat) const;
  double ocrPatternStartProbability() const { return ocrPatternStartProb_; }
  double ocrPatternSmoothingProbability() const
  {
    return ocrPatternSmoothingProb_;
  }

private:
  GlobalProfile* gprof_;
  FrequencyList* freqList_;
  double ocrPatternStartProb_;
  double ocrPatternSmoothingProb_;
  double histPatternStartProb_;
};
}

#endif // OCRC_LANGUAGE_MODEL_HXX
