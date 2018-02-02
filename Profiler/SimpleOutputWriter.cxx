#include "SimpleOutputWriter.h"
#include "Document/Document.h"

using namespace OCRCorrection;

////////////////////////////////////////////////////////////////////////////////
SimpleOutputWriter::SimpleOutputWriter(std::wostream& os,
                                       const Document& document)
  : document_(document)
  , os_(os)
{
}

////////////////////////////////////////////////////////////////////////////////
void
SimpleOutputWriter::write() const
{
  for (const auto& token : document_) {
    // const Document::const_PageIterator pbeg = document_.pagesBegin();
    // const Document::const_PageIterator pend = document_.pagesEnd();
    // for (Document::const_PageIterator p = pbeg; p != pend; ++p) {
    // const Document::const_iterator tbeg = p->begin();
    // const Document::const_iterator tend = p->end();
    // for (Document::const_iterator t = tbeg; t != tend; ++t) {
    if (token.isNormal())
      writeNormalToken(token);
  }
}

////////////////////////////////////////////////////////////////////////////////
// norà:{nora+[(a:à,3)]}+ocr[(à:c,3)],voteWeight=0.000129274,levDistance=1
void
SimpleOutputWriter::writeNormalToken(const Token& token) const
{
  struct Candidates
  {
    Candidates(const Token& token)
      : t(token)
    {
    }
    Token::CandidateIterator begin() const noexcept
    {
      return t.candidatesBegin();
    }
    Token::CandidateIterator end() const noexcept { return t.candidatesEnd(); }
    const Token& t;
  };
  bool somethingWasPrinted = false;
  Candidates candidates(token);
  os_ << "@" << token.getWOCR_lc() << "\n";
  for (const auto& cand : candidates) {
    os_ << cand.getWord() << ":{" << cand.getBaseWord() << "+"
        << cand.getInstruction() << "}+ocr" << cand.getOCRTrace()
        << ",voteWeight=" << cand.getVoteWeight()
        << ",levDistance=" << cand.getLevDistance()
        << ",dict=" << cand.getDictModule().getName() << "\n";
  }
}
