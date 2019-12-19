#ifndef OCRCorrection_SimpleXMLReader_h__
#define OCRCorrection_SimpleXMLReader_h__

#include <string>
#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/sax/AttributeList.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>

XERCES_CPP_NAMESPACE_USE

namespace csl {
class AdditionalLex; // forward decl
}
namespace OCRCorrection {
class Document; // forward decl

class Xstr {
public:
  Xstr(const XMLCh *str) : str_(XMLString::transcode(str)) {}
  ~Xstr() { XMLString::release(&str_); }
  bool operator==(const char *other) const { return strcmp(str_, other) == 0; }
  bool operator<(const char *other) const { return strcmp(str_, other) < 0; }
  const char *str() const { return str_; }

private:
  char *str_;
};

class SimpleXMLReader : public HandlerBase {
public:
  SimpleXMLReader(csl::AdditionalLex &alex)
      : alex_(alex), doc_{}, text_{}, cor_{}, state_{}, count_{} {}

  // Parses a simple xml file format.  The root element is of the
  // document should be `<text>`.  All text within is parsed normally.
  // It is possible to encode additinal lexicon entries using
  // `<alex entry="entry"/>` (additional lexicon entries are not assumed
  // to be part of the text to be processed).  Single tokens in the
  // text can be marked up with a correction using `<cor
  // val="right">wrong</cor>`.  The `<lb/>` and `<pb/>` tags mark line
  // or page breaks respectively.  A page break allways implies a line
  // break as well.  Anything within `<meta>...</meta>` is ignored.
  void parse(const std::string &path, Document &document);

  // SAX handles
  virtual void characters(const XMLCh *const chars,
                          const XMLSize_t length) override;
  virtual void startElement(const XMLCh *const name,
                            AttributeList &attrs) override;
  virtual void endElement(const XMLCh *const name) override;
  virtual void startDocument() override;
  virtual void endDocument() override;
  virtual void fatalError(const SAXParseException &ex) override;

private:
  void addTokens();
  void addCor();
  void addAlex(const std::string &entry);
  void addLineBreak();
  void addPageBreak();
  void appendText(const XMLCh *chars, const XMLSize_t len);
  static std::string getAttr(const char *str, AttributeList &attrs);
  enum state { start, text, meta, cor };

  csl::AdditionalLex &alex_;
  Document *doc_;
  std::string text_;
  std::string cor_;
  state state_;
  int count_;
};
} // namespace OCRCorrection

#endif // OCRCorrection_SimpleXMLReader_h__
