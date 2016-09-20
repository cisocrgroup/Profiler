#include <memory>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
#include <locale>
#include <sstream>
#include <stdexcept>
#include <regex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "Profiler/Profiler.h"
#include "Llocs/LlocsReader.h"
#include "WagnerFischer.hpp"

namespace ocrc = OCRCorrection;
struct Data {
        Data(): llocs(), lex(0), acc(0), conf(0), i(0) {}
        ocrc::LlocsReader::LlocsPtr llocs;
        double lex, acc, conf;
        size_t i;
};

////////////////////////////////////////////////////////////////////////////////
static double
calculateAccuracy(const ocrc::LlocsReader::Llocs& llocs, size_t& dd, size_t& nn)
{
        const static std::regex re{"\\.llocs$"};
        static lev::WagnerFischer wf;

        // gt
        const auto gtfile = std::regex_replace(llocs.path, re, ".gt.txt");
        std::wifstream is(gtfile);
        if (not is.good()) {
                std::wcerr << "[warning] could not open: "
                           << gtfile.data() << "\n";
                return NAN;
        }
        std::wstring gt;
        std::getline(is, gt);
        is.close();

        // prediction
        is.open(std::regex_replace(llocs.path, re, ".txt"));
        if (not is.good()) {
                std::wcerr << "[warning] could not open: " << llocs.path << "\n";
        }
        std::wstring pred;
        std::getline(is, pred);
        is.close();

        //const auto d = wf(gt, llocs.chars);
        const auto d = wf(gt, pred);
        double error = 0;
        // const size_t denom = std::max(gt.size(), llocs.chars.size());
        // const size_t denom = std::max(gt.size(), pred.size());
        //const size_t denom = llocs.chars.size();
        const size_t denom = pred.size();
        if (denom)
                error = double(d) / double(denom);

        dd += d;
        nn += denom;
        return 1 - error;
        //return 1 - (double(d) / double(gt.size()));
}

////////////////////////////////////////////////////////////////////////////////
static double
calculateConfidence(const ocrc::LlocsReader::Llocs& llocs)
{
        double mean = 0;
        for (auto c: llocs.confids) {
                mean += c;
        }
        if (mean > 0)
                mean /= llocs.confids.size();
        return mean;
}

// ////////////////////////////////////////////////////////////////////////////////
// static double
// calculateBinaryConfidence(const ocrc::LlocsReader::Llocs& llocs, double mean)
// {
//         double bin = 0;
//         for (auto c: llocs.confids) {
//                 if (c > mean)
//                         bin += 1;
//         }
//         if (bin > 0)
//                 bin /= llocs.confids.size();
//         return bin;
// }

////////////////////////////////////////////////////////////////////////////////
static double
calculateLexicality(ocrc::Document::iterator b, ocrc::Document::iterator e)
{
        //double total = 0;
        static const auto SHORT = ocrc::Profiler::Profiler_Token::SHORT;
        size_t n = 0, l = 0;
        for (auto i = b; i != e; ++i) {
                if (i->isNormal() and SHORT < i->getWOCR().size()) {
                        ++n;
                        if (i->hasTopCandidate()) {
                                l += i->candidatesBegin()->getDlev();
                        } else {
                                l += std::min(i->getWOCR().size(), size_t(3));
                        }
                        n += i->getWOCR().size();
                }
        }
        //return total;
        if (n > 0 and l > 0)
                return double(l) / double(n);
        return 0;
}

////////////////////////////////////////////////////////////////////////////////
static int
run2(int argc, char **argv)
{
        assert(argc > 2);
        ocrc::Document document;
        ocrc::LlocsReader reader;
        reader.parse(argc - 2, argv + 2, document);

        //ocrc::Profiler profiler;
        //profiler.readConfiguration(argv[1]);
        //profiler.createProfile(document);

        // gather confidence values
        double lsum = 0, osum = 0, asum = 0;
        size_t nn = 0, dd = 0;
        std::vector<Data> lines;
        const auto e = document.end();
        size_t c = 0;
        for (auto i = document.begin(); i != e; ) {
                Data data;
                data.llocs = reader.getTloc(*i).llocs;

                // find end of current line
                const char *path = data.llocs->path;
                auto b = i;
                while (++i != e and strcmp(reader.getTloc(*i).path, path) == 0)
                         ; // do nothing

                // calculate;
                //data.lex = calculateLexicality(b, i);
                data.conf = calculateConfidence(*data.llocs);
                data.acc = calculateAccuracy(*data.llocs, dd, nn);
                data.i = c++;

                // global
                lsum += data.lex;
                osum += data.conf;

                lines.push_back(std::move(data));
        }
        lsum /= lines.size();
        osum /= lines.size();
        std::wcerr << dd  << " / " << nn << "\n";
        asum = 1.0 - (double(dd) / double(nn));

        // remove nan's (lines with missing ground truth file)
        lines.erase(std::remove_if(begin(lines), end(lines), [](const Data& t) {
                return std::isnan(t.acc);
        }), end(lines));
        // std::sort(begin(lines), end(lines), [](const Data& a, const Data& b) {
        //         return a.acc < b.acc;
        // });

        for (const auto& data: lines) {
                //data.conf = calculateBinaryConfidence(*data.llocs, osum);
                std::wcout << data.i << "|"
                           << data.llocs->path << "|"
                           << data.acc << "|"
                           << data.conf << "|"
                           << data.lex << "|"
                           << data.llocs->chars << " [accuracy:"
                           << data.acc << ",confidence:"
                           << data.conf << ",mean-lex:"
                           << data.lex << "]\n";
        }
        std::wcerr << "total mean lexicality: " << lsum << std::endl;
        std::wcerr << "total mean confidence: " << osum << std::endl;
        std::wcerr << "  total mean accuracy: " << asum << std::endl;
        return EXIT_SUCCESS;
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
                return run2(argc, argv);
        } catch (const std::exception& e) {
                std::wcerr << "[error] " << e.what() << "\n";
        }
        return 1;
}
