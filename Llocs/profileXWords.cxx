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
#include "Llocs/Llocs.hpp"
#include "WagnerFischer.hpp"

namespace ocrc = OCRCorrection;
#ifdef FOO
struct Data {
        Data(): llocs(), nlex(0), n(0) {}
        ocrc::LlocsReader::LlocsPtr llocs;
	size_t nlex, n;
        //double lex, acc, conf;
        //size_t i;
};
using DataVec = std::vector<Data>;

////////////////////////////////////////////////////////////////////////////////
static std::pair<size_t, size_t>
calculateLexicality(ocrc::Document::iterator b, ocrc::Document::iterator e)
{
        static const auto SHORT = ocrc::Profiler::Profiler_Token::SHORT;
	std::pair<size_t, size_t> res{0, 0};
	std::for_each(b, e, [&res](const ocrc::Token& t) {
	    const size_t len = t.getWOCR().size();
	    if (t.isNormal() and SHORT < len) {
	      if (t.hasTopCandidate())
		res.first += t.candidatesBegin()->getDlev();
	      else
		res.first += std::min(len, 3UL);
	      res.second += len;
	    }
	  });
	return res;
}

////////////////////////////////////////////////////////////////////////////////
static int
run2(int argc, char **argv)
{
        assert(argc > 2);
        ocrc::Document document;
        ocrc::LlocsReader reader;
        reader.parse(argv + 2, document);

	// profiler
        ocrc::Profiler profiler;
        profiler.readConfiguration(argv[1]);
        profiler.createProfile(document);
	auto counts = calculateLexicality(document.begin(), document.end());
	const double mlex = double(counts.first) / double(counts.second);
        std::wcerr << "lsum=" << counts.first << ", n=" << counts.second << "\n";
	std::wcout << "mean-lex " << mlex << "\n";

	double errorsum = 0, nchars=0;
	for (const auto& llocs: reader.llocs()) {
	  for (const auto& triple: *llocs) {
	    ++nchars;
	    errorsum += triple.conf;
	  }
	}
	std::wcout << "total-error " << errorsum << "\n";
	std::wcout << "total-chars " << static_cast<size_t>(nchars) << "\n";
	std::wcout << "mean-error " << (errorsum/nchars) << "\n";
        return EXIT_SUCCESS;
}
#endif // FOO
////////////////////////////////////////////////////////////////////////////////
int
main(int argc, char **argv)
{

        std::locale::global(std::locale(""));

        if (argc != 2) {
                std::wcerr << "Usage: " << argv[0] << " llocs\n";
                return 1;
        }
        ocrc::Document document;
        ocrc::LlocsReader reader;
        reader.parse(argv + 1, document);
	double nchars = 0;
	double errsum = 0;
	for (const auto& llocs: reader.llocs()) {
		for (const auto& triple: *llocs) {
			++nchars;
			errsum += triple.conf;
		}
	}
	std::wcout << (errsum/nchars) << "\n";
	return 0;

#ifdef FOO
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
#endif // FOO
}
