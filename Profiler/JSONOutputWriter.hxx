#ifndef OCRCorrection_JSONOutputWriter_hxx__
#define OCRCorrection_JSONOutputWriter_hxx__

#include <iostream>

namespace csl {
class PosPattern;
}

namespace OCRCorrection {
class Document;
class Token;
class Candidate;
class JSONOutputWriter {
public:
  JSONOutputWriter(std::wostream &out, const Document &doc)
      : doc_(doc), out_(out) {}
  void write() const;

private:
  void writeNormalToken(wchar_t pre, const Token &token) const;
  void writeCandidate(wchar_t pre, const Candidate &candidate) const;
  void writeInstruction(wchar_t pre, const csl::PosPattern &instr) const;
  const Document &doc_;
  std::wostream &out_;
};
} // namespace OCRCorrection
#endif // OCRCorrection_JSONOutputWriter_hxx__
