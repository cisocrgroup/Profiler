#include "JSONOutputWriter.hxx"
#include "Document/Document.h"

using namespace OCRCorrection;

void
JSONOutputWriter::write() const
{
  for (const auto& token : doc_) {
    if (token.isNormal()) {
      writeNormalToken(token);
    }
  }
}

// {
//   "Vnheilfolles": {
//     "OCR": "Vnheilfolles",
//     "Candidates": [
//       {
//         "Suggestion": "Unheilvolles",
//         "Modern": "unheilvolles",
//         "Dict": "dict_modern_hypothetic_errors",
//         "HistPatterns": null,
//         "OCRPatterns": [
//           {
//             "Left": "u",
//             "Right": "v",
//             "Pos": 0
//           },
//           {
//             "Left": "v",
//             "Right": "f",
//             "Pos": 6
//           }
//         ],
//         "Distance": 2,
//         "Weight": 0.777747
//       },
//       {
//         "Suggestion": "Vnheilvolles",
//         "Modern": "unheilvolles",
//         "Dict": "dict_modern_hypothetic_errors",
//         "HistPatterns": [
//           {
//             "Left": "u",
//             "Right": "v",
//             "Pos": 0
//           }
//         ],
//         "OCRPatterns": [
//           {
//             "Left": "v",
//             "Right": "f",
//             "Pos": 6
//           }
//         ],
//         "Distance": 1,
//         "Weight": 0.11111
//       },

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
JSONOutputWriter::writeNormalToken(const Token& token) const
{
  writeString(out_, token.getWOCR()) << ": {\n";
  writeKeyVal(out_, L"OCR", token.getWOCR()) << "\n";
  writeString(out_, L"Candidates") << ": [\n";
  for (auto i = token.candidatesBegin(); i != token.candidatesEnd(); ++i) {
    writeCandidate(*i);
  }
  out_ << "]\n}\n";
}

void
JSONOutputWriter::writeCandidate(const Candidate& candidate) const
{
  out_ << "{\n";
  writeKeyVal(out_, L"Suggestion", candidate.getString()) << ",\n";
  writeKeyVal(out_, L"Modern", candidate.getString()) << ",\n";
  writeKeyVal(out_, L"Dict", candidate.getDictModule().getName()) << ",\n";
  writeString(out_, L"HistPatterns") << ": [\n";
  for (const auto& histp : candidate.getHistInstruction()) {
    writeInstruction(histp);
  }
  for (const auto& ocrp : candidate.getOCRTrace()) {
    writeInstruction(ocrp);
  }
  writeKeyVal(out_, L"Distance", candidate.getDlev()) << ",\n";
  writeKeyVal(out_, L"Weight", candidate.getVoteWeight()) << "\n";
  out_ << "],\n";
  out_ << "},\n";
}

void
JSONOutputWriter::writeInstruction(const csl::PosPattern& instr) const
{
  out_ << "{\n";
  writeKeyVal(out_, L"Left", instr.getLeft()) << ",\n";
  writeKeyVal(out_, L"Right", instr.getRight()) << ",\n";
  writeKeyVal(out_, L"Pos", instr.getPosition()) << "\n";
  out_ << "},\n";
}

//       {
//         "Suggestion": "Vnheilvolles",
//         "Modern": "unheilvolles",
//         "Dict": "dict_modern_hypothetic_errors",
//         "HistPatterns": [
//           {
//             "Left": "u",
//             "Right": "v",
//             "Pos": 0
//           }
//         ],
//         "OCRPatterns": [
//           {
//             "Left": "v",
//             "Right": "f",
//             "Pos": 6
//           }
//         ],
//         "Distance": 1,
//         "Weight": 0.11111
//       },
