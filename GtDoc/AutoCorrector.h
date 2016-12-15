#ifndef OCRCorrection_AutoCorrector_h__
#define OCRCorrection_AutoCorrector_h__

#include <vector>
#include <string>

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
		struct CorrectPercent {};
		struct CorrectPatterns {};
		void correct(GtDoc& doc, CorrectPercent) const;
		void correct(GtDoc& doc, CorrectPatterns) const;
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
		int trainset_, first_;
	};
}

#endif // OCRCorrection_AutoCorrector_h__
