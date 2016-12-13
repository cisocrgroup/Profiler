#include <cmath>
#include "GlobalProfile/GlobalProfile.h"
#include "RecPrec.h"

using namespace OCRCorrection;

////////////////////////////////////////////////////////////////////////////////
double
RecPrec::precision() const noexcept
{
	return (double)true_positives() /
		((double)true_positives() + (double)false_positives());
}

////////////////////////////////////////////////////////////////////////////////
double
RecPrec::recall() const noexcept
{
	return (double)true_positives() /
		((double)true_positives() + (double)false_negatives());
}

////////////////////////////////////////////////////////////////////////////////
double
RecPrec::accuracy() const noexcept
{
	return ((double)true_positives() + (double) true_negatives()) /
		(double)sum();
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
	if (not r.empty()) {
		return not begin(r)->getOCRTrace().empty(); // empty ocr trace means no errors
	}
	return false; // no candidates means no errors
}

////////////////////////////////////////////////////////////////////////////////
bool
RecPrec::is_true_positive(const Token&, ModeNormal)
{
	return true;
}

////////////////////////////////////////////////////////////////////////////////
bool
RecPrec::is_true_positive(const Token& token, ModeStrict)
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
RecPrec::is_true_positive(const Token& token, ModeVeryStrict)
{
	CandidateRange r(token);
	return token.metadata()["groundtruth-lc"] == r.begin()->getWord();
}


////////////////////////////////////////////////////////////////////////////////
void
OCRCorrection::RecPrec::classify(const Document& doc)
{
	for (const auto& token: doc) {
		// each normal token must have a groundtruth attached to it
		if (token.isNormal() and not token.has_metadata("groundtruth"))
			throw std::runtime_error("cannot evaluate recall and "
					"precision of tokens without groundtruth");

		// handle normal tokens without any corrections
		if (not token.has_metadata("correction") and token.isNormal() and
				token.getWOCR().size() > 3 /* long */) {
			const auto idx = token.getIndexInDocument();
			(*this)[classify(token)].push_back(idx);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
OCRCorrection::RecPrec::Class
OCRCorrection::RecPrec::classify(const Token& token) const
{
	switch (mode_) {
	case Mode::Normal:
		return classify(token, ModeNormal());
	case Mode::Strict:
		return classify(token, ModeStrict());
	case Mode::VeryStrict:
		return classify(token, ModeVeryStrict());
	default:
		throw std::logic_error("default label in `switch(mode_)` "
				"encountered");
	}
}

////////////////////////////////////////////////////////////////////////////////
void
OCRCorrection::RecPrec::write(const std::string& dir, const Document& doc) const
{
	auto info = dir + "/info.txt";
	auto tp = dir + "/true_positive.txt";
	auto tn = dir + "/true_negative.txt";
	auto fp = dir + "/false_positive.txt";
	auto fn = dir + "/false_negative.txt";
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

	os.open(fn);
	if (not os.good())
		throw std::system_error(errno, std::system_category(), fn);
	os << "# " << Utils::utf8(fn) << "\n";
	write(os, Class::FalseNegative, doc);
	os.close();

	size_t normal = 0;
	size_t corrections = 0;
	os.open(corrs);
	if (not os.good())
		throw std::system_error(errno, std::system_category(), fn);
	os << "# " << Utils::utf8(corrs) << "\n";
	for (const auto& token: doc) {
		++normal;
		if (token.has_metadata("correction")) {
			++corrections;
			os << token.getWOCR() << ":"
			   << token.metadata()["correction"];
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
	   << "True positive:    " << true_positives() << "\n"
	   << "True negative:    " << true_negatives() << "\n"
	   << "False positive:   " << false_positives() << "\n"
	   << "False negative:   " << false_negatives() << "\n"
	   << "Precision:        " << precision() << "\n"
	   << "Recall:           " << recall() << "\n"
	   << "Normal tokens:    " << normal << "\n"
	   << "Corrected tokens: " << corrections << "\n";
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
RecPrec::write(std::wostream& os, Class c, const Document& doc) const
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
		if (doc.at(id).has_metadata("file"))
			os << "file: " << doc.at(id).metadata()["file"] << "\n";
		os << "gt:   " << doc.at(id).metadata()["groundtruth"] << "\n";
		os << "ocr:  " << doc.at(id).getWOCR() << "\n";
		for (const auto& cand: CandRange(doc.at(id))) {
			os << "cand: " << cand << "\n";
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
void
RecPrec::write(std::wostream& os, const GlobalProfile& gp) const
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
