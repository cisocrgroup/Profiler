#include <regex>
#include "AutoCorrector.h"
#include "Document/Document.h"
#include "Utils/Utils.h"

using namespace OCRCorrection;

////////////////////////////////////////////////////////////////////////////////
void
AutoCorrector::add_patterns(const std::string& pats)
{
	size_t i = -1;
	do {
		++i;
		const auto pos = pats.find(",", i);
		add_pattern(pats.substr(i, pos -i));
		i = pos;
	} while (i != std::string::npos);
}

////////////////////////////////////////////////////////////////////////////////
void
AutoCorrector::add_pattern(const std::string& pat)
{
	static const std::regex first(R"(first\s*(\d{1,2})%?)");
	static const std::regex pattern(R"(([^:]*):([^:]*):(\d{1,2}))");
	std::smatch m;
	if (std::regex_match(pat, m, first)) {
		percent_ = static_cast<double>(std::stoi(m[1])) / 100.0;
	} else if (std::regex_match(pat, m, pattern)) {
		patterns_.emplace_back(Utils::tolower(Utils::utf8(m[1])),
				Utils::tolower(Utils::utf8(m[2])), std::stoi(m[3]));
	} else {
		throw std::runtime_error("(AutoCorrector) Invalid pattern: " + pat);
	}
}

////////////////////////////////////////////////////////////////////////////////
void
AutoCorrector::correct(Document& doc) const
{
	correct(doc, CorrectPercent());
	correct(doc, CorrectPatterns());
}

////////////////////////////////////////////////////////////////////////////////
void
AutoCorrector::correct(Document& doc, CorrectPercent) const
{
	int n = static_cast<int>(doc.getNrOfTokens() * percent_);
	for (auto& token: doc) {
		if (n <= 0)
			break;
		correct(token);
		--n;
	}
}

////////////////////////////////////////////////////////////////////////////////
void
AutoCorrector::correct(Document& doc, CorrectPatterns) const
{
	std::vector<int> applications(patterns_.size());
	std::transform(begin(patterns_), end(patterns_), begin(applications),
			[](const Pattern& p) {return p.n;});

	for (auto& token: doc) {
		for (size_t i = 0; i < patterns_.size(); ++i) {
			if (patterns_[i].match(token) and applications[i] > 0) {
				correct(token);
				--applications[i];
				// don't break here; use every pattern
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
void
AutoCorrector::correct(Token& token) const
{
	if (not token.has_metadata("groundtruth"))
		throw std::runtime_error("(AutoCorrector) Cannot auto correct "
				"token without any ground truth data");
	token.metadata()["correction"] = token.metadata()["groundtruth"];
	token.metadata()["correction-lc"] = token.metadata()["groundtruth-lc"];
	std::wcout << "Correcting token " << token.getWOCR()
		   << " with "
		   << token.metadata()["correction"] << "\n";
}

////////////////////////////////////////////////////////////////////////////////
bool
AutoCorrector::Pattern::match(const Token& token) const
{
	const auto b = token.getWOCR_lc().begin();
	const auto e = token.getWOCR_lc().end();
	if (ocr.empty())
		return match(begin(gt), end(gt), b, e);
	if (gt.empty())
		return match(begin(ocr), end(ocr), b, e);

	size_t n;
	It ab, ae, bb, be;
	if (gt.size() > ocr.size()) {
		n = gt.size() - ocr.size();
		ab = begin(gt);
		ae = end(gt);
		bb = begin(ocr);
		be = end(ocr);
	} else {
		n = ocr.size() - gt.size();
		ab = begin(ocr);
		ae = end(ocr);
		bb = begin(gt);
		be = end(gt);
	}

	for (auto i = b; i != e; ++i) {
		if (match_here(ab, ae, i, e)) {
			for (auto j = 0U; j <= n and i + j != e; ++j) {
				if (match_here(bb, be, i + j, e))
					return true;
			}
		}
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////
bool
AutoCorrector::Pattern::match(It pb, It pe, It b, It e)
{
	for (; b != e; ++b) {
		if (match_here(pb, pe, b, e))
			return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////
bool
AutoCorrector::Pattern::match_here(It pb, It pe, It b, It e)
{
	for (; pb != pe and b != e; ++pb, ++b) {
		if (*pb != L'.' and *pb != *b)
			return false;
	}
	return true;
}
