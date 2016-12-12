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
	if (pat.find("gt:") == 0) {
		patterns_.emplace_back(Utils::tolower(Utils::utf8(pat.substr(3))), false);
	} else if (pat.find("ocr:") == 0) {
		patterns_.emplace_back(Utils::tolower(Utils::utf8(pat.substr(4))), true);
	} else {
		patterns_.emplace_back(Utils::tolower(Utils::utf8(pat)), true);
	}
}

////////////////////////////////////////////////////////////////////////////////
void
AutoCorrector::correct(Document& doc) const
{
	for (auto& token: doc) {
		if (not token.isNormal()) // skip not normal token
			continue;
		if (not token.has_metadata("groundtruth"))
			throw std::runtime_error("Cannot autocorrect "
					"without groundtruth");
		for (const auto& p: patterns_) {
			if (p.ocr and token.getWOCR_lc().
					find(p.pattern) != std::wstring::npos) {
				correct(token);
				break; // correct each token once
			} else if (not p.ocr and token.metadata()["groundtruth-lc"].
					find(p.pattern) != std::wstring::npos) {
				correct(token);
				break; // correct each token once
			}
		}
	}
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
