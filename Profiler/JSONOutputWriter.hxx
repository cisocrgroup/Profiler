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
class JSONOutputWriter
{
public:
  JSONOutputWriter(std::wostream& out, const Document& doc)
    : doc_(doc)
    , out_(out)
  {}
  void write() const;

private:
  void writeNormalToken(const Token& token) const;
  void writeCandidate(const Candidate& candidate) const;
  void writeInstruction(const csl::PosPattern& instr) const;
  const Document& doc_;
  std::wostream& out_;
};
}
#endif // OCRCorrection_JSONOutputWriter_hxx__
