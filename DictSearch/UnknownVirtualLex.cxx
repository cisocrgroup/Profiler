#include <limits>
#include "UnknownVirtualLex.h"

using namespace csl;

////////////////////////////////////////////////////////////////////////////////
const std::wstring UnknownVirtualLex::name_ = L"unknown_virtual";

////////////////////////////////////////////////////////////////////////////////
UnknownVirtualLex::UnknownVirtualLex()
	: DictSearch::iDictModule(std::numeric_limits<size_t>::max())
{
}

////////////////////////////////////////////////////////////////////////////////
bool
UnknownVirtualLex::doquery(const std::wstring& query, Receiver& res)
{
	// This lex is the last one with a maximal cascade rank.
	// If no other dictionary could find the query, this one marks the
	// token as an unkown entry.
	// This is most likely an error, but we could not generate any candidates
	// for this.
	csl::Interpretation i;
	i.setWord(query);
	i.setBaseWord(query);
	i.setBaseWordScore(0);
	i.setLevDistance(1); // is handled separately for ocr trace, but lev is at least 1
	i.getHistInstruction().emplace_back(
			csl::Pattern::UNKNOWN, csl::Pattern::UNKNOWN, 0);
	assert(i.getHistInstruction().isUnknown());
	static_cast<csl::iInterpretationReceiver&>(res).receive(i);
	return true;
}
