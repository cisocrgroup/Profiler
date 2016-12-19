#ifndef OCRCorrection_AutoCorrector_h__
#define OCRCorrection_AutoCorrector_h__

#include <vector>
#include <string>
#include <unordered_set>

namespace OCRCorrection {
	class GtDoc;
	class GtLine;
	class GtToken;

	class AutoCorrector {
	public:
		void add_patterns(const std::string& pat);
		void add_pattern(const std::string& pat);
		void operator()(GtDoc& doc) {correct(doc);}
		void correct(GtDoc& doc);

	private:
		using Candidates = std::unordered_set<std::wstring>;
		using Suggestions = std::map<int, Candidates>;
		struct CorrectPercent {};
		struct CorrectPatterns {};
		struct CorrectSuggestionsRanked {};
		struct CorrectSuggestionsEach {};
		void correct(GtDoc& doc, CorrectPercent) const;
		void correct(GtDoc& doc, CorrectSuggestionsEach) const;
		void correct(GtDoc& doc, CorrectSuggestionsRanked) const;
		bool correct(GtLine& line, GtToken t, const Candidates& cands) const;
		static Suggestions read_suggestions(const std::string& str);
		int calculate_testset_size(const GtDoc& doc) const;
		int calculate_corrections_size(const GtDoc& doc) const;
		int calculate_number_of_tokens(const GtDoc& doc) const;

		Suggestions suggestions_;
		int testset_, first_, nx_, n_;
		bool ranked_, each_;
	};
}

#endif // OCRCorrection_AutoCorrector_h__
