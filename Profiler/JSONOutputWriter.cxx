#include "JSONOutputWriter.hxx"
#include "Document/Document.h"
#include <unordered_set>

using namespace OCRCorrection;

static std::wostream&
writeString(std::wostream& out, const std::wstring& str)
{
  out << '"' << str << '"';
  return out;
}

static std::wostream&
writeKeyVal(std::wostream& out, const std::wstring& key, double val)
{
  writeString(out, key);
  out << ": " << val;
  return out;
}

static std::wostream&
writeKeyVal(std::wostream& out, const std::wstring& key, size_t val)
{
  writeString(out, key);
  out << ": " << val;
  return out;
}

static std::wostream&
writeKeyVal(std::wostream& out,
            const std::wstring& key,
            const std::wstring& val)
{
  writeString(out, key);
  out << ": ";
  writeString(out, val);
  return out;
}

void
JSONOutputWriter::write() const
{
  std::unordered_set<std::wstring> types;
  out_ << "{";
  wchar_t pre = L'\n';
  for (const auto& token : doc_) {
    if (token.isNormal() && !types.count(token.getWOCR())) {
      types.insert(token.getWOCR());
      writeNormalToken(pre, token);
    }
    pre = L',';
  }
  out_ << "}\n";
}

void
JSONOutputWriter::writeNormalToken(wchar_t pre, const Token& token) const
{
  out_ << pre;
  writeString(out_, token.getWOCR()) << ": {\n";
  writeKeyVal(out_, L"OCR", token.getWOCR()) << ",\n";
  pre = L'\n';
  writeString(out_, L"Candidates") << ": [";
  for (auto i = token.candidatesBegin(); i != token.candidatesEnd(); ++i) {
    writeCandidate(pre, *i);
    pre = L',';
  }
  out_ << "]\n}";
}

void
JSONOutputWriter::writeCandidate(wchar_t pre, const Candidate& candidate) const
{
  out_ << pre << "{\n";
  writeKeyVal(out_, L"Suggestion", candidate.getString()) << ",\n";
  writeKeyVal(out_, L"Modern", candidate.getString()) << ",\n";
  writeKeyVal(out_, L"Dict", candidate.getDictModule().getName()) << ",\n";
  writeString(out_, L"HistPatterns") << ": [";
  pre = L'\n';
  for (const auto& histp : candidate.getHistInstruction()) {
    writeInstruction(pre, histp);
    pre = L',';
  }
  out_ << "],\n";
  pre = L'\n';
  writeString(out_, L"OCRPatterns") << ": [";
  for (const auto& ocrp : candidate.getOCRTrace()) {
    writeInstruction(pre, ocrp);
    pre = L',';
  }
  out_ << "],\n";
  writeKeyVal(out_, L"Distance", candidate.getDlev()) << ",\n";
  writeKeyVal(out_, L"Weight", candidate.getVoteWeight()) << "\n";
  out_ << "}";
}

void
JSONOutputWriter::writeInstruction(wchar_t pre,
                                   const csl::PosPattern& instr) const
{
  out_ << pre << "{";
  writeKeyVal(out_, L"Left", instr.getLeft()) << ",";
  writeKeyVal(out_, L"Right", instr.getRight()) << ",";
  writeKeyVal(out_, L"Pos", instr.getPosition()) << "}\n";
}
