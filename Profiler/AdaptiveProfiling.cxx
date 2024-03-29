#include "DictSearch/AdaptiveLex.h"
#include "Profiler.h"

using namespace OCRCorrection;

////////////////////////////////////////////////////////////////////////////////
void Profiler::calculateCandidateSet(
    const Token &t, csl::DictSearch::CandidateSet &candidates) {
  // std::wcerr << "(Profiler) Calculating candidate set for token " <<
  // t.getWOCR()
  //            << "\n";
  candidates.reset();
  if (config_.adaptive_ and t.has_metadata("correction")) {
    calculateAdaptiveCandidateSet(t, candidates);
  } else {
    calculateNonAdaptiveCandidateSet(t, candidates);
  }
}

////////////////////////////////////////////////////////////////////////////////
void Profiler::calculateNonAdaptiveCandidateSet(
    const Token &t, csl::DictSearch::CandidateSet &candidates) {
  // std::wcerr << "(Profiler) Calculating non adaptive profile for token "
  //            << t.getWOCR() << "\n";
  dictSearch_.query(t.getWOCR_lc(), &candidates);
  std::sort(candidates.begin(), candidates.end());
  // for (const auto& cand: candidates) {
  // 	std::wcerr << "(Profiler) NonAdaptive candidate: " << cand << "\n";
  // }
}

////////////////////////////////////////////////////////////////////////////////
void Profiler::calculateAdaptiveCandidateSet(
    const Token &t, csl::DictSearch::CandidateSet &candidates) {
  assert(config_.adaptive_);
  assert(t.has_metadata("correction"));
  assert(t.has_metadata("correction-lc"));
  // std::wcerr << "(Profiler) Calculating adaptive profile for token "
  //            << t.getWOCR() << " (" << t.metadata()["correction"] << ")\n";
  dictSearch_.query(t.getWOCR_lc(), &candidates);
  // for (const auto& cand: candidates) {
  // 	std::wcerr << "(Profiler) Adaptive candidate (before): " << cand << "\n";
  // }
  const auto correctionlc = t.metadata()["correction-lc"];
  candidates.discard_if([&](const csl::Interpretation &i) {
    const bool ret = i.getBaseWord() != correctionlc or
      i.getWord() != correctionlc;
    // std::wcerr << t.getWOCR_lc() << " (" << correctionlc << ")" << std::endl;
    // std::wcerr << "base word: " << i.getBaseWord()
    // 	       << " (" << correctionlc << ") " << ret << std::endl;
    // std::wcerr << "word:      " << i.getWord()
    // 	       << " (" << correctionlc << ") " << ret << std::endl;
    return ret;
  });
  // for (const auto& cand: candidates) {
  // 	std::wcerr << "(Profiler) Adaptive candidate (after): " << cand << "\n";
  // }
  if (not candidates.empty())
    std::sort(candidates.begin(), candidates.end());
  else
    createCandidatesWithCorrection(t, candidates);
  assert(not candidates.empty());
}

////////////////////////////////////////////////////////////////////////////////
void Profiler::createCandidatesWithCorrection(
    const Token &t, csl::DictSearch::CandidateSet &candidates) {
  assert(candidates.empty());
  // std::wcerr << "(Profiler) Calculating candidates from correction for token
  // "
  // 	   << t.getWOCR() << " ("
  // 	   << t.origin().metadata()["correction"] << ")\n";
  csl::AdaptiveLex::add(t.metadata()["correction-lc"], t.getWOCR_lc(), candidates);
  // for (const auto& cand: candidates) {
  // 	std::wcerr << "(Profiler) AdaptiveGt candidate: " << cand << "\n";
  // }
}
