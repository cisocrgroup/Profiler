#include "Utils/IStr.h"
#include "AdaptiveHistGtLex.h"

using namespace OCRCorrection;

////////////////////////////////////////////////////////////////////////////////
const std::wstring AdaptiveHistGtLex::NAME_ = L"adaptive_hist_gt";

////////////////////////////////////////////////////////////////////////////////
AdaptiveHistGtLex::AdaptiveHistGtLex(size_t rank, size_t max_lev)
	: DictModule(rank)
	, costs_()
	, max_lev_(max_lev)
{
}

////////////////////////////////////////////////////////////////////////////////
void
AdaptiveHistGtLex::add(const std::wstring& gt, const std::wstring& ocr,
		Receiver& receiver)
{
	auto i = gt_.find(gt);
	if (i == end(gt_)) {
		i = gt_.emplace_hint(i, gt, lev(gt, ocr));
	}
	add_candidate(gt, i->second, receiver);
}

////////////////////////////////////////////////////////////////////////////////
bool
AdaptiveHistGtLex::doquery(const std::wstring& q, Receiver& receiver)
{
	bool res = false;
	for (const auto& gt: gt_) {
		const auto d = lev(gt.first, q);
		if (d <= max_lev_) {
			// std::wcerr << "(AdaptiveHistGtLex) doquery lev("
			// 	<< gt.first << "," << q << ") = " << d << "\n";
			add_candidate(gt.first, d, receiver);
			res = true;
		}
	}
	return res;
}

////////////////////////////////////////////////////////////////////////////////
void
AdaptiveHistGtLex::add_candidate(const std::wstring& gt, size_t lev,
		Receiver& receiver)
{
	csl::Interpretation i;
	i.setWord(gt.data());
	i.setBaseWord(gt.data());
	i.setBaseWordScore(0);
	i.setLevDistance(lev);
	// std::wcerr << "(AdaptiveHistGtLex) Interpretation: " << i << "\n";
	static_cast<csl::iInterpretationReceiver&>(receiver).receive(i);
}

////////////////////////////////////////////////////////////////////////////////
size_t
AdaptiveHistGtLex::lev(const std::wstring& a, const std::wstring& b)
{
	const size_t m(a.size());
	const size_t n(b.size());

	if (m == 0) return n;
	if (n == 0) return m;
	costs_.resize(n + 1);

	for (auto k = 0U; k <= n; ++k)
		costs_[k] = k;

	size_t i = 0;
	for (auto ia = begin(a); ia != end(a); ++ia, ++i) {
		costs_[0] = i + 1;
		size_t corner = i;

		size_t j = 0;
		for (auto ib = begin(b); ib != end(b); ++ib, ++j) {
			size_t upper = costs_[j + 1];
			if (towupper(*ia) == towupper(*ib)) { // ignore case
				costs_[j + 1] = corner;
			} else {
				size_t t(upper < corner ? upper : corner);
				costs_[j + 1] = (costs_[j] < t ? costs_[j] : t) + 1;
			}
			corner = upper;
		}
	}
	return costs_[n];
}
