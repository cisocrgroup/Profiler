#ifndef OCRCorrection_AutoCorrector_h__
#define OCRCorrection_AutoCorrector_h__

#include <vector>
#include <string>
#include <unordered_set>

namespace OCRCorrection {
	class GtDoc;
	class GtLine;

	class AutoCorrector {
	public:
		void add_patterns(const std::string& pat);
		void add_pattern(const std::string& pat);
		void operator()(GtDoc& doc) const {correct(doc);}
		void correct(GtDoc& doc) const;

	private:
		using Candidates = std::unordered_set<std::wstring>;
		using Suggestions = std::map<int, Candidates>;
		struct CorrectPercent {};
		struct CorrectPatterns {};
		struct CorrectSuggestions {};
		void correct(GtDoc& doc, CorrectPercent) const;
		void correct(GtDoc& doc, CorrectPatterns) const;
		void correct(GtDoc& doc, CorrectSuggestions) const;
		void correct(GtLine& line, const Candidates& cands) const;
		static Suggestions read_suggestions(const std::string& str);
		struct Pattern {
			using It = std::wstring::const_iterator;
			Pattern(std::wstring g, std::wstring o, int p)
				: gt(std::move(g))
				, ocr(std::move(o))
				, n(p)
			{}
			void correct(GtLine& line) const;

			std::wstring gt, ocr;
			double n;
		};
		std::vector<Pattern> patterns_;
		Suggestions suggestions_;
		int testset_, first_;
	};
}

#endif // OCRCorrection_AutoCorrector_h__
