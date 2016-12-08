#include <cmath>
#include "RecPrec.h"

using namespace OCRCorrection;

////////////////////////////////////////////////////////////////////////////////
double
RecPrec::precision() const noexcept
{
	return (double)true_positives() / ((double)true_positives() + (double)false_positives());
}

////////////////////////////////////////////////////////////////////////////////
double
RecPrec::recall() const noexcept
{
	return (double)true_positives() / ((double)true_positives() + (double)false_negatives());
}

////////////////////////////////////////////////////////////////////////////////
double
RecPrec::accuracy() const noexcept
{
	return ((double)true_positives() + (double) true_negatives()) / (double)sum();
}

////////////////////////////////////////////////////////////////////////////////
double
RecPrec::f_measure(double beta) const noexcept
{
	const auto beta_square = std::pow(beta, 2);
	const auto p = precision();
	const auto r = recall();
	return (p * r) / ((beta_square * p) + r);
}

////////////////////////////////////////////////////////////////////////////////
bool
RecPrec::has_ocr_errors(const Token& token)
{
	using std::begin;
	using std::end;
	CandidateRange r(token);
	return std::any_of(begin(r), end(r), [](const Candidate& cand) {
		return not cand.getOCRTrace().empty();
	});
}
////////////////////////////////////////////////////////////////////////////////
bool
RecPrec::is_true_positive(const Token& token, const std::wstring& gt, ModeNormal)
{
	return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
RecPrec::is_true_positive(const Token& token, const std::wstring& gt, ModeStrict)
{

	using std::begin;
	using std::end;
	CandidateRange r(token);
	auto i = std::find_if(begin(r), end(r), [&gt](const Candidate& cand) {
		return gt == cand.getWord();
	});
	return i != end(r);
}

////////////////////////////////////////////////////////////////////////////////
bool
RecPrec::is_true_positive(const Token& token, const std::wstring& gt, ModeVeryStrict)
{
	CandidateRange r(token);
	return gt == r.begin()->getWord();
}
