#include "Document/Document.h"
#include "autocorrection.h"

namespace ocrc = OCRCorrection;

////////////////////////////////////////////////////////////////////////////////
static void fullcorrect(ocrc::Document& doc);

////////////////////////////////////////////////////////////////////////////////
static void correctk(ocrc::Document& doc, size_t k);

////////////////////////////////////////////////////////////////////////////////
void
ocrc::autocorrect(Document& doc, size_t k)
{
        if (k == 0) {
                fullcorrect(doc);
        } else {
                correctk(doc, k);
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
                i->setWOCR(L"");
                i->setWOCR_lc(L"");
                i->setWOCR(i->candidatesBegin()->getWord());
        }
}

////////////////////////////////////////////////////////////////////////////////
void
correctk(ocrc::Document& doc, size_t k)
{
}
