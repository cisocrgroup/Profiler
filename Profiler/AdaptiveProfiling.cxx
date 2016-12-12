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
	// std::wcerr << "(Profiler) Calculating non adaptive profile for token "
	// 	   << t.getWOCR() << "\n";
	dictSearch_.query(t.getWOCR_lc(), &candidates);
	std::sort(candidates.begin(), candidates.end());
	// for (const auto& cand: candidates) {
	// 	std::wcerr << "(Profiler) NonAdaptive candidate: " << cand << "\n";
	// }
}

////////////////////////////////////////////////////////////////////////////////
void
Profiler::calculateAdaptiveCandidateSet(const Profiler_Token& t,
		csl::DictSearch::CandidateSet& candidates)
{
	assert(config_.adaptive_);
	assert(t.origin().has_metadata("correction"));
	assert(t.origin().has_metadata("correction-lc"));

	// std::wcerr << "(Profiler) Calculating adaptive profile for token "
	// 	   << t.getWOCR() << " ("
	// 	   << t.origin().metadata()["correction"] << ")\n";
	dictSearch_.query(t.getWOCR_lc(), &candidates);
	// for (const auto& cand: candidates) {
	// 	std::wcerr << "(Profiler) Adaptive candidate (before): "
	// 		<< cand << "\n";
	// }
	candidates.discard_if([&t](const csl::Interpretation& i) {
		return i.getBaseWord() != t.origin().metadata()["correction-lc"] and
			i.getWord() != t.origin().metadata()["correction-lc"];
	});
	// for (const auto& cand: candidates) {
	// 	std::wcerr << "(Profiler) Adaptive candidate (after): "
	// 		<< cand << "\n";
	// }
	if (not candidates.empty())
		std::sort(candidates.begin(), candidates.end());
	else
		createCandidatesWithCorrection(t, candidates);
}

////////////////////////////////////////////////////////////////////////////////
void
Profiler::createCandidatesWithCorrection(const Profiler_Token& t,
		csl::DictSearch::CandidateSet& candidates)
{
	assert(candidates.empty());
	// std::wcerr << "(Profiler) Calculating candidates from correction for token "
	// 	   << t.getWOCR() << " ("
	// 	   << t.origin().metadata()["correction"] << ")\n";
	getAdaptiveHistGtLex().add(t.origin().metadata()["correction-lc"],
			t.getWOCR_lc(), candidates);
	// for (const auto& cand: candidates) {
	// 	std::wcerr << "(Profiler) AdaptiveGt candidate: " << cand << "\n";
	// }
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
