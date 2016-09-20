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
#include "util.hpp"

namespace ocrc = OCRCorrection;
struct Data {
        Data(): llocs(), lex(0), acc(0), conf(0), i(0) {}
        ocrc::LlocsReader::LlocsPtr llocs;
        double lex, acc, conf;
        size_t i;
};
using DataVec = std::vector<Data>;

////////////////////////////////////////////////////////////////////////////////
static double
calculateAccuracy(const ocrc::LlocsReader::Llocs& llocs)
{
        const static std::regex re{"\\.llocs$"};
        static lev::WagnerFischer wf;

        // gt
        const auto gtfile = std::regex_replace(llocs.path, re, ".gt.txt");
        std::wifstream is(gtfile);
        if (not is.good()) {
                std::wcerr << "[warning] could not open: "
                           << gtfile.data() << "\n";
                return 1;
        }
        std::wstring gt;
        std::getline(is, gt);
        is.close();

        const auto d = wf(gt, llocs.chars);
        double error = 0;
        const size_t denom = std::max(gt.size(), llocs.chars.size());
        if (denom)
                error = double(d) / double(denom);
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

////////////////////////////////////////////////////////////////////////////////
static double
calculatePercentage(const std::vector<Data>& lines, double bucket, double step)
{
        size_t n = std::count_if(begin(lines), end(lines), [bucket, step](const Data& data) {
                return (bucket <= data.conf) and (data.conf < (bucket + step));
        });
        return double(n) / double(lines.size());
}

////////////////////////////////////////////////////////////////////////////////
static int
run2(int argc, char **argv)
{
        assert(argc > 1);
        ocrc::LlocsReader reader;
        ocrc::Document document;
        reader.parse(argc - 1, argv + 1, document);

        // gather confidence values
        double csum = 0, asum = 0;
        DataVec lines;
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
                data.conf = calculateConfidence(*data.llocs);
                data.acc = calculateAccuracy(*data.llocs);
                data.i = c++;

                // global
                asum += data.acc;
                csum += data.conf;
                lines.push_back(std::move(data));
        }
        csum /= lines.size();
        asum /= lines.size();

        // remove nan's (lines with missing ground truth file)
        lines.erase(std::remove_if(begin(lines), end(lines), [](const Data& t) {
                return std::isnan(t.acc) or std::isnan(t.conf);
        }), end(lines));

        // sort by confidence
        std::sort(begin(lines), end(lines), [](const Data& a, const Data& b) {
                //return a.acc < b.acc;
                return a.conf < b.conf;
        });

        std::wcerr << "mean confidence: " << csum << std::endl;
        std::wcerr << "  mean accuracy: " << asum << std::endl;

        const double step = std::strtod(ocrc::get("step"), 0);
        for (double c = 0; c <= (1.0 + (step/2)); c += step) {
                //std::wcerr << "c: " << c << " (step: " << step << ")\n";
                std::wcout << c << "|" << calculatePercentage(lines, c, step) << "\n";
        }
        return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
int
main(int argc, char **argv)
{
        try {
                if (argc < 2)
                        throw std::runtime_error(std::string("Usage: ") +
                                                 argv[0] + " llocs [llocs...]");
                std::locale::global(std::locale(""));
                return run2(argc, argv);
        } catch (const std::exception& e) {
                std::wcerr << "[error] " << e.what() << "\n";
        }
        return EXIT_FAILURE;
}
