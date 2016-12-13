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
	auto p = std::to_wstring(static_cast<int>(percent_ * 100)) + L"%";
	for (auto& token: doc) {
		if (n <= 0)
			break;
		correct(token);
		token.metadata()["auto_correction"] += L",first " + p;
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
		size_t i = 0;
		for (const auto& p: patterns_) {
			if (p.match(token) and applications[i] > 0) {
				correct(token);
				--applications[i];
				token.metadata()["auto_correction"] +=
					L"," + p.gt + L":" + p.ocr;
			}
			++i;
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
	if (not token.has_metadata("groundtruth-lc"))
		throw std::runtime_error("(AutoCorrector) Cannot auto correct "
				"token without any ground truth data: " +
				Utils::utf8(token.getWOCR_lc()));
	if (ocr.size() != gt.size())
		throw std::runtime_error("(AutoCorrection) Invalid pattern: " +
				Utils::utf8(gt) + ":" + Utils::utf8(ocr));
	if (ocr.size() > token.getWOCR_lc().size())
		return false;
	if (gt.size() > token.metadata()["groundtruth-lc"].size())
		return false;

	auto n = std::min(token.getWOCR_lc().size(),
			token.metadata()["groundtruth-lc"].size());
	auto d = n - gt.size();
	auto ob = token.getWOCR_lc().begin();
	auto gb = token.metadata()["groundtruth-lc"].begin();

	for (auto i = ob, j = gb; i != (ob + d) and j != (gb + d); ++i, ++j) {
		if (match(begin(ocr), end(ocr), i, ob + d) and
				match(begin(gt), end(gt), j, gb + d))
			return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////
bool
AutoCorrector::Pattern::match(It pb, It pe, It b, It e)
{
	for (; pb != pe and b != e; ++pb, ++b) {
		if (*pb != L'.' and *pb != *b)
			return false;
	}
	return true;
}
