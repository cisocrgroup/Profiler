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
		using Computer = csl::ComputeInstruction;
		class Value {
		private:
			using Interpretation = csl::DictSearch::Interpretation;
			using Instructions = std::vector<csl::Instruction>;

			Value(Computer& computer, Profiler& profiler, const Token& token);
			void calculateInstructions(
					Computer& computer, const Interpretation& i,
					Instructions& instructions) const;
			void recalculateInstructions(Computer& computer);
			friend class CandidateConstructor;

		public:
			const std::wstring ocrlc;
			std::vector<std::pair<Interpretation, Instructions>> candidates;
		};

		Value* getValue(Profiler& profiler, Token& token);
		void connectPatternProbabilities(const csl::PatternProbabilities& pp) {
			computer_.connectPatternProbabilities(pp);
		}
		void recalculateInstructions();

	private:
		std::unique_ptr<Value> calculateValue(Profiler& profiler, Token& token);

		Computer computer_;
		std::map<std::wstring, std::unique_ptr<Value>> values_;
	};
}

#endif // OCRC_CANDIDATE_CONSTRUCTOR_H
