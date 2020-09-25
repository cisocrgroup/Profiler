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
class Profiler;
class JSONOutputWriter {
public:
  JSONOutputWriter(std::wostream &out, const Document &doc, const Profiler& profiler)
    : doc_(doc), profiler_(profiler), out_(out) {}
  void write() const;

private:
  static void write(std::wostream &out, const Document &doc,
		    const Profiler& profiler);
  static void writeNormalToken(std::wostream &out, const Profiler& profiler,
			       std::wstring pre, const Token &token, size_t n);
  static void writeCandidate(std::wostream &out, const Profiler& profiler,
			     std::wstring pre, const Candidate &candidate);
  static void writeHistInstruction(std::wostream &out, const Profiler& profiler,
				   std::wstring pre, const csl::PosPattern &instr);
  static void writeOCRInstruction(std::wostream &out, const Profiler& profiler,
				  std::wstring pre, const csl::PosPattern &instr);
  const Document &doc_;
  const Profiler &profiler_;
  std::wostream &out_;
};
} // namespace OCRCorrection
#endif // OCRCorrection_JSONOutputWriter_hxx__
