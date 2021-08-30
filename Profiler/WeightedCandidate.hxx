#ifndef OCRC_WEIGHTED_CANDIATE_HXX
#define OCRC_WEIGHTED_CANDIATE_HXX

#include "../Candidate/Candidate.h"

namespace OCRCorrection {
struct Traces
{
  csl::Instruction hist, ocr;
};

struct WeightedCandidate
{
  double combinedProbability() const { return ocrProb * langProb; }
  Candidate cand;
  Traces traces;
  const csl::DictSearch::iDictModule* dictModule;
  std::wstring word, baseWord;
  double langProb, ocrProb, weight;
};

static inline bool
operator==(const Traces& a, const Traces& b)
{
  return a.hist == b.hist and a.ocr == b.ocr;
}

static inline std::wostream&
operator<<(std::wostream& os, const Traces& t)
{
  return os << "hist" << t.hist << ",ocr" << t.ocr;
}
}

#endif // OCRC_WEIGHTED_CANDIATE_HXX
