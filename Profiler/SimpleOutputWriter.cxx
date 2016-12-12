#include "Document/Document.h"
#include "SimpleOutputWriter.h"

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
	for (const auto& token: document_) {
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
void
SimpleOutputWriter::writeNormalToken(const Token& token) const
{
	struct Candidates {
		Candidates(const Token& token): t(token) {}
		Token::CandidateIterator begin() const noexcept {return t.candidatesBegin();}
		Token::CandidateIterator end() const noexcept {return t.candidatesEnd();}
		const Token& t;
	};
	bool somethingWasPrinted = false;
	Candidates candidates(token);
	for (const auto& cand: candidates) {
		somethingWasPrinted = true;
		os_ << token.getIndexInDocument() << ","
		    << token.getWOCR() << ","
		    << cand << ","
		    << "dict=" << cand.getDictModule().getName() << "\n";
	}
        if (not somethingWasPrinted) {
                os_ << token.getIndexInDocument() << ","
		    << token.getWOCR() << std::endl;
        }
}
