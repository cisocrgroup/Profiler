#include "Document/Document.h"
#include "autocorrection.h"

namespace ocrc = OCRCorrection;

////////////////////////////////////////////////////////////////////////////////
static void fullcorrect(ocrc::Document& doc);

////////////////////////////////////////////////////////////////////////////////
static void correctkmost(ocrc::Document& doc, size_t k);

////////////////////////////////////////////////////////////////////////////////
static void correct(ocrc::Token& token, const ocrc::Candidate& candidate);

////////////////////////////////////////////////////////////////////////////////
void
ocrc::autocorrect(Document& doc, size_t k)
{
        if (k == 0) {
                fullcorrect(doc);
        } else {
                correctkmost(doc, k);
        }
}

////////////////////////////////////////////////////////////////////////////////
void
fullcorrect(ocrc::Document& doc)
{
        const ocrc::Document::iterator b = doc.begin();
        const ocrc::Document::iterator e = doc.end();
        for (ocrc::Document::iterator i = b; i != e; ++i) {
                if (not i->getNrOfCandidates())
                        continue;
                correct(*i, *i->candidatesBegin());
        }
}

// sort
typedef std::map<std::pair<std::wstring, std::wstring>, size_t> Counts;
typedef std::pair<Counts::key_type, Counts::mapped_type> CountPair;
struct Cmp {
        bool operator()(const CountPair& lhs, const CountPair& rhs) const {
                return lhs.second > rhs.second;
        }
};

////////////////////////////////////////////////////////////////////////////////
void
correctkmost(ocrc::Document& doc, size_t k)
{
        Counts counts;

        const ocrc::Document::iterator b = doc.begin();
        const ocrc::Document::iterator e = doc.end();
        for (ocrc::Document::iterator i = b; i != e; ++i) {
                const ocrc::Token::CandidateIterator cb = i->candidatesBegin();
                const ocrc::Token::CandidateIterator ce = i->candidatesEnd();
                for (ocrc::Token::CandidateIterator j = cb; j != ce; ++j) {
                        const size_t n = j->getOCRTrace().size();
                        for (size_t k = 0; k < n; ++k) {
                                counts[std::make_pair(j->getOCRTrace()[k].getLeft(),
                                                      j->getOCRTrace()[k].getRight())]++;
                        }
                }
        }

        std::vector<CountPair> pairs(counts.size());
        std::copy(counts.begin(), counts.end(), pairs.begin());
        std::sort(pairs.begin(), pairs.end(), Cmp());
        for (size_t i = 0; i < pairs.size(); ++i) {
                std::wcerr << pairs[i].first.first << L":"
                           << pairs[i].first.second << L": "
                           << pairs[i].second << L"\n";
        }
}

////////////////////////////////////////////////////////////////////////////////
void
correct(ocrc::Token& token, const ocrc::Candidate& candidate)
{
        token.setWOCR(L"");
        token.setWOCR_lc(L"");
        token.setWOCR(candidate.getWord());
}
