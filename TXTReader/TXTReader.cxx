#ifndef OCRC_TXTREADER_CXX
#define OCRC_TXTREADER_CXX OCRC_TXTREADER_CXX

#include "./TXTReader.h"

namespace OCRCorrection {

void TXTReader::parse(char const *inFile, Document *document) {
  std::wcerr << "OCRC::TXTReader::parse" << std::endl;
  std::wstring line;

  const wctype_t alnumType = wctype("alnum");

  size_t length = 0;

  std::wifstream fileIn(inFile);
  if (!fileIn.is_open()) {
    std::wcerr << "OCRC::TXTReader::parse: Bad Filehandle\n";
    throw std::exception();
  }
  // fileIn.imbue( csl::CSLLocale::Instance() );

  wchar_t const *newline = L"\n";

  // quite ugly
  size_t lineCount = 0;
  document->newPage("_NO_IMAGE_DIR_");
  while (fileIn.good()) {
    std::getline(fileIn, line);
    if (lineCount == 0 && !line.empty()) { // search for utf8 byte-order marks!
      if (line.at(0) == 65279) {
        line.erase(0, 1);
      }
    }

    const wchar_t *pos = line.c_str();
    size_t length = 0;
    bool isNormal;
    while ((length = Document::getBorder(pos, &isNormal)) != 0) {
      document->pushBackToken(pos, length, isNormal);
      pos += length;
    }

    document->pushBackToken(newline, 1, false);
    ++lineCount;
  } // for all input text lines

  if (errno == EILSEQ) { // catch encoding error
    std::wcerr << "OCRC::TXTReader::parse: Encoding error in input sequence."
               << std::endl;
    exit(1);
    // throw exceptions::badInput( "MinDic::compileDic: Encoding error in input
    // sequence." );
  }

  std::wcerr << "OCRC::TXTReader::parse: Finished, "
             << document->getNrOfTokens() << " tokens." << std::endl;
}

} // namespace OCRCorrection

#endif
