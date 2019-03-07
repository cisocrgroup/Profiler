#ifndef OCRCorrection_ExtReader_h__
#define OCRCorrection_ExtReader_h__
namespace csl {
  class AdditionalLex;
}
namespace OCRCorrection {
  class Document;
  class ExtReader {
	ExtReader() = default;
	void parse(char const*path, Document& document, csl::AdditionalLex& alex);
  };
}

#endif // OCRCorrection_ExtReader_h__
