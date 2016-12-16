#include <regex>
#include "AutoCorrector.h"
#include "Document/Document.h"
#include "GtDoc.h"
#include "Utils/Utils.h"

using namespace OCRCorrection;

////////////////////////////////////////////////////////////////////////////////
static bool eligible(GtLine::range r) noexcept;

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
	static const std::regex pattern(R"(([^:]*):([^:]*):(\d{1,2}))");
	static const std::regex ranked(R"(ranked\s*(.*))");
	static const std::regex each(R"(each\s*(.*))");
	std::smatch m;
	if (std::regex_match(pat, m, first)) {
		first_ = std::stoi(m[1]);
	} else if (std::regex_match(pat, m, testset)) {
		testset_ = std::stoi(m[1]);
	} else if (std::regex_match(pat, m, pattern)) {
		patterns_.emplace_back(Utils::tolower(Utils::utf8(m[1])),
				Utils::tolower(Utils::utf8(m[2])), std::stoi(m[3]));
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
AutoCorrector::correct(GtDoc& doc) const
{
	correct(doc, CorrectPercent());
	correct(doc, CorrectPatterns());
	if (each_)
		correct(doc, CorrectSuggestionsEach());
	else if (ranked_)
		correct(doc, CorrectSuggestionsRanked());
}

////////////////////////////////////////////////////////////////////////////////
void
AutoCorrector::correct(GtDoc& doc, CorrectPercent) const
{
	// X% of tokens are in X% of lines
	const int firstn = first_size(doc);
	const int testsetn = testset_size(doc);

	for (auto i = 0; i < testsetn; ++i) {
		auto b = doc.lines()[i].begin();
		auto e = doc.lines()[i].end();
		std::for_each(b, e, [](GtChar& c) {c.eval = false;});
	}
	for (auto i = 0; i < firstn; ++i) {
		auto b = doc.lines()[i].begin();
		auto e = doc.lines()[i].end();
		std::for_each(b, e, [](GtChar& c) {c.correct();});
	}
}

////////////////////////////////////////////////////////////////////////////////
void
AutoCorrector::correct(GtDoc& doc, CorrectPatterns) const
{
	const int n = testset_size(doc);

	for (const auto& p: patterns_) {
		// auto n = p.n; // ignore n
		// apply correction only to lines in the testset
		for (auto i = 0; i < n; ++i) {
			auto& line = doc.lines().at(i);
			p.correct(line);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
void
AutoCorrector::Pattern::correct(GtLine& line) const
{
	const auto lb = line.begin();
	const auto le = line.end();
	for (auto i = lb; i != le; ++i) {
		auto p = gt.begin();
		auto e = gt.end();
		auto j = i;
		while (p != e and j != le) {
			if (*p != L'.' and *p != j->gt)
				break;
		}
		if (p != e)
			continue;
		p = ocr.begin();
		e = ocr.end();
		j = i;
		while (p != e and j != le) {
			if (*p != L'.' and *p != j->ocr)
				break;
		}
		if (p != e)
			continue;

		line.correct(GtLine::range(i, j));
	}
}

////////////////////////////////////////////////////////////////////////////////
void
AutoCorrector::correct(GtDoc& doc, CorrectSuggestionsRanked) const
{
	const int n = testset_size(doc);
	int ntokens = tokens_size(doc);

	for (const auto& s: suggestions_) {
		if (ntokens <= 0)
			break;
		for (auto i = 0; i < n and ntokens > 0; ++i) {
			correct(doc.lines().at(i), ntokens, s.second);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
void
AutoCorrector::correct(GtDoc& doc, CorrectSuggestionsEach) const
{
	const int n = testset_size(doc);
	int ntokens = tokens_size(doc);

	for (auto i = 0; i < n and ntokens > 0; ++i) {
		for (const auto& s: suggestions_) {
			if (ntokens <= 0)
				break;
			correct(doc.lines().at(i), ntokens, s.second);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
void
AutoCorrector::correct(GtLine& line, int& n, const Candidates& cands) const
{
	static std::wstring buf;
	static const auto g = [](const GtChar& c) {return c.ocr;};
	line.each_token(g, [&](GtLine::range r) {
		if (n <= 0)
			return;
		if (not eligible(r))
			return;
		buf.clear();
		std::transform(r.first, r.second, std::back_inserter(buf),
				[](const GtChar& c) {return towlower(c.ocr);});
		if (cands.count(buf)) {
			line.correct(r);
			--n;
		}
	});
}

////////////////////////////////////////////////////////////////////////////////
AutoCorrector::Suggestions
AutoCorrector::read_suggestions(const std::string& path)
{
	static const std::regex ocrerrb(R"(\s*<ocr_error>\s*)");
	static const std::regex ocrerre(R"(\s*</ocr_error>\s*)");
	static const std::regex pat(
			R"xx(\s*<pattern.*absFrequency="(\d+.*?)".*)xx");
	static const std::regex type(R"xx(\s*<type.*wOCR_lc="(.*?)".*)xx");

	std::ifstream is(path);
	if (is.good())
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
			current_freq = std::stoi(m[1]);
		} else if (std::regex_match(line, m, type)) {
			suggestions[current_freq].insert(Utils::utf8(m[1]));
		}
	}
	return suggestions;
}

////////////////////////////////////////////////////////////////////////////////
int
AutoCorrector::testset_size(const GtDoc& doc) const
{
	return (doc.lines().size() * testset_) / 100;
}

////////////////////////////////////////////////////////////////////////////////
int
AutoCorrector::first_size(const GtDoc& doc) const
{
	return (doc.lines().size() * first_) / 100;
}

////////////////////////////////////////////////////////////////////////////////
int
AutoCorrector::tokens_size(const GtDoc& doc) const
{
	const auto n = testset_size(doc);
	int ntokens = 0;
	for (auto i = 0; i < n; ++i) {
		doc.lines().at(i).each_token([&](GtLine::range r) {
			if (eligible(r))
				++ntokens;
		});
	}
	return (ntokens * first_) / 100;
}

////////////////////////////////////////////////////////////////////////////////
bool
eligible(GtLine::range r) noexcept
{
	static const auto normal = [](const GtChar& c) {return c.is_normal();};
	return std::distance(r.first, r.second) > 3 and
				std::all_of(r.first, r.second, normal);
}
