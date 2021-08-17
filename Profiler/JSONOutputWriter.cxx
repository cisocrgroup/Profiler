#include "JSONOutputWriter.hxx"
#include "Document/Document.h"
#include "Profiler.h"
#include "Profile.hxx"
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
static std::wostream &writeString(std::wostream &out,
				  const std::wstring &str,
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
static std::wostream &writeKeyVal(std::wostream &out,
				  const std::wstring &key,
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
static std::wostream &writeKeyVal(std::wostream &out,
				  const std::wstring &key,
                                  size_t val) {
  writeString(out, key);
  auto old = out.imbue(std::locale("C"));
  out << ": " << val;
  out.imbue(old);
  return out;
}

////////////////////////////////////////////////////////////////////////////////
static std::wostream &writeKeyVal(std::wostream &out,
				  const std::wstring &key,
                                  const std::wstring &val,
                                  bool tolower = false) {
  writeString(out, key);
  out << ": ";
  writeString(out, val, tolower);
  return out;
}

////////////////////////////////////////////////////////////////////////////////
void JSONOutputWriter::write() const {
  write(out_, doc_, profiler_);
}

////////////////////////////////////////////////////////////////////////////////
void JSONOutputWriter::write(std::wostream &out,
			     const Document &doc,
			     const Profiler& profiler) {
  std::unordered_map<std::wstring, std::pair<size_t, const Token *>> types;
  for (auto i = doc.begin(); i != doc.end(); ++i) {
    // Skip short and/or not normal tokens.
    if (i->isShort() or not i->isNormal()) {
      continue;
    }
    auto wocr = i->getWOCR();
    std::transform(wocr.begin(), wocr.end(), wocr.begin(), ::towlower);

    // Only increment the count for not corrected tokens
    if (not i->has_metadata("correction")) {
      types[wocr].first++;
    }
    types[wocr].second = i.operator->();
  }
  out << "{";
  auto pre = L"\n";
  for (const auto &t : types) {
    if (t.second.second->isNormal()) {
      writeNormalToken(out, profiler, pre, *t.second.second, t.second.first);
      pre = L",\n";
    }
  }
  out << "\n}\n";
}

////////////////////////////////////////////////////////////////////////////////
void JSONOutputWriter::writeNormalToken(std::wostream &out,
					const Profiler& profiler,
					std::wstring pre,
					const Token &token,
					size_t n) {

  out << pre;
  writeString(out, token.getWOCR(), true) << ": {\n";
  writeKeyVal(out, L"OCR", token.getWOCR(), true) << ",\n";
  writeKeyVal(out, L"N", n) << ",\n";
  const bool hc = token.hasCandidates();
  writeKeyVal(out, L"HasCandidates", hc) << ",\n";
  pre = L"\n";
  writeString(out, L"Candidates") << ": [";
  token.eachCandidate([&](const Candidate& cand) {
    writeCandidate(out, profiler, pre, cand);
    pre = L",";
  });
  out << "\n]\n}";
}

////////////////////////////////////////////////////////////////////////////////
void JSONOutputWriter::writeCandidate(std::wostream &out,
				      const Profiler& profiler,
				      std::wstring pre,
				      const Candidate &candidate) {                                  out << pre << "{\n";
  writeKeyVal(out, L"Suggestion", candidate.getString(), true) << ",\n";
  writeKeyVal(out, L"Modern", candidate.getBaseWord(), true) << ",\n";
  writeKeyVal(out, L"Dict", candidate.getDictModule().getName()) << ",\n";
  writeString(out, L"HistPatterns") << ": [";
  pre = L"\n";
  for (const auto &histp : candidate.getHistInstruction()) {
    writeHistInstruction(out, profiler, pre, histp);
    pre = L",";
  }
  out << "],\n";
  pre = L"\n";
  writeString(out, L"OCRPatterns") << ": [";
  for (const auto &ocrp : candidate.getOCRTrace()) {
    writeOCRInstruction(out, profiler, pre, ocrp);
    pre = L",";
  }
  out << "],\n";
  writeKeyVal(out, L"Distance", candidate.getDlev()) << ",\n";
  writeKeyVal(out, L"Weight", candidate.getVoteWeight()) << "\n";
  out << "}";
}

////////////////////////////////////////////////////////////////////////////////
void JSONOutputWriter::writeOCRInstruction(std::wostream &out,
					   const Profiler& profiler,
					   std::wstring pre,
					   const csl::PosPattern &instr) {
  double prob = profiler.getOCRPatterns().getWeight(instr);//0.0;
  out << pre << "{";
  writeKeyVal(out, L"Left", instr.getLeft(), true) << ",";
  writeKeyVal(out, L"Right", instr.getRight(), true) << ",";
  writeKeyVal(out, L"Pos", instr.getPosition()) << ",";
  writeKeyVal(out, L"Prob", prob) << "}\n";
}

////////////////////////////////////////////////////////////////////////////////
void JSONOutputWriter::writeHistInstruction(std::wostream &out,
					    const Profiler& profiler,
					    std::wstring pre,
					    const csl::PosPattern &instr) {
  double prob = profiler.getHistPatterns().getWeight(instr);//0.0;
  out << pre << "{";
  writeKeyVal(out, L"Left", instr.getLeft(), true) << ",";
  writeKeyVal(out, L"Right", instr.getRight(), true) << ",";
  writeKeyVal(out, L"Pos", instr.getPosition()) << ",";
  writeKeyVal(out, L"Prob", prob) << "}\n";
}
