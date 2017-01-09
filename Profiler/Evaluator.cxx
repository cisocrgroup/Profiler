#include <iomanip>
#include <cmath>
#include <system_error>
#include "GlobalProfile/GlobalProfile.h"
#include "Evaluator.h"

using namespace OCRCorrection;

////////////////////////////////////////////////////////////////////////////////
double
Evaluator::precision() const noexcept
{
	return (double)true_positives() /
		((double)true_positives() + (double)false_positives());
}

////////////////////////////////////////////////////////////////////////////////
double
Evaluator::recall_objective() const noexcept
{
	return (double)true_positives() /
		((double)true_positives() + (double)false_negatives_objective());
}

////////////////////////////////////////////////////////////////////////////////
double
Evaluator::recall_fair() const noexcept
{
	return (double)true_positives() /
		((double)true_positives() + (double)false_negatives_fair());
}

////////////////////////////////////////////////////////////////////////////////
bool
Evaluator::has_ocr_errors(const Token& token)
{
	using std::begin;
	using std::end;
	CandidateRange r(token);
	if (std::any_of(r.begin(), r.end(), [](const Candidate& c) {return c.isUnknown();}))
		return true; // if token is uniterpretable it is an ocr error;
	if (not r.empty()) {
		return not begin(r)->getOCRTrace().empty(); // empty ocr trace means no errors
	}
	return false; // no candidates means no errors
}

////////////////////////////////////////////////////////////////////////////////
Evaluator::Class
Evaluator::get_false_negative(const Token& token)
{
	if (token.metadata()["touch"] == L"true")
		return Class::FalseNegativeFair;
	else if (token.metadata()["touch"] == L"false")
		return Class::FalseNegativeObjective;
	else
		throw std::runtime_error("Invalid metadata for `value`: " +
				Utils::utf8(token.metadata()["touch"]));
}

////////////////////////////////////////////////////////////////////////////////
bool
Evaluator::is_true_positive(const Token&, ModeNormal)
{
	return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
Evaluator::is_true_positive(const Token& token, ModeStrict)
{

	using std::begin;
	using std::end;
	CandidateRange r(token);
	auto i = std::find_if(begin(r), end(r), [&token](const Candidate& cand) {
		return token.metadata()["groundtruth-lc"] == cand.getWord();
	});
	return i != end(r);
}

////////////////////////////////////////////////////////////////////////////////
bool
Evaluator::is_true_positive(const Token& token, ModeVeryStrict)
{
	CandidateRange r(token);
	return token.metadata()["groundtruth-lc"] == r.begin()->getWord();
}


////////////////////////////////////////////////////////////////////////////////
void
Evaluator::classify(const Document& doc)
{
	for (const auto& token: doc) {
		classify(token);
	}
}

////////////////////////////////////////////////////////////////////////////////
void
Evaluator::classify(const Token& token)
{
	// skip spaces
	if (token.isSpace())
		return;
	if (not token.has_metadata("groundtruth"))
		throw std::runtime_error("cannot evaluate recall and "
				"precision of tokens without any groundtruth");

	const CandidateRange r(token);
	if (std::any_of(r.begin(), r.end(),
				[](const Candidate& c) {return c.isUnknown();})) {
		unknowns_.push_back(token.getIndexInDocument());
	}

	// update counts
	if (token.metadata()["eval"] == L"true")
		++neval_;
	else if (token.metadata()["eval"] == L"false")
		++ntest_;
	if (token.has_metadata("correction"))
		++nx_;
	// do not evaluate tokens in the test set
	if (token.metadata()["eval"] == L"false")
		return;

	const auto idx = token.getIndexInDocument();
	(*this)[get_class(token)].push_back(idx);
}

////////////////////////////////////////////////////////////////////////////////
Evaluator::Class
Evaluator::get_class(const Token& token) const
{
	switch (mode_) {
	case Mode::Normal:
		return get_class(token, ModeNormal());
	case Mode::Strict:
		return get_class(token, ModeStrict());
	case Mode::VeryStrict:
		return get_class(token, ModeVeryStrict());
	default:
		throw std::logic_error("default label in `switch(mode_)` "
				"encountered");
	}
}

////////////////////////////////////////////////////////////////////////////////
void
Evaluator::write(const std::string& dir, const Document& doc) const
{
	auto info = dir + "/info.txt";
	auto tp = dir + "/true_positives.txt";
	auto tn = dir + "/true_negatives.txt";
	auto fp = dir + "/false_positives.txt";
	auto fnf = dir + "/false_negatives_fair.txt";
	auto fno = dir + "/false_negatives_objective.txt";
	auto uk = dir + "/unknowns.txt";
	auto corrs = dir + "/corrections.txt";

	if (mkdir(dir.data(), 0752) != 0 and errno != EEXIST)
		throw std::system_error(errno, std::system_category(), dir);

	std::wofstream os;
	os.open(tp);
	if (not os.good())
		throw std::system_error(errno, std::system_category(), tp);
	os << "# " << Utils::utf8(tp) << "\n";
	write(os, Class::TruePositive, doc);
	os.close();

	os.open(tn);
	if (not os.good())
		throw std::system_error(errno, std::system_category(), tn);
	os << "# " << Utils::utf8(tn) << "\n";
	write(os, Class::TrueNegative, doc);
	os.close();

	os.open(fp);
	if (not os.good())
		throw std::system_error(errno, std::system_category(), fp);
	os << "# " << Utils::utf8(fp) << "\n";
	write(os, Class::FalsePositive, doc);
	os.close();

	os.open(fnf);
	if (not os.good())
		throw std::system_error(errno, std::system_category(), fnf);
	os << "# " << Utils::utf8(fnf) << "\n";
	write(os, Class::FalseNegativeFair, doc);
	os.close();

	os.open(fno);
	if (not os.good())
		throw std::system_error(errno, std::system_category(), fno);
	os << "# " << Utils::utf8(fno) << "\n";
	write(os, Class::FalseNegativeObjective, doc);
	os.close();

	size_t normal = 0;
	size_t corrections = 0;

	if (not unknowns_.empty()) {
		os.open(uk);
		if (not os.good())
			throw std::system_error(errno, std::system_category(), uk);
		os << "# " << Utils::utf8(uk) << "\n";
		for (auto id: unknowns_) {
			const auto& token = doc.at(id);
			os << token.getWOCR();
			CandidateRange r(token);
			for (const auto& cand: r) {
				os << "," << cand;
			}
			os << "\n";
		}
	}
	os.close();

	os.open(corrs);
	if (not os.good())
		throw std::system_error(errno, std::system_category(), corrs);
	os << "# " << Utils::utf8(corrs) << "\n";
	for (const auto& token: doc) {
		++normal;
		if (token.has_metadata("correction")) {
			++corrections;
			os << token.metadata()["correction"] << ":"
			   << token.getWOCR();
			if (token.candidatesBegin() != token.candidatesEnd()) {
				os << "," << *token.candidatesBegin();
			}
			if (token.has_metadata("auto-correction"))
				os << token.metadata()["auto-correction"];
			os << "\n";
		}
	}
	os.close();

	os.open(info);
	if (not os.good())
		throw std::system_error(errno, std::system_category(), info);
	os << "# " << Utils::utf8(info) << "\n"
	   << "True positives:              " << true_positives() << "\n"
	   << "True negatives:              " << true_negatives() << "\n"
	   << "False positives:             " << false_positives() << "\n"
	   << "False negatives (fair):      " << false_negatives_fair() << "\n"
	   << "False negatives (objective): " << false_negatives_objective() << "\n"
	   << "Precision:                   " << std::setprecision(4) << precision() << "\n"
	   << "Recall (fair):               " << std::setprecision(4) << recall_fair() << "\n"
	   << "Recall (objective):          " << std::setprecision(4) << recall_objective() << "\n"
	   << "Evaluated tokens:            " << neval_ << "\n"
	   << "Not evaluated tokens:        " << ntest_ << "\n"
	   << "Corrected tokens:            " << nx_ << "\n";
	os.close();

	if (doc.has_global_profile()) {
		auto gp = dir + "/global_profile.txt";
		os.open(gp);
		if (not os.good())
			throw std::system_error(errno, std::system_category(), gp);
		os << "# " << Utils::utf8(gp) << "\n";
		write(os, doc.global_profile());
		os.close();
	}
}

////////////////////////////////////////////////////////////////////////////////
void
Evaluator::write(std::wostream& os, Class c, const Document& doc) const
{
	struct CandRange {
		CandRange(const Token& token): token_(token) {}
		Token::CandidateIterator begin() const {
			return token_.candidatesBegin();
		}
		Token::CandidateIterator end() const {
			return token_.candidatesEnd();
		}
		const Token& token_;
	};

	for (const size_t id: classes_[static_cast<size_t>(c)]) {
		if (doc.at(id).has_metadata("source-file"))
			os << "file: " << doc.at(id).metadata()["source-file"] << "\n";
		os << "gt:   " << doc.at(id).metadata()["groundtruth"] << "\n";
		os << "ocr:  " << doc.at(id).getWOCR() << "\n";
		for (const auto& cand: CandRange(doc.at(id))) {
			os << "cand: " << cand << "\n";
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
void
Evaluator::write(std::wostream& os, const GlobalProfile& gp) const
{
	using Pair = std::pair<csl::Pattern, double>;
	std::vector<Pair> hist, ocr;
	gp.histPatternProbabilities_.sortToVector(&hist);
	std::sort(begin(ocr), end(ocr), [&gp](const Pair& a, const Pair& b) {
		return gp.histPatternProbabilities_.getWeight(a.first) >
			gp.histPatternProbabilities_.getWeight(b.first);
	});
	for (const auto& p: hist) {
		os << "hist:" << p.first.getLeft() << ":"
		   << p.first.getRight() << ":"
		   << gp.histPatternProbabilities_.getWeight(p.first)
		   << "\n";
	}
	gp.ocrPatternProbabilities_.sortToVector(&ocr);
	std::sort(begin(ocr), end(ocr), [&gp](const Pair& a, const Pair& b) {
		return gp.ocrPatternProbabilities_.getWeight(a.first) >
			gp.ocrPatternProbabilities_.getWeight(b.first);
	});
	for (const auto& p: ocr) {
		os << "ocr:" << p.first.getLeft() << ":"
		   << p.first.getRight() << ":"
		   << gp.ocrPatternProbabilities_.getWeight(p.first)
		   << "\n";
	}
}
