#ifndef OCRC_LANGUAGE_MODEL_HXX
#define OCRC_LANGUAGE_MODEL_HXX

#include "Profiler.h"

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
  LanguageModel(const Profiler::Configuration& config,
                FrequencyList* freqList,
                GlobalProfile* gprof);
  double ocrTraceProbability(const csl::Instruction& ocrtrace) const;
  double languageProbability(const csl::Interpretation& cand) const;
  double ocrPatternProbability(const csl::Pattern& pat) const;
  double histPatternProbability(const csl::Pattern& pat) const;
  void clearHistPatternProbabilities() const;
  void clearOCRPatternProbabilities() const;
  GlobalProfile& globalProfile() const { return *gprof_; }
  FrequencyList& frequencyList() const { return *freqList_; }
  double histPatternCutoff() const { return histPatternCutoff_; }
  double ocrPatternCutoff() const { return ocrPatternCutoff_; }
  double ocrPatternStartProbability() const { return ocrPatternStartProb_; }
  double ocrPatternSmoothingProbability() const
  {
    return ocrPatternSmoothingProb_;
  }

private:
  GlobalProfile* gprof_;
  FrequencyList* freqList_;
  const double ocrPatternStartProb_;
  const double ocrPatternSmoothingProb_;
  const double histPatternStartProb_;
  const double histPatternCutoff_;
  const double ocrPatternCutoff_;
  const bool resetOCRPatternProbabilities_;
  const bool resetHistPatternProbabilities_;
};
}

#endif // OCRC_LANGUAGE_MODEL_HXX
