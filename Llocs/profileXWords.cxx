#include <algorithm>
#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include <locale>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "Profiler/Profiler.h"
#include "Llocs/LlocsReader.h"

namespace ocrc = OCRCorrection;
static const double THRESHOLD = .6;

////////////////////////////////////////////////////////////////////////////////
static bool
isCorrect(const ocrc::Token& t)
{
        return t.isNormal() and
                t.getNrOfCandidates() > 0 and
                t.candidatesBegin()->getVoteWeight() >= THRESHOLD;
}

////////////////////////////////////////////////////////////////////////////////
static ocrc::Document::iterator
getCorrectRangeStart(const ocrc::LlocsReader& reader,
                     ocrc::Document::iterator b,
                     ocrc::Document::iterator e)
{
        while (b != e and not isCorrect(*b)) {
                ++b;
        }
        return b;
}

////////////////////////////////////////////////////////////////////////////////
static ocrc::Document::iterator
getCorrectRangeEnd(const ocrc::LlocsReader& reader,
                   ocrc::Document::iterator b,
                   ocrc::Document::iterator e)
{
        if (b != e) {
                // origin file of the token
                const char *path = reader.getTloc(*b).path;
                for (; b != e; ++b) {
                        if ((strcmp(path, reader.getTloc(*b).path) != 0) or
                            (b->isNormal() and not isCorrect(*b))) {
                                break;
                        }
                }
        }
        return b;
}

////////////////////////////////////////////////////////////////////////////////
static const std::wstring&
getCorrectToken(const ocrc::Token& token)
{
        return token.getNrOfCandidates() ?
                token.candidatesBegin()->getString():
                token.getWOCR();
}

////////////////////////////////////////////////////////////////////////////////
static void
printRange(ocrc::LlocsReader& reader,
           ocrc::Document::iterator b,
           ocrc::Document::iterator e)
{
        double start, end;
        start = std::numeric_limits<double>::max();
        end = 0;
        const char *path = nullptr;
        for (; b != e; ++b) {
                std::wcout << getCorrectToken(*b);
                // if (b->getNrOfCandidates())
                //         std::wcout << "/" << b->candidatesBegin()->getVoteWeight();

                const auto& tloc = reader.getTloc(*b);
                start = std::min(start, tloc.start);
                end = std::max(end, tloc.end);
                path = tloc.path;
        }
        if (path)
                std::wcout << " " << start << " " << end << " " << path << "\n";
}

////////////////////////////////////////////////////////////////////////////////
static void
print(ocrc::LlocsReader& reader, ocrc::Document& document)
{
        const auto e = document.end();
        for (auto i = document.begin(); i != e; ) {
                auto rb = getCorrectRangeStart(reader, i, e);
                auto re = getCorrectRangeEnd(reader, rb, e);
                printRange(reader, rb, re);
                i = re;
        }
}

////////////////////////////////////////////////////////////////////////////////
static int
run(int argc, char **argv)
{
        assert(argc > 2);
        ocrc::Document document;
        ocrc::LlocsReader reader;
        reader.parse(argc - 2, argv + 2, document);

        ocrc::Profiler profiler;
        profiler.readConfiguration(argv[1]);
        profiler.createProfile(document);

        print(reader, document);
        return 0;
}

////////////////////////////////////////////////////////////////////////////////
int
main(int argc, char **argv)
{
        std::locale::global(std::locale(""));
        if (argc < 3) {
                std::wcerr << "Usage: " << argv[0] << " config llocs [llocs...]\n";
                return 1;
        }

        try {
                return run(argc, argv);
        } catch (const std::exception& e) {
                std::wcerr << "[error] " << e.what() << "\n";
        }
        return 1;
}
