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
