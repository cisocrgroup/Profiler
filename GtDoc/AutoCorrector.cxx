#include <regex>
#include "AutoCorrector.h"
#include "Document/Document.h"
#include "GtDoc.h"
#include "Utils/Utils.h"

using namespace OCRCorrection;
static const auto G = [](const GtChar& c) {return c.ocr;};

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
AutoCorrector::correct(GtDoc& doc)
{
	n_ = calculate_testset_size(doc);
	nx_ = calculate_corrections_size(doc);

	int c = 0;
	for (auto& line: doc.lines()) {
		line.each_token(G, [&](GtLine::range r) {
			if (c < n_) {
				line.set_eval(r, false);
				if (r.eligible())
					c++;
			}
		});
	}
	if (not each_ and not ranked_)
		correct(doc, CorrectPercent());
	if (each_)
		correct(doc, CorrectSuggestionsEach());
	else if (ranked_)
		correct(doc, CorrectSuggestionsRanked());
}

////////////////////////////////////////////////////////////////////////////////
void
AutoCorrector::correct(GtDoc& doc, CorrectPercent) const
{
	int c = 0;
	for (auto& line: doc.lines()) {
		line.each_token(G, [&](GtLine::range r) {
			if (c < nx_) {
				if (r.eligible()) {
					c++;
					line.correct(r);
				}
			}
		});
	}
}

////////////////////////////////////////////////////////////////////////////////
void
AutoCorrector::correct(GtDoc& doc, CorrectSuggestionsRanked) const
{
	int c = 0;
	for (const auto& s: suggestions_) {
		if (c >= nx_)
			break;
		for (auto& line: doc.lines()) {
			if (c >= nx_)
				break;
			line.each_token(G, [&](GtLine::range r) {
				if (c <= nx_ and r.eligible()) {
					correct(line, r, s.second);
					c++;
				}
			});
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
void
AutoCorrector::correct(GtDoc& doc, CorrectSuggestionsEach) const
{
	int c = 0;
	for (auto& line: doc.lines()) {
		if (c >= nx_)
			break;
		for (const auto& s: suggestions_) {
			if (c >= nx_)
				break;
			line.each_token(G, [&](GtLine::range r) {
				if (c <= nx_ and r.eligible()) {
					correct(line, r, s.second);
					c++;
				}
			});
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
bool
AutoCorrector::correct(GtLine& line, GtToken t, const Candidates& cands) const
{
	static std::wstring buf;
	buf.clear();
	std::transform(t.b, t.e, std::back_inserter(buf), [](const GtChar& c) {
		return tolower(c.ocr);
	});
	if (cands.count(buf)) {
		line.correct(t);
		return true;
	} else {
		return false;
	}
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
	for (const auto& p: suggestions) {
		for (const auto& str: p.second) {
			std::wcerr << p.first << ": "  << str << "\n";
		}
	}
	return suggestions;
}

////////////////////////////////////////////////////////////////////////////////
int
AutoCorrector::calculate_testset_size(const GtDoc& doc) const
{
	return (calculate_number_of_tokens(doc) * testset_) / 100;
}

////////////////////////////////////////////////////////////////////////////////
int
AutoCorrector::calculate_corrections_size(const GtDoc& doc) const
{
	return (calculate_number_of_tokens(doc) * first_) / 100;
}

////////////////////////////////////////////////////////////////////////////////
int
AutoCorrector::calculate_number_of_tokens(const GtDoc& doc) const
{
	int n = 0;
	for (const auto& line: doc.lines()) {
		line.each_token([&](GtLine::range r) {
			if (r.eligible())
				++n;
		});
	}
	return n;
}

