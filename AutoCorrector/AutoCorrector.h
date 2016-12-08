#ifndef OCRCorrection_AutoCorrector_h__
#define OCRCorrection_AutoCorrector_h__

#include <vector>
#include <string>

namespace OCRCorrection {
	class Document;

	class AutoCorrector {
	public:
		void add_patterns(const std::string& pat);
		void add_pattern(const std::string& pat);
		void operator()(Document& doc) const {correct(doc);}
		void correct(Document& doc) const;

	private:
		std::vector<std::wstring> patterns_;
	};
}

#endif // OCRCorrection_AutoCorrector_h__
