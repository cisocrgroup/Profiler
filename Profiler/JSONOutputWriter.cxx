#include "JSONOutputWriter.hxx"
#include "Document/Document.h"
#include <iomanip>
#include <iterator>
#include <string>
#include <unordered_map>

using namespace OCRCorrection;

////////////////////////////////////////////////////////////////////////////////
static std::wostream &escapeString(std::wostream &out,
                                   const std::wstring &str) {
  std::ios_base::fmtflags f(out.flags());
  for (auto c : str) {
    if (c == '"' or c == '\\' or ('\x00' <= c and c <= '\x1f')) {
      out << "\\u" << std::hex << std::setw(4) << std::setfill(L'0') << int(c);
    } else {
      out << c;
    }
  }
  out.flags(f);
  return out;
}

////////////////////////////////////////////////////////////////////////////////
static std::wostream &writeString(std::wostream &out, const std::wstring &str,
                                  bool tolower = false) {
  out << '"';
  if (not tolower) {
    escapeString(out, str);
  } else {
    auto cpy = str;
    std::transform(str.begin(), str.end(), cpy.begin(), ::towlower);
    escapeString(out, cpy);
  }
  out << '"';
  return out;
}

////////////////////////////////////////////////////////////////////////////////
static std::wostream &writeKeyVal(std::wostream &out, const std::wstring &key,
                                  double val) {
  writeString(out, key);
  auto old = out.imbue(std::locale("C"));
  out << ": " << val;
  out.imbue(old);
  return out;
}

////////////////////////////////////////////////////////////////////////////////
static std::wostream &writeKeyVal(std::wostream &out, const std::wstring &key,
                                  bool val) {
  writeString(out, key);
  if (val) {
    out << ": true";
  } else {
    out << ": false";
  }
  return out;
}
////////////////////////////////////////////////////////////////////////////////
static std::wostream &writeKeyVal(std::wostream &out, const std::wstring &key,
                                  size_t val) {
  writeString(out, key);
  auto old = out.imbue(std::locale("C"));
  out << ": " << val;
  out.imbue(old);
  return out;
}

////////////////////////////////////////////////////////////////////////////////
static std::wostream &writeKeyVal(std::wostream &out, const std::wstring &key,
                                  const std::wstring &val,
                                  bool tolower = false) {
  writeString(out, key);
  out << ": ";
  writeString(out, val, tolower);
  return out;
}

////////////////////////////////////////////////////////////////////////////////
void JSONOutputWriter::write() const {
  // std::wofstream tmpout("/tmp/profiler.json");
  // write(tmpout, doc_);
  write(out_, doc_);
}

////////////////////////////////////////////////////////////////////////////////
void JSONOutputWriter::write(std::wostream &out, const Document &doc) {
  std::unordered_map<std::wstring, std::pair<size_t, const Token *>> types;
  for (auto i = doc.begin(); i != doc.end(); ++i) {
    // skip short or not normal tokens
    if (i->isShort() or not i->isNormal()) {
      continue;
    }
    auto wocr = i->getWOCR();
    std::transform(wocr.begin(), wocr.end(), wocr.begin(), ::towlower);

    types[wocr].first++;
    types[wocr].second = i.operator->();
  }
  out << "{";
  wchar_t pre = L'\n';
  for (const auto &t : types) {
    if (t.second.second->isNormal()) {
      writeNormalToken(out, pre, *t.second.second, t.second.first);
      pre = L',';
    }
  }
  out << "}\n";
}

////////////////////////////////////////////////////////////////////////////////
void JSONOutputWriter::writeNormalToken(std::wostream &out, wchar_t pre,
                                        const Token &token, size_t n) {
  out << pre;
  writeString(out, token.getWOCR(), true) << ": {\n";
  writeKeyVal(out, L"OCR", token.getWOCR(), true) << ",\n";
  writeKeyVal(out, L"N", n) << ",\n";
  const bool hc = token.candidatesBegin() != token.candidatesEnd();
  writeKeyVal(out, L"HasCandidates", hc) << ",\n";
  pre = L'\n';
  writeString(out, L"Candidates") << ": [";
  for (auto i = token.candidatesBegin(); i != token.candidatesEnd(); ++i) {
    writeCandidate(out, pre, *i);
    pre = L',';
  }
  out << "]\n}";
}

////////////////////////////////////////////////////////////////////////////////
void JSONOutputWriter::writeCandidate(std::wostream &out, wchar_t pre,
                                      const Candidate &candidate) {
  out << pre << "{\n";
  writeKeyVal(out, L"Suggestion", candidate.getString(), true) << ",\n";
  writeKeyVal(out, L"Modern", candidate.getBaseWord(), true) << ",\n";
  writeKeyVal(out, L"Dict", candidate.getDictModule().getName()) << ",\n";
  writeString(out, L"HistPatterns") << ": [";
  pre = L'\n';
  for (const auto &histp : candidate.getHistInstruction()) {
    writeInstruction(out, pre, histp);
    pre = L',';
  }
  out << "],\n";
  pre = L'\n';
  writeString(out, L"OCRPatterns") << ": [";
  for (const auto &ocrp : candidate.getOCRTrace()) {
    writeInstruction(out, pre, ocrp);
    pre = L',';
  }
  out << "],\n";
  writeKeyVal(out, L"Distance", candidate.getDlev()) << ",\n";
  writeKeyVal(out, L"Weight", candidate.getVoteWeight()) << "\n";
  out << "}";
}

////////////////////////////////////////////////////////////////////////////////
void JSONOutputWriter::writeInstruction(std::wostream &out, wchar_t pre,
                                        const csl::PosPattern &instr) {
  out << pre << "{";
  writeKeyVal(out, L"Left", instr.getLeft(), true) << ",";
  writeKeyVal(out, L"Right", instr.getRight(), true) << ",";
  writeKeyVal(out, L"Pos", instr.getPosition()) << "}\n";
}
