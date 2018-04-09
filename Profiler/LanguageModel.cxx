#include "LanguageModel.hxx"
#include "FrequencyList.h"
#include "GlobalProfile/GlobalProfile.h"

using namespace OCRCorrection;

static double
clampProb(double w, double start);

double
LanguageModel::ocrTraceProbability(const csl::Instruction& ocrtrace) const
{
  // See Profiler.cxx Profiler::getCombinedProb
  double ocrProb = 1.0;
  for (const auto& pat : ocrtrace) {
    ocrProb *= ocrPatternProbability(pat);
    ocrProb = clampProb(ocrProb, ocrPatternStartProb_);
  }
  return ocrProb;
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

void
LanguageModel::clearHistPatternProbabilities() const
{
  if (resetHistPatternProbabilities_) {
    gprof_->histPatternProbabilities_.clear();
  }
}

void
LanguageModel::clearOCRPatternProbabilities() const
{
  if (resetOCRPatternProbabilities_) {
    gprof_->ocrPatternProbabilities_.clearExplicitWeights();
  }
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
