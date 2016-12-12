#ifndef OCRCorrection_AutoCorrector_h__
#define OCRCorrection_AutoCorrector_h__

#include <vector>
#include <string>

namespace OCRCorrection {
	class Document;
	class Token;

	class AutoCorrector {
	public:
		void add_patterns(const std::string& pat);
		void add_pattern(const std::string& pat);
		void operator()(Document& doc) const {correct(doc);}
		void correct(Document& doc) const;

	private:
		void correct(Token& token) const;
		struct Pattern {
			Pattern(std::wstring p, bool o): pattern(std::move(p)), ocr(o) {}
			std::wstring pattern;
			bool ocr;
		};
		std::vector<Pattern> patterns_;
	};
}

#endif // OCRCorrection_AutoCorrector_h__
