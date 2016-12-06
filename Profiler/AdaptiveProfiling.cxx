#include "AdaptiveHistGtLex.h"
#include "Profiler.h"

using namespace OCRCorrection;

////////////////////////////////////////////////////////////////////////////////
void
Profiler::calculateCandidateSet(const Profiler_Token& t,
		csl::DictSearch::CandidateSet& candidates)
{
	candidates.reset();
	if (canApplyAdaptiveProfiling(t))
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
bool
Profiler::canApplyAdaptiveProfiling(const Profiler_Token& t) const
{
	return config_.adaptive_ and not t.getWCorr_lc().empty();
}

////////////////////////////////////////////////////////////////////////////////
void
Profiler::calculateAdaptiveCandidateSet(const Profiler_Token& t,
		csl::DictSearch::CandidateSet& candidates)
{
	assert(config_.adaptive_);
	assert(not t.getWCorr_lc().empty());
	std::wcerr << "(Profiler) Calculating adaptive profile for token "
		   << t.getWCorr_lc() << " (" << t.getWOCR_lc() << ")\n";
	dictSearch_.query(t.getWOCR_lc(), &candidates);
	candidates.discard_if([&t](const csl::Interpretation& i) {
		return i.getBaseWord() != t.getWCorr_lc() and
			i.getWord() != t.getWOCR_lc();
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
	// std::wcerr << "(Profiler) Calculating candidates from gt for token "
	// 	   << t.getWCorr_lc() << " (" << t.getWOCR_lc() << ")\n";
	getAdaptiveHistGtLex().add(t.getWCorr_lc(), t.getWOCR_lc(), candidates);
	// for (const auto& cand: candidates) {
	// 	std::wcerr << "(Profiler) AdaptiveGt: " << cand << "\n";
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
	// std::wofstream os("/tmp/adaptive-profiler.log");
	// if (not os.good())
	// 	throw std::system_error(errno, std::system_category(),
	// 			"/tmp/adaptive-profiler.log");

	// auto oldcout = std::wcout.rdbuf(os.rdbuf());
	// auto oldcerr = std::wcerr.rdbuf(os.rdbuf());

	// createNonAdaptiveProfile(document_);

	// std::wcout.rdbuf(oldcout);
	// std::wcerr.rdbuf(oldcerr);
