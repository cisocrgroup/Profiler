

#include "./Profiler.h"

namespace OCRCorrection {

Profiler::Evaluation_Token::Evaluation_Token(Profiler_Token& tok)
  : tok_(tok)
  , copyOfGroundtruth_(tok_.getGroundtruth())
{

  // set up allowed keys
  initCounter(L"shouldBeProfiled");
  initCounter(L"wasProfiled");
  initCounter(L"ocrCorrect");
  initCounter(L"ocrWrong");

  initCounter(L"ocrCorrect_abbyyNotSuspicious");
  initCounter(L"ocrCorrect_abbyySuspicious");
  initCounter(L"ocrCorrect_noSuggestion");
  initCounter(L"ocrCorrect_noSuggestion_abbyyNotSuspicious");
  initCounter(L"ocrCorrect_noSuggestion_abbyySuspicious");
  initCounter(L"ocrCorrect_noSuggestion_profilerSuspicious");
  initCounter(L"ocrCorrect_noSuggestion_profilerNotSuspicious");
  initCounter(L"ocrCorrect_profilerSuspicious");
  initCounter(L"ocrCorrect_profilerNotSuspicious");
  initCounter(L"ocrCorrect_suggestionCorrect");
  initCounter(L"ocrCorrect_suggestionWrong");
  initCounter(L"ocrWrong_abbyyNotSuspicious");
  initCounter(L"ocrWrong_abbyySuspicious");
  initCounter(L"ocrWrong_hasCorrectionSuggestion");
  initCounter(L"ocrWrong_suggestionCorrect");
  initCounter(L"ocrWrong_suggestionWrong");
  initCounter(L"ocrWrong_suggestionEquals");
  initCounter(L"ocrWrong_noSuggestion");
  initCounter(L"ocrWrong_noSuggestion_abbyyNotSuspicious");
  initCounter(L"ocrWrong_noSuggestion_abbyySuspicious");
  initCounter(L"ocrWrong_noSuggestion_profilerNotSuspicious");
  initCounter(L"ocrWrong_noSuggestion_profilerSuspicious");
  initCounter(L"ocrWrong_notDetected");
  initCounter(L"ocrWrong_profilerNotSuspicious");
  initCounter(L"ocrWrong_profilerSuspicious");
  initCounter(L"ocrWrong_suggestionCorrect");
  initCounter(L"ocrWrong_suggestionEquals");
  initCounter(L"ocrWrong_suggestionWrong");
  initCounter(L"skippedForEvaluation");

  initCounter(L"guessed_histTrace_nonempty");
  initCounter(L"guessed_ocrTrace_nonempty");
  initCounter(L"guessed_histTrace_good");
  initCounter(L"guessed_ocrTrace_good");
  initCounter(L"guessed_histTrace_nonempty_good");
  initCounter(L"guessed_ocrTrace_nonempty_good");
  initCounter(L"guessed_wOrig_good");
  initCounter(L"guessed_wOrig_nonempty_good");
  initCounter(L"guessed_baseWord_good");
  initCounter(L"guessed_baseWord_nonempty_good");
  initCounter(L"shouldDetectHistPattern");
  initCounter(L"shouldDetectOCRPattern");

  initCounter(L"papereval_allWords");
  initCounter(L"papereval_isCorrect");
  initCounter(L"papereval_isModern");
  initCounter(L"papereval_allWords");
  initCounter(L"papereval_guessed_wOrig_good");
  initCounter(L"papereval_guessed_baseWord_good");

  reset();

  // compute "hist" denominators for recall evaluation
  if (tok_.isShort()) {
    shouldBeProfiled_ = false;
    shouldDetectHistPattern_ = false;
  } else if (copyOfGroundtruth_.getOCRTrace() == L"*DESTROYED*" ||
             copyOfGroundtruth_.getOCRTrace() == L"*MERGE_SPLIT*") {
    shouldBeProfiled_ = false;
    shouldDetectHistPattern_ = false;
  } else if (copyOfGroundtruth_.getClassified() == L"modern") {
    shouldBeProfiled_ = true;
    shouldDetectHistPattern_ = false;
  } else if (copyOfGroundtruth_.getClassified() == L"hypothetic") {
    shouldBeProfiled_ = true;
    shouldDetectHistPattern_ = true;
  }

  else if (copyOfGroundtruth_.getClassified() == L"simplex_other_historic") {
    shouldBeProfiled_ = true;
    copyOfGroundtruth_.setHistTrace(L"[]");
    shouldDetectHistPattern_ = false;
  }

  else if (copyOfGroundtruth_.getClassified() == L"simplex_flect_historic") {
    shouldBeProfiled_ = true;
    copyOfGroundtruth_.setHistTrace(L"[]");
    shouldDetectHistPattern_ = false;
  } else if (copyOfGroundtruth_.getClassified() == L"simplex_lex_gap") {
    shouldBeProfiled_ = true;
    copyOfGroundtruth_.setHistTrace(L"[]");
    shouldDetectHistPattern_ = false;
  } else if (copyOfGroundtruth_.getClassified() == L"simplex_missing_pattern") {
    shouldBeProfiled_ = true;
    shouldDetectHistPattern_ = true;
    copyOfGroundtruth_.setHistTrace(L"*UNKNOWN*");
  } else if (copyOfGroundtruth_.getClassified() == L"simplex_lex_gap_pattern") {
    shouldBeProfiled_ = true;
    shouldDetectHistPattern_ = true;
    copyOfGroundtruth_.setHistTrace(L"*UNKNOWN*");
  } else if (copyOfGroundtruth_.getClassified() == L"compound_other_historic") {
    shouldBeProfiled_ = true;
    shouldDetectHistPattern_ = false;
    copyOfGroundtruth_.setHistTrace(L"[]");
  } else if (copyOfGroundtruth_.getClassified() == L"compound_flect_historic") {
    shouldBeProfiled_ = true;
    shouldDetectHistPattern_ = false;
    copyOfGroundtruth_.setHistTrace(L"[]");
  } else if (copyOfGroundtruth_.getClassified() == L"compound_lex_gap") {
    shouldBeProfiled_ = true;
    shouldDetectHistPattern_ = false;
    copyOfGroundtruth_.setHistTrace(L"[]");
  } else if (copyOfGroundtruth_.getClassified() ==
             L"compound_missing_pattern") {
    shouldBeProfiled_ = true;
    shouldDetectHistPattern_ = true;
    copyOfGroundtruth_.setHistTrace(L"*UNKNOWN*");
  } else if (copyOfGroundtruth_.getClassified() ==
             L"compound_lex_gap_pattern") {
    shouldBeProfiled_ = true;
    shouldDetectHistPattern_ = true;
    copyOfGroundtruth_.setHistTrace(L"*UNKNOWN*");
  } else if (copyOfGroundtruth_.getClassified() == L"geo_other_historic") {
    shouldBeProfiled_ = true;
    shouldDetectHistPattern_ = false;
    copyOfGroundtruth_.setHistTrace(L"[]");
  } else if (copyOfGroundtruth_.getClassified() == L"geo_flect_historic") {
    shouldBeProfiled_ = true;
    shouldDetectHistPattern_ = false;
    copyOfGroundtruth_.setHistTrace(L"[]");
  } else if (copyOfGroundtruth_.getClassified() == L"geo_lex_gap") {
    shouldBeProfiled_ = true;
    shouldDetectHistPattern_ = false;
    copyOfGroundtruth_.setHistTrace(L"[]");
  } else if (copyOfGroundtruth_.getClassified() == L"geo_missing_pattern") {
    shouldBeProfiled_ = true;
    shouldDetectHistPattern_ = true;
    copyOfGroundtruth_.setHistTrace(L"*UNKNOWN*");
  } else if (copyOfGroundtruth_.getClassified() == L"geo_lex_gap_pattern") {
    shouldBeProfiled_ = true;
    shouldDetectHistPattern_ = true;
    copyOfGroundtruth_.setHistTrace(L"*UNKNOWN*");
  } else if (copyOfGroundtruth_.getClassified() == L"name_lex_gap") {
    shouldBeProfiled_ = true;
    shouldDetectHistPattern_ = false;
    copyOfGroundtruth_.setHistTrace(L"[]");
  } else if (copyOfGroundtruth_.getClassified() == L"other_latin") {
    shouldBeProfiled_ = true;
    shouldDetectHistPattern_ = false;
    copyOfGroundtruth_.setHistTrace(L"[]");
  } else if (copyOfGroundtruth_.getClassified() == L"other_ocr_error") {
    shouldBeProfiled_ = false;
    shouldDetectHistPattern_ = false;
  } else if (copyOfGroundtruth_.getClassified() == L"other_merge_split") {
    shouldBeProfiled_ = false;
    shouldDetectHistPattern_ = false;
  } else if (copyOfGroundtruth_.getClassified() == L"split_tail") {
    shouldBeProfiled_ = false;
    shouldDetectHistPattern_ = false;
  } else if (copyOfGroundtruth_.getClassified() == L"other_abbrev") {
    shouldBeProfiled_ = false;
    shouldDetectHistPattern_ = false;
  } else if (copyOfGroundtruth_.getClassified() == L"other_trash") {
    shouldBeProfiled_ = false;
    shouldDetectHistPattern_ = false;
  } else if (copyOfGroundtruth_.getClassified() == L"other_dont_know") {
    shouldBeProfiled_ = false;
    shouldDetectHistPattern_ = false;
  } else if (copyOfGroundtruth_.getClassified() ==
             L"unknown") { // this means the token wasn't really clicked
    shouldBeProfiled_ = false;
    shouldDetectHistPattern_ = false;
  } else {
    std::wcerr << "Encountered groundtruth-classification without handling: "
               << copyOfGroundtruth_.getClassified() << std::endl;
  }

  //////// this is a new kind of evaluation "papereval", introduced Sept 2011
  //////////////////
  if (!tok_.isShort() && tok_.isNormal()) {
    getCounter(L"papereval_allWords") += 1;

    if (tok_.getWOCR_lc() == copyOfGroundtruth_.getWOrig_lc()) {
      getCounter(L"papereval_isCorrect") += 1;
    }
    if (tok_.getWOCR_lc() == copyOfGroundtruth_.getBaseWord()) {
      getCounter(L"papereval_isModern") += 1;
    } else {
      // std::wcout << tok_.getWOCR_lc() << "==" <<
      // copyOfGroundtruth_.getBaseWord() << std::endl;
    }
  }
  ////////////////////

  if ((!shouldBeProfiled_) || copyOfGroundtruth_.getOCRTrace() == L"[]") {
    shouldDetectOCRPattern_ = false;
  } else {
    shouldDetectOCRPattern_ = true;
  }

  if (shouldBeProfiled_) {
    getCounter(L"shouldBeProfiled") += 1;
  }

  if (shouldDetectHistPattern_) {
    getCounter(L"shouldDetectHistPattern") += 1;

    if ((copyOfGroundtruth_.getVerified() == Profiler_Token::VERIFIED_TRUE) &&
        (copyOfGroundtruth_.getHistTrace() != L"*UNKNOWN*")) {
      csl::Trace histTrace;
      histTrace.parseFromString(copyOfGroundtruth_.getHistTrace());
      for (csl::Trace::const_iterator pat = histTrace.begin();
           pat != histTrace.end();
           ++pat) {
        counterPerPattern_[*pat][L"shouldDetectHistPattern"] += 1;
      }
    }
  }

  if (shouldDetectOCRPattern_) {
    getCounter(L"shouldDetectOCRPattern") += 1;

    if (copyOfGroundtruth_.getVerified() == Profiler_Token::VERIFIED_TRUE) {
      csl::Trace ocrTrace;
      try {
        ocrTrace.parseFromString(copyOfGroundtruth_.getOCRTrace());
      } catch (std::exception& exc) {
        std::wcerr << "OCRC::Evaluation: Caught exception: " << exc.what()
                   << std::endl;
        std::wcerr << "Word was: " << tok.getWOCR_lc() << std::endl;
        std::wcerr << "ocr-instr was:" << copyOfGroundtruth_.getOCRTrace()
                   << std::endl;
        throw exc;
      }
      for (csl::Trace::const_iterator pat = ocrTrace.begin();
           pat != ocrTrace.end();
           ++pat) {
        counterPerPattern_[*pat][L"shouldDetectOCRPattern"] += 1;
      }
    }
  }
} // Evaluation_Token Constructor

void
Profiler::Evaluation_Token::reset()
{

  shouldBeProfiled_ = false;
  isProfiled_ = false;

  shouldDetectHistPattern_ = false;
  shouldDetectOCRPattern_ = false;
}

void
Profiler::Evaluation_Token::registerCandidate(
  Profiler_Interpretation const& cand)
{
  getCounter(L"wasProfiled") += cand.getVoteWeight();

  // denominator for precision for nonempty patterns
  if (!cand.getHistTrace().empty()) {
    getCounter(L"guessed_histTrace_nonempty") += cand.getVoteWeight();
    for (csl::Trace::const_iterator pat = cand.getHistTrace().begin();
         pat != cand.getHistTrace().end();
         ++pat) {
      counterPerPattern_[*pat][L"guessed_histPattern"] += cand.getVoteWeight();
    }
  }
  if (!cand.getOCRTrace().empty()) {
    getCounter(L"guessed_ocrTrace_nonempty") += cand.getVoteWeight();
    for (csl::Trace::const_iterator pat = cand.getOCRTrace().begin();
         pat != cand.getOCRTrace().end();
         ++pat) {
      counterPerPattern_[*pat][L"guessed_ocrPattern"] += cand.getVoteWeight();
    }
  }

  // numerator for recall & precision (including empty instructions)
  if (shouldBeProfiled_) {
    if (cand.getHistTrace().toString() == copyOfGroundtruth_.getHistTrace()) {
      getCounter(L"guessed_histTrace_good") += cand.getVoteWeight();
    }

    if ((cand.getOCRTrace().toString() == copyOfGroundtruth_.getOCRTrace())) {
      getCounter(L"guessed_ocrTrace_good") += cand.getVoteWeight();
    }
  }

  // numerator for recall & precision (without empty instructions)
  if (shouldDetectHistPattern_) {
    if (copyOfGroundtruth_.getHistTrace() == L"[]") {
      std::wcerr << __LINE__ << "CAN NOT HAPPEN" << std::endl;
      exit(1);
    }

    if (cand.getHistTrace().toString() == copyOfGroundtruth_.getHistTrace()) {
      getCounter(L"guessed_histTrace_nonempty_good") += cand.getVoteWeight();
    }

    // This is a slightly messy help to remember which patterns we detected only
    // with this candidate (we need that right below)
    std::map<csl::Pattern, double> didDetect;

    for (csl::Trace::const_iterator pat = cand.getHistTrace().begin();
         pat != cand.getHistTrace().end();
         ++pat) {
      didDetect[*pat] += cand.getVoteWeight();
    }
    for (std::map<csl::Pattern, double>::const_iterator pat = didDetect.begin();
         pat != didDetect.end();
         ++pat) {
      // give credit for the recognition of the pattern, but never for more than
      // it should have detected
      counterPerPattern_[pat->first][L"guessed_histPattern_good"] +=
        std::min(pat->second,
                 counterPerPattern_[pat->first][L"shouldDetectHistPattern"]);
    }
  }

  if (shouldDetectOCRPattern_) {
    if (copyOfGroundtruth_.getOCRTrace() == L"[]") {
      std::wcerr << __LINE__ << "CAN NOT HAPPEN" << std::endl;
      exit(1);
    }

    if (cand.getOCRTrace().toString() == copyOfGroundtruth_.getOCRTrace()) {
      getCounter(L"guessed_ocrTrace_nonempty_good") += cand.getVoteWeight();
    }

    // This is a slightly messy help to remember which patterns we detected only
    // with this candidate (we need that right below)
    std::map<csl::Pattern, double> didDetect;
    for (csl::Trace::const_iterator pat = cand.getOCRTrace().begin();
         pat != cand.getOCRTrace().end();
         ++pat) {
      didDetect[*pat] += cand.getVoteWeight();
    }
    for (std::map<csl::Pattern, double>::const_iterator pat = didDetect.begin();
         pat != didDetect.end();
         ++pat) {
      // give credit for the recognition of the pattern, but never for more than
      // it should have detected
      counterPerPattern_[pat->first][L"guessed_ocrPattern_good"] += std::min(
        pat->second, counterPerPattern_[pat->first][L"shouldDetectOCRPattern"]);
    }
  }

  // Did we guess the correct wOrig??
  if (copyOfGroundtruth_.getWOrig_lc() == cand.getWord()) {
    // std::wcerr << copyOfGroundtruth_.getWOrig_lc() << " == " <<
    // cand.getWord() << std::endl;
    getCounter(L"guessed_wOrig_good") += cand.getVoteWeight();
    if ((copyOfGroundtruth_.getHistTrace() != L"[]") ||
        (copyOfGroundtruth_.getOCRTrace() != L"[]")) {
      getCounter(L"guessed_wOrig_nonempty_good") += cand.getVoteWeight();
    }
  }

  // Did we guess the correct baseWord??
  if (copyOfGroundtruth_.getBaseWord() == cand.getBaseWord()) {
    getCounter(L"guessed_baseWord_good") += cand.getVoteWeight();
    if ((copyOfGroundtruth_.getHistTrace() != L"[]") ||
        (copyOfGroundtruth_.getOCRTrace() != L"[]")) {
      getCounter(L"guessed_baseWord_nonempty_good") += cand.getVoteWeight();
    }
  }

  ////// "papereval"
  if ((!tok_.isShort()) &&
      tok_.isNormal()) { // should be clear, otherwise there are no candidates
                         // (see 'else')

    if (copyOfGroundtruth_.getBaseWord() == cand.getBaseWord()) {
      getCounter(L"papereval_guessed_baseWord_good") += cand.getVoteWeight();
    }

    // scenario is: do correct only if the estimated ocr trace is probable
    // enough.

    if (cand.getCombinedProbability() > 1e-7) { // we would auto-correct
      if (copyOfGroundtruth_.getWOrig_lc() == cand.getWord()) {
        getCounter(L"papereval_guessed_wOrig_good") += cand.getVoteWeight();
      }
    } else { // we leave wOCR
      if (copyOfGroundtruth_.getWOrig_lc() == tok_.getWOCR_lc()) {
        getCounter(L"papereval_guessed_wOrig_good") += cand.getVoteWeight();
      }
    }
  } else {
    throw OCRCException(
      "Profiler::Evaluation: Received candidate for short or not-normal word.");
  }

} // registerCandidate

void
Profiler::Evaluation_Token::registerNoCandidates()
{
  if (!tok_.isShort() && tok_.isNormal()) {
    // no candidates, that means we leave the token the way it is
    if (tok_.getWOCR_lc() == copyOfGroundtruth_.getWOrig_lc()) {
      getCounter(L"papereval_guessed_wOrig_good") += 1;
    }
    if (tok_.getWOCR_lc() == copyOfGroundtruth_.getBaseWord()) {
      getCounter(L"papereval_guessed_baseWord_good") += 1;
    }
  }
}

///////////// THOSE FOUR ARE FOR Evaluation_Token !!!!!!

std::map<std::wstring, double> const&
Profiler::Evaluation_Token::getCounter() const
{
  return counter_;
}

double&
Profiler::Evaluation_Token::getCounter(std::wstring const& key)
{
  std::map<std::wstring, double>::iterator it = counter_.find(key);
  if (it == counter_.end())
    throw OCRCException(
      std::wstring(
        L"OCRC::Profiler::Evaluation_Token::getCounter Unknown key: ") +
      key);
  return it->second;
}

double
Profiler::Evaluation_Token::getCounter(std::wstring const& key) const
{
  std::map<std::wstring, double>::const_iterator it = counter_.find(key);
  if (it == counter_.end())
    throw OCRCException(
      std::wstring(
        L"OCRC::Profiler::Evaluation_Token::getCounter Unknown key: ") +
      key);
  return it->second;
}

void
Profiler::Evaluation_Token::initCounter(std::wstring const& key)
{
  counter_[key] = 0;
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// class Evaluation
/////////////////////////////////////

Profiler::Evaluation::Evaluation(Profiler const& myProfiler)
  : myProfiler_(myProfiler)
  , finished_(false)
{

  // set up counter keys
  initCounter(L"shouldBeProfiled");
  initCounter(L"wasProfiled");
  initCounter(L"ocrCorrect");
  initCounter(L"ocrWrong");

  initCounter(L"ocrCorrect_abbyyNotSuspicious");
  initCounter(L"ocrCorrect_abbyySuspicious");
  initCounter(L"ocrCorrect_noSuggestion");
  initCounter(L"ocrCorrect_noSuggestion_abbyyNotSuspicious");
  initCounter(L"ocrCorrect_noSuggestion_abbyySuspicious");
  initCounter(L"ocrCorrect_noSuggestion_profilerSuspicious");
  initCounter(L"ocrCorrect_noSuggestion_profilerNotSuspicious");
  initCounter(L"ocrCorrect_profilerSuspicious");
  initCounter(L"ocrCorrect_profilerNotSuspicious");
  initCounter(L"ocrCorrect_suggestionCorrect");
  initCounter(L"ocrCorrect_suggestionWrong");
  initCounter(L"ocrWrong_abbyyNotSuspicious");
  initCounter(L"ocrWrong_abbyySuspicious");
  initCounter(L"ocrWrong_hasCorrectionSuggestion");
  initCounter(L"ocrWrong_suggestionCorrect");
  initCounter(L"ocrWrong_suggestionWrong");
  initCounter(L"ocrWrong_suggestionEquals");
  initCounter(L"ocrWrong_noSuggestion");
  initCounter(L"ocrWrong_noSuggestion_abbyyNotSuspicious");
  initCounter(L"ocrWrong_noSuggestion_abbyySuspicious");
  initCounter(L"ocrWrong_noSuggestion_profilerNotSuspicious");
  initCounter(L"ocrWrong_noSuggestion_profilerSuspicious");
  initCounter(L"ocrWrong_notDetected");
  initCounter(L"ocrWrong_profilerNotSuspicious");
  initCounter(L"ocrWrong_profilerSuspicious");
  initCounter(L"ocrWrong_suggestionCorrect");
  initCounter(L"ocrWrong_suggestionEquals");
  initCounter(L"ocrWrong_suggestionWrong");
  initCounter(L"skippedForEvaluation");

  initCounter(L"guessed_histTrace_nonempty");
  initCounter(L"guessed_ocrTrace_nonempty");
  initCounter(L"guessed_histTrace_good");
  initCounter(L"guessed_ocrTrace_good");
  initCounter(L"guessed_histTrace_nonempty_good");
  initCounter(L"guessed_ocrTrace_nonempty_good");
  initCounter(L"guessed_wOrig_good");
  initCounter(L"guessed_wOrig_nonempty_good");
  initCounter(L"guessed_baseWord_good");
  initCounter(L"guessed_baseWord_nonempty_good");
  initCounter(L"shouldDetectHistPattern");
  initCounter(L"shouldDetectOCRPattern");

  initCounter(L"papereval_allWords");
  initCounter(L"papereval_isCorrect");
  initCounter(L"papereval_isModern");
  initCounter(L"papereval_allWords");
  initCounter(L"papereval_guessed_wOrig_good");
  initCounter(L"papereval_guessed_baseWord_good");
}

void
Profiler::Evaluation::registerToken(Evaluation_Token& evalTok)
{
  if (evalTok.tok_.isNormal() && !evalTok.tok_.isShort() &&
      (evalTok.copyOfGroundtruth_.getClassified() !=
       L"split_tail") && // those are usually not verified!
      (evalTok.copyOfGroundtruth_.getVerified() ==
       Profiler_Token::VERIFIED_FALSE)) {

    // std::wcerr << "OCRC::Profiler::Evaluation::registerToken::warning:
    // unverified token in evaluation" << std::endl
    // 	       << "Token:'" << evalTok.tok_.getWOCR_lc() << "' - '"
    // 	       << evalTok.copyOfGroundtruth_.getHistTrace() << "'"
    // 	       << ", classified as " <<
    // evalTok.copyOfGroundtruth_.getClassified()
    // 	       << std::endl;

    getCounter(L"skippedForEvaluation") += 1;
    return;
  }

  // compute groundtruth pattern probabilities for hist and ocr
  if (evalTok.shouldBeProfiled_) {

    csl::Trace histTrace;
    if (evalTok.copyOfGroundtruth_.getHistTrace() != L"*UNKNOWN*") {

      if (evalTok.copyOfGroundtruth_.getHistTrace() == L"") {
        std::wcerr << "OCRC::Profiler::Evaluation::registerToken::warning: "
                      "groundtruth histTrace can not be empty here."
                   << std::endl
                   << "Token:'" << evalTok.tok_.getWOCR_lc() << "' - '"
                   << evalTok.copyOfGroundtruth_.getHistTrace() << "'"
                   << std::endl;
      } else {
        histTrace.parseFromString(evalTok.copyOfGroundtruth_.getHistTrace());

        histPatternCounter_.registerNGrams(
          evalTok.copyOfGroundtruth_.getBaseWord(), 1);
        for (csl::Trace::const_iterator posPattern = histTrace.begin();
             posPattern != histTrace.end();
             ++posPattern) {
          histPatternCounter_.registerPattern(*posPattern, 1);
        }
      }
    }

    ocrPatternCounter_.registerNGrams(evalTok.copyOfGroundtruth_.getWOrig_lc(),
                                      1);
    csl::Trace ocrTrace;
    if (evalTok.copyOfGroundtruth_.getOCRTrace() == L"") {
      std::wcerr << "OCRC::Profiler::Evaluation::registerToken::warning: "
                    "groundtruth ocrTrace can not be empty here."
                 << std::endl
                 << "Token:'" << evalTok.tok_.getWOCR_lc() << "' - '"
                 << evalTok.copyOfGroundtruth_.getHistTrace() << "'"
                 << std::endl;
    } else {
      ocrTrace.parseFromString(evalTok.copyOfGroundtruth_.getOCRTrace());

      for (csl::Trace::const_iterator posPattern = ocrTrace.begin();
           posPattern != ocrTrace.end();
           ++posPattern) {
        ocrPatternCounter_.registerPattern(*posPattern, 1);
      }
    }
  } // if shouldBeProfiled_ then compute gt pattern probs for hist and ocr

  // The contributions of the token to the various figures concerning precision
  // and recall are added to the global counter

  getCounter(L"shouldBeProfiled") += evalTok.getCounter(L"shouldBeProfiled");
  getCounter(L"shouldDetectHistPattern") +=
    evalTok.getCounter(L"shouldDetectHistPattern");
  getCounter(L"shouldDetectOCRPattern") +=
    evalTok.getCounter(L"shouldDetectOCRPattern");

  getCounter(L"wasProfiled") += evalTok.getCounter(L"wasProfiled");

  getCounter(L"guessed_histTrace_good") +=
    evalTok.getCounter(L"guessed_histTrace_good");
  getCounter(L"guessed_histTrace_nonempty") +=
    evalTok.getCounter(L"guessed_histTrace_nonempty");
  getCounter(L"guessed_histTrace_nonempty_good") +=
    evalTok.getCounter(L"guessed_histTrace_nonempty_good");

  getCounter(L"guessed_ocrTrace_good") +=
    evalTok.getCounter(L"guessed_ocrTrace_good");
  getCounter(L"guessed_ocrTrace_nonempty") +=
    evalTok.getCounter(L"guessed_ocrTrace_nonempty");
  getCounter(L"guessed_ocrTrace_nonempty_good") +=
    evalTok.getCounter(L"guessed_ocrTrace_nonempty_good");

  getCounter(L"papereval_allWords") +=
    evalTok.getCounter(L"papereval_allWords");
  getCounter(L"papereval_isCorrect") +=
    evalTok.getCounter(L"papereval_isCorrect");
  getCounter(L"papereval_isModern") +=
    evalTok.getCounter(L"papereval_isModern");
  getCounter(L"papereval_guessed_wOrig_good") +=
    evalTok.getCounter(L"papereval_guessed_wOrig_good");
  getCounter(L"papereval_guessed_baseWord_good") +=
    evalTok.getCounter(L"papereval_guessed_baseWord_good");

  // simply transfer all values of the token-wise data structure to the global
  // one.
  for (std::map<csl::Pattern, std::map<std::wstring, double>>::const_iterator
         pat = evalTok.counterPerPattern_.begin();
       pat != evalTok.counterPerPattern_.end();
       ++pat) {
    for (std::map<std::wstring, double>::const_iterator key =
           pat->second.begin();
         key != pat->second.end();
         ++key) {
      counterPerPattern_[pat->first][key->first] += key->second;
      // std::wcerr << L"counterPerPattern_[" << pat->first.toString() << L"]["
      // << key->first << L"] += " << key->second << std::endl;
    }
  }

  std::wstring correctionSuggestion;
  if (!evalTok.tok_.isNormal() || evalTok.tok_.isShort()) {
  } else {
    /*
     * This "correction statistics" only considers the top-ranked interpretation
     */
    if (!evalTok.tok_.getCandidateSet()->empty()) {
      // This assumes that the tokens in the candidateSet are lower case
      // already!!!
      Profiler_Interpretation const& topCand =
        evalTok.tok_.getCandidateSet()->at(0);
      correctionSuggestion = topCand.getWord();

      if (evalTok.tok_.getWOCR_lc() ==
          evalTok.copyOfGroundtruth_.getWOrig_lc()) { // ocr correct
        getCounter(L"ocrCorrect") += 1;
        if (correctionSuggestion == evalTok.copyOfGroundtruth_.getWOrig_lc()) {
          getCounter(L"ocrCorrect_suggestionCorrect") += 1;
        } else {
          getCounter(L"ocrCorrect_suggestionWrong") += 1;
          // std::wcout << "MAGIC::ocrCorrect_suggestionWrong(log)=" <<
          // (int)(log( evalTok.tok_.getProbNormalizationFactor() ) / log(10)) <<
          // std::endl;
        }

        if (evalTok.tok_.getAbbyySpecifics().isSuspicious()) {
          getCounter(L"ocrCorrect_abbyySuspicious") += 1;
        } else {
          getCounter(L"ocrCorrect_abbyyNotSuspicious") += 1;
        }
        if (evalTok.tok_.isSuspicious()) { // profiler suspicious
          getCounter(L"ocrCorrect_profilerSuspicious") += 1;
        } else {
          getCounter(L"ocrCorrect_profilerNotSuspicious") += 1;
        }
      } else { // ocr wrong
        getCounter(L"ocrWrong") += 1;

        if (evalTok.tok_.getAbbyySpecifics().isSuspicious()) {
          getCounter(L"ocrWrong_abbyySuspicious") += 1;
        } else {
          getCounter(L"ocrWrong_abbyyNotSuspicious") += 1;
        }
        if (evalTok.tok_.isSuspicious()) { // profiler suspicious
          getCounter(L"ocrWrong_profilerSuspicious") += 1;
        } else {
          getCounter(L"ocrWrong_profilerNotSuspicious") += 1;
          std::wcout << "ocrWrong_profilerNotSuspicious: wOrig="
                     << evalTok.copyOfGroundtruth_.getWOrig_lc()
                     << ", wOCR=" << evalTok.tok_.getWOCR_lc() << std::endl;
        }

        // 		    std::wcerr << "[COUNTS]:wOrig=" <<
        // evalTok.copyOfGroundtruth_.getWOrig_lc()
        // 			       << "\t wOCR=" <<
        // evalTok.tok_.getWOCR_lc()
        // 			       << "\tclassifed=" <<
        // evalTok.copyOfGroundtruth_.getClassified()
        // 			       << "\thistTrace=" <<
        // evalTok.copyOfGroundtruth_.getHistTrace()
        // 			       << "\tocrTrace=" <<
        // evalTok.copyOfGroundtruth_.getOCRTrace()
        // 			       << std::endl;

        if (correctionSuggestion != evalTok.tok_.getWOCR_lc()) {
          getCounter(L"ocrWrong_hasCorrectionSuggestion") += 1;
          if (correctionSuggestion ==
              evalTok.copyOfGroundtruth_.getWOrig_lc()) {
            getCounter(L"ocrWrong_suggestionCorrect") += 1; // good correction
            // std::wcout << "MAGIC::ocrWrong_suggestionCorrect(log)=" <<
            // (int)(log( evalTok.tok_.getProbNormalizationFactor() ) / log(10))
            // << std::endl;

          } else {
            getCounter(L"ocrWrong_suggestionWrong") += 1;
            // std::wcout << "MAGIC::ocrWrong_suggestionWrong(log)=" <<
            // (int)(log( evalTok.tok_.getProbNormalizationFactor() ) / log(10))
            // << std::endl;
          }
        } else {
          getCounter(L"ocrWrong_suggestionEquals") += 1;
        }
      } // ocr wrong

      bool histTraceCorrect = false;
      bool ocrTraceCorrect = false;
      if (topCand.getHistTrace().toString() ==
          evalTok.copyOfGroundtruth_.getHistTrace()) {
        histTraceCorrect = true;
      }
      if (topCand.getOCRTrace().toString() ==
          evalTok.copyOfGroundtruth_.getOCRTrace()) {
        ocrTraceCorrect = true;
      }

    } else { // no candidates
      if (evalTok.tok_.getWOCR_lc() ==
          evalTok.copyOfGroundtruth_.getWOrig_lc()) { // ocr correct
        getCounter(L"ocrCorrect") += 1;
        getCounter(L"ocrCorrect_noSuggestion") += 1;

        if (evalTok.tok_.getAbbyySpecifics().isSuspicious()) { // suspicious
          getCounter(L"ocrCorrect_noSuggestion_abbyySuspicious") += 1;
        } else {
          getCounter(L"ocrCorrect_noSuggestion_abbyyNotSuspicious") += 1;
        }
        if (evalTok.tok_.isSuspicious()) { // profiler suspicious
          getCounter(L"ocrCorrect_profilerSuspicious") += 1;
          getCounter(L"ocrCorrect_noSuggestion_profilerSuspicious") += 1;
        } else {
          getCounter(L"ocrCorrect_profilerNotSuspicious") += 1;
          getCounter(L"ocrCorrect_noSuggestion_profilerNotSuspicious") += 1;
        }

      } else { // ocr wrong
               // 		    std::wcerr << "[COUNTS]:wOrig=" <<
               // evalTok.copyOfGroundtruth_.getWOrig_lc()
               // 			       << "\t wOCR=" <<
               // evalTok.tok_.getWOCR_lc()
               // 			       << "\tclassifed=" <<
               // evalTok.copyOfGroundtruth_.getClassified()
               // 			       << "\thistTrace=" <<
               // evalTok.copyOfGroundtruth_.getHistTrace()
               // 			       << "\tocrTrace=" <<
               // evalTok.copyOfGroundtruth_.getOCRTrace()
               // 			       << std::endl;
        getCounter(L"ocrWrong") += 1;
        getCounter(L"ocrWrong_noSuggestion") += 1;

        if (evalTok.tok_.getAbbyySpecifics().isSuspicious()) { // suspicious
          getCounter(L"ocrWrong_noSuggestion_abbyySuspicious") += 1;
        } else {
          getCounter(L"ocrWrong_noSuggestion_abbyyNotSuspicious") += 1;
        }

        if (evalTok.tok_.isSuspicious()) { // profiler suspicious
          getCounter(L"ocrWrong_profilerSuspicious") += 1;
          getCounter(L"ocrWrong_noSuggestion_profilerSuspicious") += 1;
        } else {
          std::wcout << "---HAAA?---" << std::endl;
          getCounter(L"ocrWrong_profilerNotSuspicious") += 1;
          getCounter(L"ocrWrong_noSuggestion_profilerNotSuspicious") += 1;
        }
      }
    }
  } // token normal and long

} // registerToken

void
Profiler::Evaluation::finish()
{
  // compute probabilities for hist. variant patterns
  histPatternProbabilities_.clear();
  for (PatternCounter::PatternIterator it = histPatternCounter_.patternsBegin();
       it != histPatternCounter_.patternsEnd();
       ++it) {
    //                                   pattern     its count        the
    //                                   left-hand-side's nGramCount // absolute
    //                                   freq
    histPatternProbabilities_.setWeight(
      it->first,
      (it->second / histPatternCounter_.getNGramCount(it->first.getLeft())),
      it->second);
    // 	    std::wcerr << "[GROUNDTRUTH]histPatternProbabilities_.setWeight( "
    // << it->first.toString()
    // 		       <<", ( " << it->second << " / " <<
    // histPatternCounter_.getNGramCount( it->first.getLeft() ) << " ) )"
    // 		       << "=" << ( it->second / histPatternCounter_.getNGramCount(
    // it->first.getLeft() ) ) << std::endl;
  }

  // compute probabilities for ocr patterns
  ocrPatternProbabilities_.clear();
  for (PatternCounter::PatternIterator it = ocrPatternCounter_.patternsBegin();
       it != ocrPatternCounter_.patternsEnd();
       ++it) {
    // hack: applies to ins and del
    if (it->first.getLeft().size() + it->first.getRight().size() == 1) {
      continue;
    }

    // applies to insertions
    // applies to insertions
    // if( it->first.getLeft().size() == 0 && it->first.getRight().size() == 1 )
    // { 	continue;
    // }

    //                                   pattern     its count        the
    //                                   left-hand-side's nGramCount // absolute
    //                                   freq
    ocrPatternProbabilities_.setWeight(
      it->first,
      (it->second / ocrPatternCounter_.getNGramCount(it->first.getLeft())),
      it->second);
  }

  createTopKListStatistics(L"hist",
                           histPatternProbabilities_,
                           myProfiler_.getHistPatterns(),
                           &topKListStats_hist_);
  createTopKListStatistics(L"ocr",
                           ocrPatternProbabilities_,
                           myProfiler_.getOCRPatterns(),
                           &topKListStats_ocr_);

  finished_ = true;
}

PatternContainer const&
Profiler::Evaluation::getHistPatternProbabilities() const
{
  if (!finished_)
    throw OCRCException(
      "OCRC::Profiler::Evaluation::getHistPatternProbabilities: called with "
      "finished_==false");
  return histPatternProbabilities_;
}

PatternContainer const&
Profiler::Evaluation::getOCRPatternProbabilities() const
{
  if (!finished_)
    throw OCRCException(
      "OCRC::Profiler::Evaluation::getOCRPatternProbabilities: called with "
      "finished_==false");
  return ocrPatternProbabilities_;
}

double
Profiler::Evaluation::getPrecisionByPattern(std::wstring const& which,
                                            csl::Pattern const& pat) const
{
  std::wstring prefix(L"guessed_");
  prefix += which;
  prefix += L"Pattern";

  if (counterPerPattern_.find(pat) == counterPerPattern_.end()) {
    return -1;
  } else {
    return counterPerPattern_.find(pat)
             ->second.find(prefix + L"_good")
             ->second /
           counterPerPattern_.find(pat)->second.find(prefix)->second;
  }
}

double
Profiler::Evaluation::getRecallByPattern(std::wstring const& which,
                                         csl::Pattern const& pat) const
{
  std::wstring numeratorKey, denominatorKey;
  if (which == L"hist") {
    numeratorKey = L"guessed_histPattern_good";
    denominatorKey = L"shouldDetectHistPattern";
  } else if (which == L"ocr") {
    numeratorKey = L"guessed_ocrPattern_good";
    denominatorKey = L"shouldDetectOCRPattern";
  } else {
    throw OCRCException("OCRC::Profiler::Evaluation::getRecallByPattern: arg "
                        "'which' can only be 'hist' or 'arg'");
  }

  if (counterPerPattern_.find(pat) == counterPerPattern_.end()) {
    return -1;
  } else {
    return counterPerPattern_.find(pat)->second.find(numeratorKey)->second /
           counterPerPattern_.find(pat)->second.find(denominatorKey)->second;
  }
}

void
Profiler::Evaluation::writeStatistics(std::wostream& os,
                                      size_t iterationNumber) const
{

  os << "<h1>Evaluation</h1>" << std::endl
     << "Skipped for evaluation: " << getCounter(L"skippedForEvaluation")
     << std::endl
     << "<h2>Pattern Precision/ Recall including empty instructions</h2>"
     << std::endl
     << "<table>" << std::endl
     << "<tr><th/><th colspan='2'>Hist</th><th colspan='2'>OCR</th>"
     << std::endl

     << "<tr><th>Precision</th>"
     // hist
     << "<td>" << getCounter(L"guessed_histTrace_good") << " / "
     << getCounter(L"wasProfiled") << "</td>"
     << "<td id=\"DATA_precision_hist_it" << iterationNumber << "\">"
     << (getCounter(L"guessed_histTrace_good") / getCounter(L"wasProfiled"))
     << "</td>"
     << std::endl
     // ocr
     << "<td>" << getCounter(L"guessed_ocrTrace_good") << " / "
     << getCounter(L"wasProfiled") << "</td>"
     << "<td id=\"DATA_precision_ocr_it" << iterationNumber << "\">"
     << (getCounter(L"guessed_ocrTrace_good") / getCounter(L"wasProfiled"))
     << "</td></tr>" << std::endl;

  os << "<tr><th>Recall</th>"
     // hist
     << "<td>" << getCounter(L"guessed_histTrace_good") << " / "
     << getCounter(L"shouldBeProfiled") << "</td>"
     << "<td id=\"DATA_recall_hist_it" << iterationNumber << "\">"
     << (getCounter(L"guessed_histTrace_good") /
         getCounter(L"shouldBeProfiled"))
     << "</td>"
     << std::endl
     // ocr
     << "<td>" << getCounter(L"guessed_ocrTrace_good") << " / "
     << getCounter(L"shouldBeProfiled") << "</td>"
     << "<td id=\"DATA_recall_ocr_it" << iterationNumber << "\">"
     << (getCounter(L"guessed_ocrTrace_good") / getCounter(L"shouldBeProfiled"))
     << "</td></tr>" << std::endl
     << "</table>" << std::endl;

  os << "<h2>Pattern Precision/ Recall without empty instructions</h2>"
     << std::endl
     << "<table>" << std::endl
     << "<tr><th/><th colspan='2'>Hist</th><th colspan='2'>OCR</th>"
     << std::endl

     << "<tr><th>Precision</th>"
     // hist
     << "<td>" << getCounter(L"guessed_histTrace_nonempty_good") << " / "
     << getCounter(L"guessed_histTrace_nonempty") << "</td>"
     << "<td id=\"DATA_precision_hist_nonempty_it" << iterationNumber << "\">"
     << (getCounter(L"guessed_histTrace_nonempty_good") /
         getCounter(L"guessed_histTrace_nonempty"))
     << "</td>"
     << std::endl
     // ocr
     << "<td>" << getCounter(L"guessed_ocrTrace_nonempty_good") << " / "
     << getCounter(L"guessed_ocrTrace_nonempty") << "</td>"
     << "<td id=\"DATA_precision_ocr_nonempty_it" << iterationNumber << "\">"
     << (getCounter(L"guessed_ocrTrace_nonempty_good") /
         getCounter(L"guessed_ocrTrace_nonempty"))
     << "</td></tr>" << std::endl;

  os << "<tr><th>Recall</th>"
     // hist
     << "<td>" << getCounter(L"guessed_histTrace_nonempty_good") << " / "
     << getCounter(L"shouldDetectHistPattern") << "</td>"
     << "<td id=\"DATA_recall_hist_nonempty_it" << iterationNumber << "\">"
     << (getCounter(L"guessed_histTrace_nonempty_good") /
         getCounter(L"shouldDetectHistPattern"))
     << "</td>"
     << std::endl
     // ocr
     << "<td>" << getCounter(L"guessed_ocrTrace_nonempty_good") << " / "
     << getCounter(L"shouldDetectOCRPattern") << "</td>"
     << "<td id=\"DATA_recall_ocr_nonempty_it" << iterationNumber << "\">"
     << (getCounter(L"guessed_ocrTrace_nonempty_good") /
         getCounter(L"shouldDetectOCRPattern"))
     << "</td></tr>" << std::endl
     << "</table>" << std::endl;

  os << "<h2>Paper statistics</h2>" << std::endl
     << "Considers all tokens except short tokens" << std::endl
     << "<table>" << std::endl
     << "<tr><td>tokens</td><td id=\"DATA_papereval_allWords_it"
     << iterationNumber << "\">" << getCounter(L"papereval_allWords")
     << "</td></tr>" << std::endl
     << "<tr><td>correct</td><td id=\"DATA_papereval_isCorrect_it"
     << iterationNumber << "\">" << getCounter(L"papereval_isCorrect")
     << "</td></tr>" << std::endl
     << "<tr><td>modern</td><td id=\"DATA_papereval_isModern_it"
     << iterationNumber << "\">" << getCounter(L"papereval_isModern")
     << "</td></tr>" << std::endl
     << "<tr><td>profiler correct</td><td "
        "id=\"DATA_papereval_guessed_wOrig_good_it"
     << iterationNumber << "\">" << getCounter(L"papereval_guessed_wOrig_good")
     << "</td></tr>" << std::endl
     << "<tr><td>profiler modern</td><td "
        "id=\"DATA_papereval_guessed_baseWord_good_it"
     << iterationNumber << "\">"
     << getCounter(L"papereval_guessed_baseWord_good") << "</td></tr>"
     << std::endl
     << "</table>" << std::endl;

  os << "<h2>Correction statistics</h2>" << std::endl
     << "Here, short tokens do not go into the statistics" << std::endl
     << "<table>" << std::endl
     << "<tr><td>ocrCorrect</td><td id=\"DATA_count_ocrCorrect_it"
     << iterationNumber << "\">" << getCounter(L"ocrCorrect") << "</td></tr>"
     << std::endl
     << "<tr><td>ocrWrong</td><td id=\"DATA_count_ocrWrong_it"
     << iterationNumber << "\">" << getCounter(L"ocrWrong") << "</td></tr>"
     << std::endl
     << "<tr><td>ocrCorrect_suggestionCorrect</td><td "
        "id=\"DATA_count_ocrCorrect_suggestionCorrect_it"
     << iterationNumber << "\">" << getCounter(L"ocrCorrect_suggestionCorrect")
     << "</td></tr>" << std::endl
     << "<tr><td>ocrCorrect_suggestionWrong</td><td "
        "id=\"DATA_count_ocrCorrect_suggestionWrong_it"
     << iterationNumber << "\">" << getCounter(L"ocrCorrect_suggestionWrong")
     << "</td></tr>" << std::endl
     << "<tr><td>ocrCorrect_noSuggestion</td><td "
        "id=\"DATA_count_ocrCorrect_noSuggestion_it"
     << iterationNumber << "\">" << getCounter(L"ocrCorrect_noSuggestion")
     << "</td></tr>" << std::endl
     << "<tr><td>ocrWrong</td><td>" << getCounter(L"ocrWrong") << "</td></tr>"
     << std::endl
     << "<tr><td>ocrWrong_hasCorrectionSuggestion</td><td "
        "id=\"DATA_count_ocrWrong_hasCorrectionSuggestion_it"
     << iterationNumber << "\">"
     << getCounter(L"ocrWrong_hasCorrectionSuggestion") << "</td></tr>"
     << std::endl
     << "<tr><td>ocrWrong_suggestionCorrect</td><td "
        "id=\"DATA_count_ocrWrong_suggestionCorrect_it"
     << iterationNumber << "\">" << getCounter(L"ocrWrong_suggestionCorrect")
     << "</td></tr>" << std::endl
     << "<tr><td>ocrWrong_suggestionWrong</td><td "
        "id=\"DATA_count_ocrWrong_suggestionWrong_it"
     << iterationNumber << "\">" << getCounter(L"ocrWrong_suggestionWrong")
     << "</td></tr>" << std::endl
     << "<tr><td>ocrWrong_suggestionEquals</td><td "
        "id=\"DATA_count_ocrWrong_suggestionEquals_it"
     << iterationNumber << "\">" << getCounter(L"ocrWrong_suggestionEquals")
     << "</td></tr>" << std::endl
     << "<tr><td>ocrWrong_noSuggestion</td><td "
        "id=\"DATA_count_ocrWrong_noSuggestion_it"
     << iterationNumber << "\">" << getCounter(L"ocrWrong_noSuggestion")
     << "</td></tr>" << std::endl;

  double errordetection_precision =
    ((getCounter(L"ocrWrong_profilerSuspicious")) /
     (getCounter(L"ocrWrong_profilerSuspicious") +
      getCounter(L"ocrCorrect_profilerSuspicious")));
  double errordetection_recall =
    (getCounter(L"ocrWrong_profilerSuspicious") / getCounter(L"ocrWrong"));

  os << "<tr><td>errordetection_precision<td "
        "id=\"DATA_errordetection_precision_it"
     << iterationNumber << "\">" << errordetection_precision << "</td></tr>"
     << std::endl
     << "<tr><td>errordetection_recall<td id=\"DATA_errordetection_recall_it"
     << iterationNumber << "\">" << errordetection_recall << "</td></tr>"
     << std::endl
     << "</table>" << std::endl;

  size_t penaltyFactor = 5;
  size_t workloadAbbyy =
    getCounter(L"ocrCorrect_abbyySuspicious") +
    getCounter(L"ocrCorrect_noSuggestion_abbyySuspicious") +
    getCounter(L"ocrWrong_abbyySuspicious") +
    getCounter(L"ocrWrong_abbyyNotSuspicious") * penaltyFactor +
    getCounter(L"ocrWrong_noSuggestion_abbyySuspicious") +
    getCounter(L"ocrWrong_noSuggestion_abbyyNotSuspicious") * penaltyFactor;

  os << "<h2>Suspiciousness/ Figure of Merit</h2>" << std::endl
     << "Here, short tokens do not go into the statistics" << std::endl
     << "<h3>Abbyy</h3>" << std::endl
     << "<table>" << std::endl
     << "<tr><td/><td/><td>Workload</td></tr>" << std::endl
     << "<tr><td>ocrWrong</td><td>" << getCounter(L"ocrWrong")
     << "</td><td/></tr>" << std::endl
     << "<tr><td>ocrCorrect_abbyyNotSuspicious</td><td>"
     << getCounter(L"ocrCorrect_abbyyNotSuspicious") << "</td><td>"
     << "</td></tr>" << std::endl
     << "<tr><td>ocrCorrect_abbyySuspicious</td><td>"
     << getCounter(L"ocrCorrect_abbyySuspicious") << "</td><td>"
     << getCounter(L"ocrCorrect_abbyySuspicious") << "</td></tr>" << std::endl
     << "<tr><td>ocrCorrect_noSuggestion_abbyyNotSuspicious</td><td>"
     << getCounter(L"ocrCorrect_noSuggestion_abbyyNotSuspicious") << "</td><td>"
     << "</td></tr>" << std::endl
     << "<tr><td>ocrCorrect_noSuggestion_abbyySuspicious</td><td>"
     << getCounter(L"ocrCorrect_noSuggestion_abbyySuspicious") << "</td><td>"
     << getCounter(L"ocrCorrect_noSuggestion_abbyySuspicious") << "</td></tr>"
     << std::endl
     << "<tr><td>ocrWrong_abbyySuspicious</td><td>"
     << getCounter(L"ocrWrong_abbyySuspicious") << "</td><td>"
     << getCounter(L"ocrWrong_abbyySuspicious") << "</td></tr>" << std::endl
     << "<tr><td>ocrWrong_abbyyNotSuspicious</td><td>"
     << getCounter(L"ocrWrong_abbyyNotSuspicious") << "</td><td>"
     << getCounter(L"ocrWrong_abbyyNotSuspicious") * penaltyFactor
     << "</td></tr>" << std::endl
     << "<tr><td>ocrWrong_noSuggestion_abbyySuspicious</td><td>"
     << getCounter(L"ocrWrong_noSuggestion_abbyySuspicious") << "</td><td>"
     << getCounter(L"ocrWrong_noSuggestion_abbyySuspicious") << "</td></tr>"
     << std::endl
     << "<tr><td>ocrWrong_noSuggestion_abbyyNotSuspicious</td><td>"
     << getCounter(L"ocrWrong_noSuggestion_abbyyNotSuspicious") << "</td><td>"
     << getCounter(L"ocrWrong_noSuggestion_abbyyNotSuspicious") * penaltyFactor
     << "</td></tr>" << std::endl
     << "<tr><td/><td/><td>" << workloadAbbyy << "</td></tr>" << std::endl
     << "</table>" << std::endl;

  size_t workloadProfiler =
    getCounter(L"ocrCorrect_profilerSuspicious") +
    getCounter(L"ocrCorrect_noSuggestion_profilerSuspicious") +
    getCounter(L"ocrWrong_profilerSuspicious") +
    getCounter(L"ocrWrong_profilerNotSuspicious") * penaltyFactor +
    getCounter(L"ocrWrong_noSuggestion_profilerSuspicious") +
    getCounter(L"ocrWrong_noSuggestion_profilerNotSuspicious") * penaltyFactor;

  os << "<h3>Profiler</h3>" << std::endl
     << "<table>" << std::endl
     << "<tr><td/><td/><td>Workload</td></tr>" << std::endl
     << "<tr><td>ocrWrong</td><td>" << getCounter(L"ocrWrong")
     << "</td><td/></tr>" << std::endl
     << "<tr><td>ocrCorrect_profilerSuspicious</td><td>"
     << getCounter(L"ocrCorrect_profilerSuspicious") << "</td><td>"
     << getCounter(L"ocrCorrect_profilerSuspicious") << "</td></tr>"
     << std::endl
     << "<tr><td>ocrCorrect_noSuggestion_profilerSuspicious</td><td>"
     << getCounter(L"ocrCorrect_noSuggestion_profilerSuspicious") << "</td><td>"
     << getCounter(L"ocrCorrect_noSuggestion_profilerSuspicious")
     << "</td></tr>" << std::endl
     << "<tr><td>ocrWrong_profilerSuspicious</td><td>"
     << getCounter(L"ocrWrong_profilerSuspicious") << "</td><td>"
     << getCounter(L"ocrWrong_profilerSuspicious") << "</td></tr>" << std::endl
     << "<tr><td>ocrWrong_profilerNotSuspicious</td><td>"
     << getCounter(L"ocrWrong_profilerNotSuspicious") << "</td><td>"
     << getCounter(L"ocrWrong_profilerNotSuspicious") * penaltyFactor
     << "</td></tr>" << std::endl
     << "<tr><td>ocrWrong_noSuggestion_profilerSuspicious</td><td>"
     << getCounter(L"ocrWrong_noSuggestion_profilerSuspicious") << "</td><td>"
     << getCounter(L"ocrWrong_noSuggestion_profilerSuspicious") << "</td></tr>"
     << std::endl
     << "<tr><td>ocrWrong_noSuggestion_profilerNotSuspicious</td><td>"
     << getCounter(L"ocrWrong_noSuggestion_profilerNotSuspicious")
     << "</td><td>"
     << getCounter(L"ocrWrong_noSuggestion_profilerNotSuspicious") *
          penaltyFactor
     << "</td></tr>" << std::endl
     << "<tr><td/><td/><td>" << workloadProfiler << "</td></tr>" << std::endl
     << "<tr><td/><td/><td>"
     << ((double)(workloadProfiler) / static_cast<double>(workloadAbbyy))
     << "</td></tr>" << std::endl
     << "</table>" << std::endl;

  // 	std::wofstream histPatternFile( "./histPatterns_groundtruth.xml" );
  // 	histPatternProbabilities_.writeToXML( histPatternFile );
  // 	histPatternFile.close();

  // 	std::wofstream ocrPatternFile( "./ocrPatterns_groundtruth.xml" );
  // 	ocrPatternProbabilities_.writeToXML( ocrPatternFile );
  // 	ocrPatternFile.close();
}

void
Profiler::Evaluation::writeHistPatternPrecisionRecall(
  PatternContainer const& histPatterns,
  std::wostream& os) const
{
  std::vector<std::pair<csl::Pattern, double>> patternsSorted;
  histPatterns.sortToVector(&patternsSorted);
  for (std::vector<std::pair<csl::Pattern, double>>::const_iterator it =
         patternsSorted.begin();
       it != patternsSorted.end();
       ++it) {
    if (counterPerPattern_.find(it->first) == counterPerPattern_.end()) {
      os << "???" << std::endl;
    } else {
      os << "<span style=\"cursor:help\" title=\"Prec="
         << counterPerPattern_.find(it->first)
              ->second.find(L"guessed_histPattern_good")
              ->second
         << " / "
         << counterPerPattern_.find(it->first)
              ->second.find(L"guessed_histPattern")
              ->second
         << " = " << getPrecisionByPattern(L"hist", it->first) << ", Rec="
         << counterPerPattern_.find(it->first)
              ->second.find(L"guessed_histPattern_good")
              ->second
         << " / "
         << counterPerPattern_.find(it->first)
              ->second.find(L"shouldDetectHistPattern")
              ->second
         << " = " << getRecallByPattern(L"hist", it->first) << "\">"

         << it->first.getLeft() << "_" << it->first.getRight() << "#"
         << histPatterns.getWeight(it->first) << "#"
         << (histPatterns.getAbsoluteFreqs().find(it->first))->second
         << "</span>" << std::endl;
    }
  }
}

void
Profiler::Evaluation::writeOCRPatternPrecisionRecall(
  PatternContainer const& ocrPatterns,
  std::wostream& os) const
{
  std::vector<std::pair<csl::Pattern, double>> patternsSorted;
  ocrPatterns.sortToVector(&patternsSorted);

  for (std::vector<std::pair<csl::Pattern, double>>::const_iterator it =
         patternsSorted.begin();
       it != patternsSorted.end();
       ++it) {
    if (counterPerPattern_.find(it->first) == counterPerPattern_.end()) {
      os << "???" << std::endl;
    } else {
      os << "<span style=\"cursor:help\" title=\"Prec="
         << counterPerPattern_.find(it->first)
              ->second.find(L"guessed_ocrPattern_good")
              ->second
         << " / "
         << counterPerPattern_.find(it->first)
              ->second.find(L"guessed_ocrPattern")
              ->second
         << " = "
         << counterPerPattern_.find(it->first)
                ->second.find(L"guessed_ocrPattern_good")
                ->second /
              counterPerPattern_.find(it->first)
                ->second.find(L"guessed_ocrPattern")
                ->second
         << ", Rec="
         << counterPerPattern_.find(it->first)
              ->second.find(L"guessed_ocrPattern_good")
              ->second
         << " / "
         << counterPerPattern_.find(it->first)
              ->second.find(L"shouldDetectOCRPattern")
              ->second
         << " = "
         << counterPerPattern_.find(it->first)
                ->second.find(L"guessed_ocrPattern_good")
                ->second /
              counterPerPattern_.find(it->first)
                ->second.find(L"shouldDetectOCRPattern")
                ->second
         << "\">"

         << it->first.getLeft() << "_" << it->first.getRight() << "#"
         << ocrPatterns.getWeight(it->first) << "#"
         << (ocrPatterns.getAbsoluteFreqs().find(it->first))->second
         << "</span>" << std::endl;
    }
  }
}

void
Profiler::Evaluation::createTopKListStatistics(
  std::wstring const& which,
  PatternContainer const& groundtruthPatterns,
  PatternContainer const& profiledPatterns,
  TopKListStatistics* stats)
{

  // create sorted lists from the pattern maps
  std::vector<std::pair<csl::Pattern, double>> patterns_profiled_sorted;
  profiledPatterns.sortToVector(&patterns_profiled_sorted);

  std::vector<std::pair<csl::Pattern, double>> patterns_gt_sorted;
  groundtruthPatterns.sortToVector(&patterns_gt_sorted);

  // These sets contain all patterns that are present in the current head of
  // each list
  std::set<csl::Pattern> isInProfiledList;
  std::set<csl::Pattern> isInGroundtruthList;

  // These are for the rec/prec-values regarding the presence of profiled
  // patterns in the groundtruth list and vice versa
  size_t matches = 0;

  stats->matchesInTopK_.push_back(-1); // dummy values for "the first 0 ranks"

  std::vector<std::pair<csl::Pattern, double>>::const_iterator profIterator =
    patterns_profiled_sorted.begin();
  std::vector<std::pair<csl::Pattern, double>>::const_iterator gtIterator =
    patterns_gt_sorted.begin();

  size_t rank = 0;
  double shouldDetectPattern = 0;
  double guessedPattern = 0;
  double guessedPattern_good = 0;
  while ((profIterator != patterns_profiled_sorted.end()) ||
         (gtIterator != patterns_gt_sorted.end())) {

    ++rank;

    // first, add new patterns to the repective sets
    if (profIterator != patterns_profiled_sorted.end()) {
      isInProfiledList.insert(profIterator->first);
    }
    if (gtIterator != patterns_gt_sorted.end()) {
      isInGroundtruthList.insert(gtIterator->first);
    }

    // now, do the cross-check if the new pattern on the profiler-side is also
    // on the groundtruth side and vice versa
    if (profIterator != patterns_profiled_sorted.end() &&
        (isInGroundtruthList.count(profIterator->first) == 1)) {
      matches += 1;
      // std::wcerr << "rank=" << rank << ", " << profIterator->first.toString()
      // << " current profiledPattern is in groundtruth" << std::endl;
    }

    if (gtIterator != patterns_gt_sorted.end() &&
        (isInProfiledList.count(gtIterator->first) == 1)) {
      if ((profIterator != patterns_profiled_sorted.end()) &&
          (profIterator->first !=
           gtIterator
             ->first)) { // if both patterns are equal, don't give credit twice
        matches += 1;
        // std::wcerr << "rank=" << rank << ", " <<
        // profIterator->first.toString() << " current groundtruthPattern is in
        // profiled" << std::endl;
      }
    }

    // Now, calculate nr of matches for the current head of the lists
    stats->matchesInTopK_.push_back((double)matches / (double)rank);

    if (profIterator != patterns_profiled_sorted.end()) {
      ++profIterator;
    }
    if (gtIterator != patterns_gt_sorted.end()) {
      ++gtIterator;
    }
  }
}

///////////// THOSE THREE ARE FOR class Evaluation !!!!!!

double&
Profiler::Evaluation::getCounter(std::wstring const& key)
{
  std::map<std::wstring, double>::iterator it = counter_.find(key);
  if (it == counter_.end())
    throw OCRCException(
      std::wstring(L"OCRC::Profiler::Evaluation::getCounter Unknown key: ") +
      key);
  return it->second;
}

double
Profiler::Evaluation::getCounter(std::wstring const& key) const
{
  std::map<std::wstring, double>::const_iterator it = counter_.find(key);
  if (it == counter_.end())
    throw OCRCException(
      std::wstring(L"OCRC::Profiler::Evaluation::getCounter Unknown key: ") +
      key);
  return it->second;
}

void
Profiler::Evaluation::initCounter(std::wstring const& key)
{
  counter_[key] = 0;
}

} // eon
