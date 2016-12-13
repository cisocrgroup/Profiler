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
	static const std::regex pattern(R"(([^:]*):([^:]*):(\d{1,2})%?)");
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
}

////////////////////////////////////////////////////////////////////////////////
void
AutoCorrector::correct(Token& token) const
{
	token.metadata()["correction"] = token.metadata()["groundtruth"];
	token.metadata()["correction-lc"] = token.metadata()["groundtruth-lc"];
	std::wcout << "Correcting token " << token.getWOCR()
		   << " with "
		   << token.metadata()["correction"] << "\n";
}
