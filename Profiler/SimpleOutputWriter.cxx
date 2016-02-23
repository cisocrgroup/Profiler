#include "Document/Document.h"
#include "SimpleOutputWriter.h"

////////////////////////////////////////////////////////////////////////////////
OCRCorrection::SimpleOutputWriter::SimpleOutputWriter(const Document& document):
        document_(document)
{
}

////////////////////////////////////////////////////////////////////////////////
void
OCRCorrection::SimpleOutputWriter::write() const
{
        const Document::const_PageIterator pbeg = document_.pagesBegin();
        const Document::const_PageIterator pend = document_.pagesEnd();
        for (Document::const_PageIterator p = pbeg; p != pend; ++p) {
                const Document::const_iterator tbeg = p->begin();
                const Document::const_iterator tend = p->end();
                for (Document::const_iterator t = tbeg; t != tend; ++t) {
                        if (t->isNormal())
                                writeNormalToken(*t);
                }
        }
}

////////////////////////////////////////////////////////////////////////////////
void
OCRCorrection::SimpleOutputWriter::writeNormalToken(const Token& token) const
{
        bool somethingWasPrinted = false;
        const Token::CandidateIterator cbeg = token.candidatesBegin();
        const Token::CandidateIterator cend = token.candidatesEnd();
        for (Token::CandidateIterator c = cbeg; c != cend; ++c) {
                // skip these: sogleich:{sogleich+[]}+ocr[],voteWeight=1,levDistance=0
                if (c->getDlev() and c->getString() != token.getWOCR()) {
                        somethingWasPrinted = true;
                        std::wcout << "@" << token.getWOCR() << ","
                                   << token.getIndexInDocument() << ","
                                   << c->toString() << std::endl;
                }
        }
        if (not somethingWasPrinted) {
                std::wcout << token.getIndexInDocument() << ","
                           << token.getWOCR() << std::endl;
        }
}
