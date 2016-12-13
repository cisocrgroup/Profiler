#include "Utils/IStr.h"
#include "AdaptiveLex.h"

using namespace OCRCorrection;

////////////////////////////////////////////////////////////////////////////////
const std::wstring AdaptiveLex::NAME = L"adaptive_lex";
std::map<std::tuple<std::wstring, std::wstring>, size_t> AdaptiveLex::CACHE;
std::unordered_map<std::wstring, size_t> AdaptiveLex::LEX;
std::vector<size_t> AdaptiveLex::COSTS;

////////////////////////////////////////////////////////////////////////////////
AdaptiveLex::AdaptiveLex(size_t rank, size_t max_lev)
	: DictModule(rank)
	, max_lev_(max_lev)
{
}

////////////////////////////////////////////////////////////////////////////////
void
AdaptiveLex::add(const std::wstring& gt, const std::wstring& ocr,
		Receiver& receiver)
{
	auto i = LEX.find(gt);
	if (i == end(LEX)) {
		// std::wcerr << "(AdaptiveLex) adding "
		// 	      << gt << ":" << ocr << "\n";
		i = LEX.emplace_hint(i, gt, lev(gt, ocr));
	}
	add_candidate(gt, i->second, receiver);
}

////////////////////////////////////////////////////////////////////////////////
bool
AdaptiveLex::doquery(const std::wstring& q, Receiver& receiver)
{
	bool res = false;
	for (const auto& gt: LEX) {
		auto i = CACHE.find(std::tie(gt.first, q));
		if (i == end(CACHE))
			i = CACHE.emplace_hint(i, std::tie(gt.first, q), lev(gt.first, q));
		assert(i != end(CACHE));
		if (i->second <= max_lev_) {
			// std::wcerr << "(AdaptiveLex) doquery lev("
			// 	<< gt.first << "," << q << ") = " << d << "\n";
			add_candidate(gt.first, i->second, receiver);
			res = true;
		}
	}
	return res;
}

////////////////////////////////////////////////////////////////////////////////
void
AdaptiveLex::add_candidate(const std::wstring& gt, size_t lev,
		Receiver& receiver)
{
	csl::Interpretation i;
	i.setWord(gt.data());
	i.setBaseWord(gt.data());
	i.setBaseWordScore(0);
	i.setLevDistance(lev);
	// std::wcerr << "(AdaptiveLex) Interpretation: " << i << "\n";
	static_cast<csl::iInterpretationReceiver&>(receiver).receive(i);
}

////////////////////////////////////////////////////////////////////////////////
size_t
AdaptiveLex::lev(const std::wstring& a, const std::wstring& b)
{
	const size_t m(a.size());
	const size_t n(b.size());

	if (m == 0) return n;
	if (n == 0) return m;
	COSTS.resize(n + 1);

	for (auto k = 0U; k <= n; ++k)
		COSTS[k] = k;

	size_t i = 0;
	for (auto ia = begin(a); ia != end(a); ++ia, ++i) {
		COSTS[0] = i + 1;
		size_t corner = i;

		size_t j = 0;
		for (auto ib = begin(b); ib != end(b); ++ib, ++j) {
			size_t upper = COSTS[j + 1];
			if (towupper(*ia) == towupper(*ib)) { // ignore case
				COSTS[j + 1] = corner;
			} else {
				size_t t(upper < corner ? upper : corner);
				COSTS[j + 1] = (COSTS[j] < t ? COSTS[j] : t) + 1;
			}
			corner = upper;
		}
	}
	return COSTS[n];
}
