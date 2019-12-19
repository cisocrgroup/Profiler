#include "SimpleXMLReader.hxx"
#include "DictSearch/AdditionalLex.h"
#include "Document/Document.h"
#include <stdexcept>

using namespace OCRCorrection;

////////////////////////////////////////////////////////////////////////////////
void SimpleXMLReader::parse(const std::string &path, Document &document) {
  doc_ = &document;
  xercesc::XMLPlatformUtils::Initialize();
  xercesc::SAXParser parser_;
  parser_.setDocumentHandler(this);
  parser_.setErrorHandler(this);
  parser_.parse(path.c_str());
}

////////////////////////////////////////////////////////////////////////////////
void SimpleXMLReader::characters(const XMLCh *const chars, const XMLSize_t n) {
  switch (state_) {
  case text:
    appendText(chars, n);
    break;
  case cor:
    addTokens();
    appendText(chars, n);
    break;
  default:
    return;
  }
}

////////////////////////////////////////////////////////////////////////////////
void SimpleXMLReader::startElement(const XMLCh *const name,
                                   AttributeList &attrs) {
  if (state_ == meta) { // count meta tag nesting depth
    count_++;
    return;
  }
  const auto tagName = Xstr(name);
  if (tagName == "text" and state_ == start) {
    state_ = text;
  } else if (tagName == "meta" and state_ == text) {
    state_ = meta;
    count_ = 1;
  } else if (tagName == "cor" and state_ == text) {
    state_ = cor;
    cor_ = getAttr("cor", attrs);
  } else if (tagName == "alex" and state_ == text) {
    const auto alex = getAttr("entry", attrs);
    if (not alex.empty()) {
      addAlex(alex);
    }
  } else if (tagName == "lb" and state_ == text) {
    addLineBreak();
  } else if (tagName == "pb" and state_ == text) {
    addPageBreak();
  } else if (state_ != meta) {
    throw std::runtime_error("sax: simple xml: invalid start element: " +
                             std::string(tagName.str()));
  }
}

////////////////////////////////////////////////////////////////////////////////
void SimpleXMLReader::endElement(const XMLCh *const name) {
  if (state_ == meta) { // decrease meta tag nesting depth
    count_--;
    if (count_ == 0) {
      state_ = text;
    }
    return;
  }
  const auto tagName = Xstr(name);
  if (tagName == "text" and state_ == text) {
    state_ = text;
    addTokens();
  } else if (tagName == "alex" and state_ == text) {
    // that's ok;
  } else if (tagName == "cor" and state_ == cor) {
    state_ = text;
    addCor();
  } else if ((tagName == "lb" or tagName == "pb") and state_ == text) {
    // that's ok;
  } else {
    throw std::runtime_error("sax: simple xml: invalid end element: " +
                             std::string(tagName.str()));
  }
}

////////////////////////////////////////////////////////////////////////////////
void SimpleXMLReader::startDocument() {
  count_ = 0;
  state_ = start;
}

////////////////////////////////////////////////////////////////////////////////
void SimpleXMLReader::endDocument() {
  count_ = 0;
  state_ = start;
  doc_->findLineBorders();
}

////////////////////////////////////////////////////////////////////////////////
void SimpleXMLReader::fatalError(const SAXParseException &ex) {
  throw std::runtime_error(std::string(Xstr(ex.getSystemId()).str()) + ":" +
                           std::to_string(ex.getLineNumber()) + ":" +
                           std::to_string(ex.getColumnNumber()) +
                           ": sax: simple xml: fatal error");
}

////////////////////////////////////////////////////////////////////////////////
std::string SimpleXMLReader::getAttr(const char *str, AttributeList &attrs) {
  for (XMLSize_t i = 0; i < attrs.getLength(); i++) {
    const auto key = Xstr(attrs.getName(i));
    if (key == str) {
      return std::string(Xstr(attrs.getValue(i)).str());
    }
  }
  return {};
}

////////////////////////////////////////////////////////////////////////////////
void SimpleXMLReader::appendText(const XMLCh *chars, const XMLSize_t len) {
  auto buf = std::unique_ptr<XMLCh[]>(new XMLCh[len + 1]);
  bool start = true;
  for (XMLSize_t i = 0, j = 0; i < len;) {
    bool hadWS = false;
    while (std::iswspace(chars[i]) and i < len) {
      hadWS = true;
      i++;
    }
    if (hadWS and not start) {
      buf[j++] = ' ';
    }
    if (i < len) {
      buf[j++] = chars[i++];
    }
    buf[j + 1] = 0;
    start = false;
  }
  text_ += std::string(Xstr(buf.get()).str());
}

////////////////////////////////////////////////////////////////////////////////
void SimpleXMLReader::addTokens() {
  auto wtext = Utils::utf8(text_);
  const wchar_t *pos = wtext.c_str();
  size_t len = 0;
  bool isNormal;
  while ((len = Document::getBorder(pos, &isNormal)) != 0) {
    doc_->pushBackToken(pos, len, isNormal);
    pos += len;
  }
  text_.clear();
}

////////////////////////////////////////////////////////////////////////////////
void SimpleXMLReader::addCor() {
  if (cor_.empty()) {
    throw std::runtime_error("missing correction for " + text_);
  }
  doc_->pushBackToken(Utils::utf8(text_), true);
  auto wcor = Utils::utf8(cor_);
  doc_->at(doc_->getNrOfTokens() - 1).metadata()["correction"] = wcor;
  std::transform(wcor.begin(), wcor.end(), wcor.begin(), ::towlower);
  doc_->at(doc_->getNrOfTokens() - 1).metadata()["correction-lc"] = wcor;
  text_.clear();
  cor_ = {};
}

////////////////////////////////////////////////////////////////////////////////
void SimpleXMLReader::addAlex(const std::string &entry) {
  alex_.add(Utils::utf8(entry));
}

////////////////////////////////////////////////////////////////////////////////
void SimpleXMLReader::addPageBreak() { addLineBreak(); }

////////////////////////////////////////////////////////////////////////////////
void SimpleXMLReader::addLineBreak() { doc_->pushBackToken(L"\n", false); }
