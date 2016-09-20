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
#include "util.hpp"
#include "WagnerFischer.hpp"

////////////////////////////////////////////////////////////////////////////////
namespace ocrc = OCRCorrection;
using Tlocs = ocrc::LlocsReader::Tlocs;
struct Data {
	const char *ini;
	const char *odir;
	char **llocs;
	double threshold;
};

////////////////////////////////////////////////////////////////////////////////
static Data
InizializeData(int argc, char **argv)
{
	if (argc < 3)
		throw std::runtime_error(std::string("Usage: ") + argv[0] + "<ini> llocs...");
	Data data;
	data.ini = argv[1];
	data.llocs = argv + 2;
	data.threshold = std::strtod(ocrc::get("threshold"), nullptr);
	data.odir = ocrc::get("odir");
	return data;
}

////////////////////////////////////////////////////////////////////////////////
static double
CalculatePotential(const Tlocs& tlocs)
{
	if (tlocs.token->hasTopCandidate()) {
		auto c = tlocs.token->candidatesBegin();
		if (c->getDlev() > 0) {
			const auto& t = c->getOCRTrace();
			double sum = 0;
			double n = 0;
			for (const auto& pp: t) {
				const auto p = pp.getPosition();
				for (auto i = 0U; i < pp.getRight().size(); ++i) {
					++n;
					sum += (1 - std::next(tlocs.b, (p + i))->conf);
				}
			}
			sum /= n;
			double res = sum * c->getVoteWeight();
			return std::isnan(res) ? 0 : res;
		}
	}
	return -1;
}

////////////////////////////////////////////////////////////////////////////////
static int
Run(int argc, char **argv)
{
	auto data = InizializeData(argc, argv);
	ocrc::LlocsReader reader;
	ocrc::Document document;
	ocrc::Profiler profiler;

	reader.parse(data.llocs, document);
	profiler.readConfiguration(data.ini);
	profiler.createProfile(document);

	// select fishy tokens
	using P = std::pair<Tlocs, double>;
	std::vector<P> fishy;
	fishy.reserve(reader.tlocs().size());
	for (const auto& tlocs: reader.tlocs()) {
		fishy.emplace_back(tlocs, CalculatePotential(tlocs));
	}
	fishy.erase(std::remove_if(begin(fishy), end(fishy), [&data](const P& p) {
	      return false; //return p.second < data.threshold; 
	}), end(fishy));

	std::wcerr << "size: " << fishy.size() << "\n";
	for (auto& p: fishy) {
		p.second = CalculatePotential(p.first);
	}
	std::wcerr << "size: " << fishy.size() << "\n";
	std::sort(begin(fishy), end(fishy), [](const P& a, const P& b) {
		return a.second > b.second;
	});

	std::wcerr << "size: " << fishy.size() << "\n";
	lev::WagnerFischer wf;
	std::set<ocrc::LlocsReader::LlocsPtr> lines;
	std::for_each(begin(fishy), end(fishy), [&lines,&wf](const P& p) {
	    if (p.first.token->hasTopCandidate()) {
	      assert(p.first.token->hasTopCandidate());
		auto c = p.first.token->candidatesBegin();
		wf(c->getString(), p.first.string());
		wf.backtrack();
		std::wcout << "   TRUTH: " << wf.truth() << "\n";
		std::wcout << "   TRACE: " << wf.trace() << "\n";
		std::wcout << "    PRED: " << wf.pred() << "\n";

		ocrc::Llocs::iterator pred = p.first.b;
		auto truth = begin(c->getString());
		for (const auto& t: wf.trace()) {
			switch (t) {
			case lev::WagnerFischer::EditOp::S:
				pred->c = *truth;
				++pred;
				++truth;
				break;
			case lev::WagnerFischer::EditOp::D:
				p.first.llocs->content().insert(pred, {*truth, 0, 0});
				++truth;
				break;
			case lev::WagnerFischer::EditOp::I:
				pred = p.first.llocs->content().erase(pred);
				break;
			default:
				++truth;
				++pred;
				break;
			}
		}
		p.first.llocs->invalidate();
		std::wcout << "PRED LINE: " << p.first.llocs->pred() << "\n";
 		std::wcout << "  GT LINE: " << p.first.llocs->gt() << "\n";

	    }
	    lines.insert(p.first.llocs);
	});
	int i = 0;
	for (auto& line: lines) {
		auto end = line->end();
		for (auto i = line->begin(); i != end;) {
			auto next = std::next(i);
			if (iswpunct(i->c) and next != end and not iswspace(next->c))
				line->content().insert(next, {L' ', 0, 0});
			i = next;
		}
		line->invalidate();
		std::wcout << "PRED: " << line->pred() << "\n";
		std::wcout << "  GT: " << line->gt() << "\n";
		line->copy(data.odir, ++i, false);
	}
	return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
int
main(int argc, char **argv)
{
	try {
		std::locale::global(std::locale(""));
		return Run(argc, argv);
	} catch (const std::exception& e) {
		std::wcerr << "[error] " << e.what() << "\n";
	}
	return EXIT_FAILURE;
}
