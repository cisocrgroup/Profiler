#ifndef OCRC_WEIGHTED_CANDIATE_HXX
#define OCRC_WEIGHTED_CANDIATE_HXX

namespace OCRCorrection {
struct Traces
{
  csl::Instruction hist, ocr;
};

struct WeightedCandidate
{
  double combinedProbability() const { return ocrProb * langProb; }
  Traces traces;
  std::wstring dictModule, word, baseWord;
  double langProb, ocrProb, weight;
};

static inline bool
operator==(const Traces& a, const Traces& b)
{
  return a.hist == b.hist && a.ocr == b.ocr;
}
}

#endif // OCRC_WEIGHTED_CANDIATE_HXX
