#include "LanguageModel.h"
#include "FrequencyList.h"
#include "GlobalProfile/GlobalProfile.h"

using namespace OCRCorrection;

static double
clampProb(double w, double start);

double
LanguageModel::combinedProbability(const csl::Interpretation& cand,
                                   const csl::Instruction& ocrtrace) const
{
  // See Profiler.cxx Profiler::getCombinedProb
  const auto langProb = languageProbability(cand);
  double ocrProb = 1.0;
  for (const auto& pat : ocrtrace) {
    ocrProb *= ocrPatternProbability(pat);
    ocrProb = clampProb(ocrProb, ocrPatternStartProb_);
  }
  return clampProb(ocrProb * langProb, ocrPatternStartProb_);
}

double
LanguageModel::languageProbability(const csl::Interpretation& cand) const
{
  // See Profiler.cxx Profiler::getCombinedProb
  return clampProb(freqList_->getLanguageProbability(cand),
                   ocrPatternStartProb_);
}

double
LanguageModel::ocrPatternProbability(const csl::Pattern& pat) const
{
  return clampProb(gprof_->ocrPatternProbabilities_.getWeight(pat),
                   ocrPatternStartProb_);
}

double
LanguageModel::histPatternProbability(const csl::Pattern& pat) const
{
  return clampProb(gprof_->histPatternProbabilities_.getWeight(pat),
                   histPatternStartProb_);
}

double
clampProb(double w, double start)
{
  if (std::isnan(w)) {
    return start;
  }
  if (w <= 0) {
    return start;
  }
  if (w > 1) {
    return 1;
  }
  return w;
}
