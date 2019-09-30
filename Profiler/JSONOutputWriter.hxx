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
  static void write(std::wostream &out, const Document &doc);
  static void writeNormalToken(std::wostream &out, std::wstring pre,
                               const Token &token, size_t n);
  static void writeCandidate(std::wostream &out, std::wstring pre,
                             const Candidate &candidate);
  static void writeInstruction(std::wostream &out, std::wstring pre,
                               const csl::PosPattern &instr);
  const Document &doc_;
  std::wostream &out_;
};
} // namespace OCRCorrection
#endif // OCRCorrection_JSONOutputWriter_hxx__
