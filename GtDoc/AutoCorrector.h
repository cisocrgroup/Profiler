#ifndef OCRCorrection_AutoCorrector_h__
#define OCRCorrection_AutoCorrector_h__

#include <vector>
#include <string>
#include <unordered_set>

namespace OCRCorrection {
	class Document;
	class Token;

	class AutoCorrector {
	public:
		void add_patterns(const std::string& pat);
		void add_pattern(const std::string& pat);
		void operator()(Document& doc) {correct(doc);}
		void correct(Document& doc);

	private:
		using Candidates = std::unordered_set<std::wstring>;
		using Suggestions = std::map<int, Candidates>;
		struct CorrectPercent {};
		struct CorrectPatterns {};
		struct CorrectSuggestionsRanked {};
		struct CorrectSuggestionsEach {};
		void mark_tokens(Document& doc) const;
		void correct(Document& doc, CorrectPercent) const;
		void correct(Document& doc, CorrectSuggestionsEach) const;
		void correct(Document& doc, CorrectSuggestionsRanked) const;
		void correct(Token& token) const;
		static Suggestions read_suggestions(const std::string& str);
		int calculate_testset_size(const Document& doc) const;
		int calculate_corrections_size(const Document& doc) const;
		static int calculate_number_of_tokens(const Document& doc);

		Suggestions suggestions_;
		int testset_, first_, nx_, n_;
		bool ranked_, each_;
	};
}

#endif // OCRCorrection_AutoCorrector_h__
