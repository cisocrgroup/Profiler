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
	static const std::regex first(R"(first\s*0*(\d{1,2})%?)");
	static const std::regex testset(R"(test\s*set\s*0*(\d{1,2})%?)");
	static const std::regex ranked(R"(ranked\s*(.*))");
	static const std::regex each(R"(each\s*(.*))");
	std::smatch m;
	if (std::regex_match(pat, m, first)) {
		first_ = std::stoi(m[1]);
	} else if (std::regex_match(pat, m, testset)) {
		testset_ = std::stoi(m[1]);
	} else if (std::regex_match(pat, m, ranked)) {
		suggestions_ = read_suggestions(m[1]);
		ranked_ = true;
	} else if (std::regex_match(pat, m, each)) {
		suggestions_ = read_suggestions(m[1]);
		each_ = true;
	} else {
		throw std::runtime_error(
				"(AutoCorrector) Invalid pattern: " + pat);
	}
}

////////////////////////////////////////////////////////////////////////////////
void
AutoCorrector::correct(Document& doc)
{
	n_ = calculate_testset_size(doc);
	nx_ = calculate_corrections_size(doc);

	mark_tokens(doc);
	if (not each_ and not ranked_)
		correct(doc, CorrectPercent());
	if (each_)
		correct(doc, CorrectSuggestionsEach());
	else if (ranked_)
		correct(doc, CorrectSuggestionsRanked());
}

////////////////////////////////////////////////////////////////////////////////
void
AutoCorrector::mark_tokens(Document& doc) const
{
	int c = 0;
	for (auto& token: doc) {
		if (token.isSpace()) {
			token.metadata()["eval"] = L"false";
		} else {
			// do evaluate tokens in the evaluation set
			if (c < n_) {
				token.metadata()["eval"] = L"false";
				c++;
			} else {
				token.metadata()["eval"] = L"true";
			}
			// short and/or not normal tokens are not touched by the profiler
			if (token.isNormal() or token.getWOCR_lc().size() > 3) {
				token.metadata()["touch"] = L"true";
			} else {
				token.metadata()["touch"] = L"false";
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
void
AutoCorrector::correct(Document& doc, CorrectPercent) const
{
	int c = 0;
	for (auto& token: doc) {
		// skip space
		if (token.isSpace())
			continue;
		// correct up to nx tokens
		if (c >= nx_)
			break;

		// correct all tokens in the test set
		c++;
		correct(token);
	}
}

////////////////////////////////////////////////////////////////////////////////
void
AutoCorrector::correct(Document& doc, CorrectSuggestionsRanked) const
{
	int c = 0;
	for (const auto& s: suggestions_) {
		for (auto& token: doc) {
			if (token.isSpace())
				continue;
			if (c >= nx_)
				break;

			c++;
			if (s.second.count(token.getWOCR_lc())) {
				correct(token);
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
void
AutoCorrector::correct(Document& doc, CorrectSuggestionsEach) const
{
	int c = 0;
	for (auto& token: doc) {
		if (token.isSpace())
			continue;
		if (c >= nx_)
			break;

		c++;
		for (const auto& s: suggestions_) {
			if (s.second.count(token.getWOCR_lc())) {
				correct(token);
				break; // just correct each token once
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
}

////////////////////////////////////////////////////////////////////////////////
AutoCorrector::Suggestions
AutoCorrector::read_suggestions(const std::string& path)
{
	static const std::regex ocrerrb(R"(\s*<ocr_errors>\s*)");
	static const std::regex ocrerre(R"(\s*</ocr_errors>\s*)");
	static const std::regex pat(
			R"xx(\s*<pattern.*absFrequency="(.*?)".*)xx");
	static const std::regex type(R"xx(\s*<type.*wOCR_lc="(.*?)".*)xx");

	std::ifstream is(path);
	if (not is.good())
		throw std::system_error(errno, std::system_category(), path);

	Suggestions suggestions;
	bool in_ocr_errors = false;
	int current_freq = 0;
	// this is bad. VERY BAD.
	std::string line;
	while (std::getline(is, line)) {
		if (std::regex_match(line, ocrerrb))
			in_ocr_errors = true;
		else if (std::regex_match(line, ocrerre))
			in_ocr_errors = false;
		if (not in_ocr_errors)
			continue;

		std::smatch m;
		if (std::regex_match(line, m, pat)) {
			current_freq = static_cast<int>(std::stod(m[1]));
		} else if (std::regex_match(line, m, type)) {
			suggestions[current_freq].insert(Utils::utf8(m[1]));
		}
	}
	// for (const auto& p: suggestions) {
	// 	for (const auto& str: p.second) {
	// 		std::wcerr << p.first << ": "  << str << "\n";
	// 	}
	// }
	return suggestions;
}

////////////////////////////////////////////////////////////////////////////////
int
AutoCorrector::calculate_testset_size(const Document& doc) const
{
	return (calculate_number_of_tokens(doc) * testset_) / 100;
}

////////////////////////////////////////////////////////////////////////////////
int
AutoCorrector::calculate_corrections_size(const Document& doc) const
{
	return (calculate_number_of_tokens(doc) * first_) / 100;
}

////////////////////////////////////////////////////////////////////////////////
int
AutoCorrector::calculate_number_of_tokens(const Document& doc)
{
	int n = 0;
	for (const auto& token: doc) {
		if (not token.isSpace())
			++n;
	}
	return n;
}

