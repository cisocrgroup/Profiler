#ifndef OCRC_PROBABILITY_DISTRIBUTION_H
#define OCRC_PROBABILITY_DISTRIBUTION_H

namespace OCRCorrection {
	struct Traces {
		csl::Instruction hist, ocr;
	};

	struct ProbabilityDistribution {
		double combinedProbability() const {
			return ocrProb * langProb;
		}
		Traces traces;
		double langProb, ocrProb, norm;
	};

	static inline bool operator==(const Traces& a, const Traces& b) {
		return a.hist == b.hist && a.ocr == b.ocr;
	}
}

#endif // OCRC_PROBABILITY_DISTRIBUTION_H
