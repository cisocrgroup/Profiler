#include "ExtReader.h"
#include "DictSearch/AdditionalLex.h"
#include "Document/Document.h"
#include <stdexcept>

using namespace OCRCorrection;

////////////////////////////////////////////////////////////////////////////////
void ExtReader::parse(const std::string &path, Document &document,
                      csl::AdditionalLex &alex) {
  std::wifstream is(path);
  if (not is.good()) {
    throw std::runtime_error("cannot open: " + std::string(path));
  }
  std::wstring line;
  while (std::getline(is, line)) {
    // skip empty lines
    if (line.empty()) {
      continue;
    }
    // add lines with '#' to the lexicon
    if (line[0] == '#') {
      alex.add(line.substr(1));
      continue;
    }
    // all other lines a are tokens with a possible empty correction
    // seperated by ':'; its an error if a token with no correction
    // does not end with ':'. A line only consiting of one ':' is
    // skipped.
    const auto pos = line.find_last_of(L':');
    if (pos == std::wstring::npos) {
      throw std::runtime_error("invalid input line: missing seperator");
    }
    if (pos == 0) {
      continue;
    }
    const auto ocr = line.substr(0, pos);
    auto cor = line.substr(pos + 1);
    document.pushBackToken(ocr, true); // all token are asumed to b normal
    if (not cor.empty()) {
      document.at(document.getNrOfTokens() - 1).metadata()["correction"] = cor;
      std::transform(cor.begin(), cor.end(), cor.begin(), ::towlower);
      document.at(document.getNrOfTokens() - 1).metadata()["correction-lc"] =
          cor;
    }
  }
}
