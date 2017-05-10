#ifndef OCRC_CANDIDATE_CONSTRUCTOR_H
#define OCRC_CANDIDATE_CONSTRUCTOR_H

#include <map>
#include <vector>
#include <memory>

#include "DictSearch/DictSearch.h"
#include "Pattern/PatternProbabilities.h"
#include "Profiler/Profiler.h"

namespace OCRCorrection {
	class CandidateConstructor {
	public:
		using Token = Profiler::Profiler_Token;
		using CandidateSet = csl::DictSearch::CandidateSet;
		using InstructionsMap = std::map<
			CandidateSet::const_iterator,
			std::vector<csl::Instruction>
				>;
		struct Value {
			const CandidateSet candidates;
			InstructionsMap instructions;
			bool mustRecalculateInstructions;
		};
		Value* getValue(Profiler& profiler, Token& token);
		void connectPatternProbabilities(const csl::PatternProbabilities& pp) {
			instructionComputer_.connectPatternProbabilities(pp);
		}
		void clearInstructions() const;

	private:
		std::unique_ptr<Value> calculateValue(Profiler& profiler, Token& token);
		InstructionsMap calculateInstructionsMap(const Token& token,
				const CandidateSet& candidates);

		csl::ComputeInstruction instructionComputer_;
		std::map<std::wstring, std::unique_ptr<Value>> values_;
	};
}

#endif // OCRC_CANDIDATE_CONSTRUCTOR_H
