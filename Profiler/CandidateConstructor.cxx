#include "Profiler/CandidateConstructor.h"

using namespace OCRCorrection;

////////////////////////////////////////////////////////////////////////////////
CandidateConstructor::Value*
CandidateConstructor::getValue(Profiler& profiler, Token& token)
{
	static int n = 0;
	static int o = 0;

	auto f = values_.find(token.getWOCR_lc());
	if (f == values_.end()) {
		++n;
		std::wcerr << "NEW VALUE: " << token.getWOCR_lc()
			   << " (new: " << n << ", old: " << o << ")\n";
		f = values_.emplace_hint(f, token.getWOCR_lc(),
				calculateValue(profiler, token));
	} else {
		++o;
	}
	assert(f != values_.end());
	if (f->second->mustRecalculateInstructions) {
		f->second->instructions = calculateInstructionsMap(
				token, f->second->candidates);
	}
	return f->second.get();
}

////////////////////////////////////////////////////////////////////////////////
void
CandidateConstructor::clearInstructions() const
{
	for (const auto& v: values_) {
		assert(v.second);
		v.second->mustRecalculateInstructions = true;
	}
}

////////////////////////////////////////////////////////////////////////////////
std::unique_ptr<CandidateConstructor::Value>
CandidateConstructor::calculateValue(Profiler& profiler, Token& token)
{
	CandidateSet tmp;
	profiler.calculateCandidateSet(token, tmp);
	std::wcerr << "SIZE OF CANDIDATES: " << tmp.size() << "\n";
	return std::unique_ptr<Value>(
			new Value{tmp, calculateInstructionsMap(token, tmp), false}
	);
}

////////////////////////////////////////////////////////////////////////////////
CandidateConstructor::InstructionsMap
CandidateConstructor::calculateInstructionsMap(const Token& token,
		const CandidateSet& candidates)
{
	InstructionsMap instructions;
	int total = 0;
	for (auto c = candidates.begin(); c != candidates.end(); ++c) {
		// ignore short words
		if (c->getWord().length() < 4) {
			continue;
		}
		// throw away candidates containing a hyphen
		// Yes, there are such words in staticlex :-/
		if (c->getWord().find( '-') != std::wstring::npos) {
			continue;
		}

		// calculate instructions
		std::vector<csl::Instruction> ocrInstructions;
		auto is_unknown = c->getHistInstruction().isUnknown();
		assert(not is_unknown or (candidates.size() == 1));
		instructionComputer_.computeInstruction(c->getWord(),
				token.getWOCR_lc(), &ocrInstructions, is_unknown);

		// ocrInstructions is empty, if the ocr errors can be explained
		// by std. lev. distance but not
		// by the distance defined with the patternWeights object.
		// In that case, drop the candidates.
		total += int(ocrInstructions.size());
		if(ocrInstructions.empty()) {
			std::wcerr << "NO OCR INSTRUCTIONS\n";
			continue;
		}
		instructions.emplace(c, ocrInstructions);
	}
	std::wcerr << "INSTRUCTIONS: " << total << " / " << instructions.size() << "\n";
	return instructions;
}

