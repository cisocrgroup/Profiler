#include <regex>
#include "AutoCorrector.h"
#include "Document/Document.h"
#include "GtDoc.h"
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
	static const std::regex trainset(R"(train\s*set\s*0*(\d{1,2})%?)");
	static const std::regex pattern(R"(([^:]*):([^:]*):(\d{1,2}))");
	std::smatch m;
	if (std::regex_match(pat, m, first)) {
		first_ = std::stoi(m[1]);
	} else if (std::regex_match(pat, m, trainset)) {
		trainset_ = std::stoi(m[1]);
	} else if (std::regex_match(pat, m, pattern)) {
		patterns_.emplace_back(Utils::tolower(Utils::utf8(m[1])),
				Utils::tolower(Utils::utf8(m[2])), std::stoi(m[3]));
	} else {
		throw std::runtime_error("(AutoCorrector) Invalid pattern: " + pat);
	}
}

////////////////////////////////////////////////////////////////////////////////
void
AutoCorrector::correct(GtDoc& doc) const
{
	correct(doc, CorrectPercent());
	correct(doc, CorrectPatterns());
}

////////////////////////////////////////////////////////////////////////////////
void
AutoCorrector::correct(GtDoc& doc, CorrectPercent) const
{
	// X% of tokens are in X% of lines
	const int firstn = (doc.lines().size() * first_) / 100;
	const int trainsetn = (doc.lines().size() * trainset_) / 100;

	for (auto i = 0; i < trainsetn; ++i) {
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
	const int n = (doc.lines().size() * trainset_) / 100;

	for (const auto& p: patterns_) {
		// auto n = p.n; // ignore n
		// apply correction only to lines in the trainset
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

		// found the pattern
		auto b = std::find_if(std::reverse_iterator<GtLine::iterator>(i),
				std::reverse_iterator<GtLine::iterator>(lb),
				[](const GtChar& c) {
			return Document::isSpace(c.gt);
		});
		auto ee = std::find_if(i, le, [](const GtChar& c) {
				return Document::isSpace(c.gt);
		});
		std::for_each(b.base(), ee, [](GtChar& c) {c.correct();});
	}
}

