#include "GlobalProfile/GlobalProfile.h"
#include "Profiler/Profiler.h"
#include "Document/Document.h"
#include "autocorrection.h"

namespace ocrc = OCRCorrection;

////////////////////////////////////////////////////////////////////////////////
static void fullcorrect(ocrc::Document& doc);

////////////////////////////////////////////////////////////////////////////////
static void correctkmost(const ocrc::Profiler& profiler, ocrc::Document& doc, size_t k);

////////////////////////////////////////////////////////////////////////////////
static void correct(ocrc::Token& token, const ocrc::Candidate& candidate);

////////////////////////////////////////////////////////////////////////////////
void
ocrc::autocorrect(const Profiler& profiler, Document& doc, size_t k)
{
        if (k == 0) {
                fullcorrect(doc);
        } else {
                correctkmost(profiler, doc, k);
        }
}

////////////////////////////////////////////////////////////////////////////////
void
fullcorrect(ocrc::Document& doc)
{
        const ocrc::Document::iterator b = doc.begin();
        const ocrc::Document::iterator e = doc.end();
        for (ocrc::Document::iterator i = b; i != e; ++i) {
                if (not i->isNormal() or not i->getNrOfCandidates())
                        continue;
                correct(*i, *i->candidatesBegin());
        }
}

////////////////////////////////////////////////////////////////////////////////
void
correctkmost(const ocrc::Profiler& profiler, ocrc::Document& doc, size_t k)
{
	std::vector<std::pair<csl::Pattern, double> > patterns;
        profiler.getOCRPatterns().sortToVector(&patterns);
        if (k < patterns.size())
                patterns.resize(k);

        const size_t n = patterns.size();
        const ocrc::Document::iterator b = doc.begin();
        const ocrc::Document::iterator e = doc.end();
        for (ocrc::Document::iterator i = b; i != e; ++i) {
                const ocrc::Token::CandidateIterator cb = i->candidatesBegin();
                const ocrc::Token::CandidateIterator ce = i->candidatesEnd();
                for (ocrc::Token::CandidateIterator j = cb; j != ce; ++j) {
                        for (size_t k = 0; k < n; ++k) {
                                if (j->getOCRTrace().containsPattern(patterns[k].first)) {
                                        std::wcerr << "Applying correction pattern: "
                                                   << patterns[k].first.getLeft() << ":"
                                                   << patterns[k].first.getRight() << "#"
                                                   << patterns[k].second << "\n";
                                        correct(*i, *j);
                                        goto endofcandidates;
                                }
                        }

                }
        endofcandidates:
                ; // next token
        }
        for (size_t i = 0; i < patterns.size(); ++i) {
                std::wcerr << patterns[i].first.getLeft() << ":"
                           << patterns[i].first.getRight() << "#"
                           << patterns[i].second << "\n";
        }
}

////////////////////////////////////////////////////////////////////////////////
void
correct(ocrc::Token& token, const ocrc::Candidate& candidate)
{
        std::wcerr << "Correcting " << token.getWOCR() << " with "
                   << candidate.getWord() << "\n";
        token.setWCorr(candidate.getWord());
//        std::wcerr << "SETTING CORRECTION: " << token.getWCorr() << "\n";
//        token.setWOCR(L"");
//        token.setWOCR_lc(L"");
//        token.setWOCR(candidate.getWord());
}
