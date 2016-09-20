#include <iostream>
#include <cassert>
#include <memory>
#include <vector>
#include <set>
#include <map>
#include <random>
#include <algorithm>
#include <regex>
#include "WagnerFischer.hpp"

#include "Llocs.hpp"
#include "util.hpp"

namespace ocrc = OCRCorrection;
using LlocsPtr = std::shared_ptr<ocrc::Llocs>;
using LlocsVec = std::vector<LlocsPtr>;
using LlocsSet = std::set<LlocsPtr>;
using CharMap = std::map<wchar_t, LlocsVec>;

////////////////////////////////////////////////////////////////////////////////
static LlocsVec
readLlocs(int argc, char **argv)
{
        LlocsVec llocs;
        for (int i = 0; i < argc; ++i) {
		auto ll = std::make_shared<ocrc::Llocs>(argv[i]);
		if (ll->hasGt())
			llocs.push_back(ll);
        }
        return llocs;
}

////////////////////////////////////////////////////////////////////////////////
static void
bestselect(LlocsVec& llocs)
{
        const size_t min = std::strtol(ocrc::get("min"), NULL, 0);
        std::sort(begin(llocs), end(llocs),
                  [](const LlocsPtr& a, const LlocsPtr& b) {
                          return a->conf() > b->conf();
                  });
	llocs.resize(std::min(min, llocs.size()));
}

////////////////////////////////////////////////////////////////////////////////
static void
worstselect(LlocsVec& llocs)
{
	using std::begin;
	using std::end;
        const size_t min = std::strtol(ocrc::get("min"), NULL, 0);
	auto accf = [](double acc, const ocrc::Llocs::Triple& t) {
		return acc + t.conf;
	};
	for (auto& ll: llocs) {
		const auto sum = std::accumulate(begin(*ll), end(*ll), 0.0, accf);
		ll->setVal(double(ll->size()) - sum);
	}
        std::sort(begin(llocs), end(llocs),
                  [](const LlocsPtr& a, const LlocsPtr& b) {
                          return a->val() > b->val();
                  });
	llocs.resize(std::min(min, llocs.size()));
}

////////////////////////////////////////////////////////////////////////////////
static void
randselect(LlocsVec& llocs)
{
	std::random_device rd;
	std::mt19937 g(rd());
        const size_t min = std::strtol(ocrc::get("min"), NULL, 0);

        std::shuffle(begin(llocs), end(llocs), g);
	llocs.resize(std::min(min, llocs.size()));
}

////////////////////////////////////////////////////////////////////////////////
static void
combselect(LlocsVec& llocs)
{
	using std::begin;
	using std::end;
	std::random_device rd;
	std::mt19937 g(rd());
        const size_t min = std::min(std::strtoul(ocrc::get("min"), NULL, 0),
				    (unsigned long int)llocs.size());
	const size_t rand = min/2;
	const size_t worst = min - rand;
	assert((rand + worst) == min);

        std::shuffle(begin(llocs), end(llocs), g);
	LlocsVec selection;
	std::copy_n(begin(llocs), rand, std::back_inserter(selection));
	LlocsSet set(begin(selection), end(selection));
	auto accf = [](double acc, const ocrc::Llocs::Triple& t) {
		return acc + t.conf;
	};
	for (auto& ll: llocs) {
		const auto sum = std::accumulate(begin(*ll), end(*ll), 0.0, accf);
		ll->setVal(double(ll->size()) - sum);
	}
	std::sort(begin(llocs), end(llocs), [](const LlocsPtr& a, const LlocsPtr& b) {
			return a->val() > b->val();
		});
	for (size_t i = 0; i < llocs.size() and selection.size() != min; ++i) {
		if (not set.count(llocs[i])) {
			selection.push_back(llocs[i]);
			set.insert(llocs[i]);
		}
	}
	std::swap(selection, llocs);
}

////////////////////////////////////////////////////////////////////////////////
static int
select(int argc, char **argv)
{
	const auto sel = ocrc::get("sel");
        const auto odir = ocrc::get("odir");
	const auto autocorrect = std::strtol(ocrc::get("ac", "1"), NULL, 0);
        auto llocs = readLlocs(argc, argv);

	if (strcmp(sel, "random") == 0)
		randselect(llocs);
	else if (strcmp(sel, "worst") == 0)
		worstselect(llocs);
	else if (strcmp(sel, "best") == 0)
		bestselect(llocs);
	else if (strcmp(sel, "comb") == 0)
		combselect(llocs);
	else if (strcmp(sel, "all") == 0)
             ; // do nothing
	else
		throw std::runtime_error("invalid selection");

        std::wcerr << "[info] selected the following "
                   << llocs.size() << " examples:\n";
        size_t i = 0;
        std::wstring line;
        for (const auto& ll: llocs) {
             if (autocorrect) {
                  std::wcerr << "[info] correcting ["
                             << ll->pred() << "]\n";
                  std::wcerr << "             with ["
                             << ll->gt() << "]\n";
                  ll->copy(odir, ++i, true);
             } else {
                  std::wcerr << "[info] correcting ["
                             << ll->pred() << "]\n";
                  std::wcerr << "             with ["
                             << ll->pred() << "]\n";
                  ll->copy(odir, ++i, false);
             }
        }
        return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
int
error(int argc, char **argv)
{
        const auto llocs = readLlocs(argc, argv);
	lev::WagnerFischer wf;
	for (const auto& ll: llocs) {
		if (not ll->hasGt())
			continue;
		const auto realerror = wf(ll->gt(), ll->pred());
		const auto error = ll->error();
		std::wcout << realerror << " " << error << "\n";
		if (realerror > 40) {
			wf.backtrack();
			std::wcerr << "          gt: " << wf.truth() << "\n";
			std::wcerr << "       trace: " << wf.trace() << "\n";
			std::wcerr << "        pred: " << wf.pred() << "\n";
			std::wcerr << "error (real): " << realerror << "\n";
			std::wcerr << "error (pred): " << error << "\n";
			std::wcerr << "        path: " << ll->path() << "\n";
		}
	}
	return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
int
lorenz(int argc, char **argv)
{
        const auto llocs = readLlocs(argc, argv);
	using P = std::pair<LlocsPtr, double>;
	std::vector<P> counts;
	counts.reserve(llocs.size());
        double cn = 0;
        for (const auto& ll: llocs) {
	  counts.emplace_back(ll, ll->error());
	  cn += ll->pred().size();
        }
	std::sort(begin(counts), end(counts), [](const P& a, const P& b) {
			return a.second < b.second;
		});
	const double n = counts.size();
	double sum = 0;
	double i = 0;
	std::wcout << "# Number-of-chars: " << cn << "\n";
	for (const auto& p: counts) {
		assert(p.first->hasGt());
		sum += p.second;
		++i;
		std::wcout << i/n << " " << sum/cn << " " << sum << "\n";
	}
	return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
static int
mconf(int argc, char **argv)
{
        const auto llocs = readLlocs(argc, argv);
	double cn = 0, sum = 0;
	for (const auto& ll: llocs) {
		for (const auto& t: *ll) {
			++cn;
			sum += t.conf;
		}
	}
	std::wcout << "mean-conf " << (sum / cn) << "\n";
	return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
static int
mmconf(int argc, char **argv)
{
        const auto llocs = readLlocs(argc, argv);
	double cn = 0, sum = 0;
	for (const auto& ll: llocs) {
		for (const auto& t: *ll) {
			if (t.conf > 0.95) {
				sum += t.conf;
			}
			++cn;
		}
	}
	std::wcout << "mean-conf " << (sum / cn) << "\n";
	return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
int
main(int argc, char **argv)
{
        try {
                if (argc < 2)
                        throw std::runtime_error(
                                std::string("Usage: ") + argv[0] + " llocs...");
		std::locale::global(std::locale(""));
		auto command = ocrc::get("command");
		if (strcmp(command, "error") == 0)
			return error(argc - 1, argv + 1);
		else if (strcmp(command, "lorenz") == 0)
			return lorenz(argc - 1, argv + 1);
		else if (strcmp(command, "select") == 0)
			return select(argc - 1, argv + 1);
		else if (strcmp(command, "mconf") == 0)
			return mconf(argc - 1, argv + 1);
		else if (strcmp(command, "mmconf") == 0)
			return mmconf(argc - 1, argv + 1);
		else
			throw std::runtime_error("Invalid command "
						 "(error|select|lorenz)");
	} catch (std::exception& e) {
                std::wcerr << "[error] " << e.what() << "\n";
        }
        return EXIT_FAILURE;
}
