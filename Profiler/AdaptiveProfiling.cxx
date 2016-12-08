#include "AdaptiveHistGtLex.h"
#include "Profiler.h"

using namespace OCRCorrection;

////////////////////////////////////////////////////////////////////////////////
void
Profiler::calculateCandidateSet(const Profiler_Token& t,
		csl::DictSearch::CandidateSet& candidates)
{
	candidates.reset();
	if (config_.adaptive_ and t.origin().has_metadata("correction"))
		calculateAdaptiveCandidateSet(t, candidates);
	else
		calculateNonAdaptiveCandidateSet(t, candidates);
}

////////////////////////////////////////////////////////////////////////////////
void
Profiler::calculateNonAdaptiveCandidateSet(const Profiler_Token& t,
		csl::DictSearch::CandidateSet& candidates)
{
	dictSearch_.query(t.getWOCR_lc(), &candidates);
	std::sort(candidates.begin(), candidates.end());
	// for (const auto& cand: candidates) {
	// 	std::wcerr << "(Profiler) NonAdaptive: " << cand << "\n";
	// }
}

////////////////////////////////////////////////////////////////////////////////
void
Profiler::calculateAdaptiveCandidateSet(const Profiler_Token& t,
		csl::DictSearch::CandidateSet& candidates)
{
	assert(config_.adaptive_);
	assert(t.origin().has_metadata("correction"));

	std::wcerr << "(Profiler) Calculating adaptive profile for token "
		   << t.origin().metadata()["correction"] << " ("
		   << t.getWOCR() << ")\n";
	dictSearch_.query(t.getWOCR_lc(), &candidates);
	candidates.discard_if([&t](const csl::Interpretation& i) {
		return i.getBaseWord() != t.origin().metadata()["correction"] and
			i.getWord() != t.origin().metadata()["correction"];
	});
	// for (const auto& cand: candidates) {
	// 	std::wcerr << "(Profiler) Adaptive: " << cand << "\n";
	// }
	if (not candidates.empty())
		std::sort(candidates.begin(), candidates.end());
	else
		createCandidatesFromGt(t, candidates);
}

////////////////////////////////////////////////////////////////////////////////
void
Profiler::createCandidatesFromGt(const Profiler_Token& t,
		csl::DictSearch::CandidateSet& candidates)
{
	assert(candidates.empty());
	std::vector<csl::Instruction> ocrinstrs;
	std::wcerr << "(Profiler) Calculating candidates from gt for token "
		   << t.origin().metadata()["correction"] << " ("
		   << t.getWOCR() << ")\n";
	getAdaptiveHistGtLex().add(t.origin().metadata()["correction"],
			t.getWOCR_lc(), candidates);
	for (const auto& cand: candidates) {
		std::wcerr << "(Profiler) AdaptiveGt: " << cand << "\n";
	}
}

////////////////////////////////////////////////////////////////////////////////
AdaptiveHistGtLex&
Profiler::getAdaptiveHistGtLex()
{
	if (not adaptive_hist_gt_lex_) {
		std::unique_ptr<AdaptiveHistGtLex> lex(
				new AdaptiveHistGtLex(
					dictSearch_.getMaxCascadeRank() + 1, 2));
		dictSearch_.addDictModule(*lex);
		adaptive_hist_gt_lex_ = lex.get();
		lex.release(); // lex is managed by dictSearch
	}
	return *adaptive_hist_gt_lex_;
}
	// std::wofstream os("/tmp/adaptive-profiler.log");
	// if (not os.good())
	// 	throw std::system_error(errno, std::system_category(),
	// 			"/tmp/adaptive-profiler.log");

	// auto oldcout = std::wcout.rdbuf(os.rdbuf());
	// auto oldcerr = std::wcerr.rdbuf(os.rdbuf());

	// createNonAdaptiveProfile(document_);

	// std::wcout.rdbuf(oldcout);
	// std::wcerr.rdbuf(oldcerr);
