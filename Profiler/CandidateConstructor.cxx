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
		f = values_.emplace_hint(f, token.getWOCR_lc(),
				calculateValue(profiler, token));
		// std::wcerr << "NEW VALUE: " << token.getWOCR_lc()
		// 	   << " CANDIATES: " << f->second->candidates.size()
		// 	   << " (new: " << n << ", old: " << o << ")\n";
		// for (const auto& c: f->second->candidates) {
		// 	std::wcerr << f->second->ocrlc << ":"
		// 		   << c.first;
		// 	for (const auto& i: c.second) {
		// 		std::wcerr << ":" << i;
		// 	}
		// 	std::wcerr << "\n";
		// }
	} else {
		++o;
	}
	assert(f != values_.end());
	assert(f->second.get());
	return f->second.get();
}

////////////////////////////////////////////////////////////////////////////////
void
CandidateConstructor::recalculateInstructions()
{
	for (const auto& v: values_) {
		assert(v.second);
		v.second->recalculateInstructions(computer_);
	}
}

////////////////////////////////////////////////////////////////////////////////
std::unique_ptr<CandidateConstructor::Value>
CandidateConstructor::calculateValue(Profiler& profiler, Token& token)
{
	return std::unique_ptr<Value>(new Value(computer_, profiler, token));
}

////////////////////////////////////////////////////////////////////////////////
CandidateConstructor::Value::Value(Computer& computer, Profiler& profiler, const Token& token)
	: ocrlc(token.getWOCR_lc())
	, candidates()
{
	CandidateSet tmp;
	profiler.calculateCandidateSet(token, tmp);
	for (const auto& candidate: tmp) {
		Instructions instructions;
		calculateInstructions(computer, candidate, instructions);
		if (not instructions.empty())
			candidates.emplace_back(candidate, std::move(instructions));
	}
}

////////////////////////////////////////////////////////////////////////////////
void
CandidateConstructor::Value::calculateInstructions(
		Computer& computer, const Interpretation& cand,
		Instructions& instructions) const
{
	instructions.clear();
	assert(instructions.empty());
	// ignore short words
	if (cand.getWord().length() < 4)
		return;
	// throw away candidates containing a hyphen
	// Yes, there are such words in staticlex :-/
	if (cand.getWord().find('-') != std::wstring::npos)
		return;

	auto is_unknown = cand.getHistInstruction().isUnknown();
	computer.computeInstruction(cand.getWord(), ocrlc, &instructions, is_unknown);
	auto e = std::remove_if(begin(instructions), end(instructions),
		[&](const csl::Instruction& i) {
			return i.size() > cand.getLevDistance();
		}
	);
	instructions.erase(e);
}


////////////////////////////////////////////////////////////////////////////////
void
CandidateConstructor::Value::recalculateInstructions(Computer& computer)
{
	for (auto& c: candidates) {
		calculateInstructions(computer, c.first, c.second);
	}
}
