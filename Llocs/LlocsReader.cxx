#include <regex>
#include "Document/Document.h"
#include "Token/Token.h"
#include "LlocsReader.h"

////////////////////////////////////////////////////////////////////////////////
std::wstring
OCRCorrection::LlocsReader::Tlocs::string() const
{
     std::wstring str;
     std::transform(b, e, std::back_inserter(str), [](const Llocs::Triple& t) {
          return t.c;
     });
     return str;
}

////////////////////////////////////////////////////////////////////////////////
void
OCRCorrection::LlocsReader::parse(char **llocs, Document& document)
{
     tlocs_.reserve(500);
     for (auto i = llocs; *i; ++i) {
          auto llocs = std::make_shared<Llocs>(*i);
          add(llocs, document);
          llocs_.push_back(llocs);
     }
}

////////////////////////////////////////////////////////////////////////////////
void
OCRCorrection::LlocsReader::add(const LlocsPtr& llocs, Document& document)
{
        size_t pos = 0;
        size_t len = 0;
        bool isNormal = false;
        const auto data = llocs->pred().data();
        const auto n = llocs->pred().size();
	document.newPage( "_NO_IMAGE_DIR_" );
        while ((len = Document::getBorder(data + pos, &isNormal)) != 0) {
             assert(pos < n);
             auto token = document.pushBackToken(data + pos, len, isNormal);
             tlocs_.emplace_back();
             tlocs_.back().token = token;
             tlocs_.back().llocs = llocs;
             tlocs_.back().b = std::next(llocs->begin(), pos);
             tlocs_.back().e = std::next(llocs->begin(), (pos + len));
             pos += len;
        }
}

////////////////////////////////////////////////////////////////////////////////
const OCRCorrection::LlocsReader::Tlocs&
OCRCorrection::LlocsReader::operator[](const Token& token) const
{
        const auto n = token.getIndexInDocument();
        assert(n < tlocs_.size());
        return tlocs_[n];
}
