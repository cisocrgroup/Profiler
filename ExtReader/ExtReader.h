#ifndef OCRCorrection_ExtReader_h__
#define OCRCorrection_ExtReader_h__

#include <string>

namespace csl {
  class AdditionalLex;
}
namespace OCRCorrection {
  class Document;
  class ExtReader {
  public:
	ExtReader() = default;
	// Parse one token per line.  Lines that start with '#' are
	// assumed to be additional lexicon entries.  Empty lines are
	// skipped.  All other lines represent OCR tokens.  OCR tokens are
	// pairs of strings seperated by ':'.  The first item is the OCR
	// token, the second item is its correction.  OCR tokens without
	// any corrections, still must contain a last ':'.
	void parse(const std::string& path, Document& document, csl::AdditionalLex& alex);
  };
}

#endif // OCRCorrection_ExtReader_h__
