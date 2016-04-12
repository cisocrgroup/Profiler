#include "Document/Document.h"
#include "Token/Token.h"
#include "LlocsReader.h"

////////////////////////////////////////////////////////////////////////////////
void
OCRCorrection::LlocsReader::parse(int n, char **llocs, Document& document)
{
        tlocs_.reserve(n * 5); //about 5 token per line
        for (int i = 0; i < n; ++i) {
                const auto lp = read(llocs[i]);
                add(lp, document);
        }
}

////////////////////////////////////////////////////////////////////////////////
void
OCRCorrection::LlocsReader::add(const Llocs& llocs, Document& document)
{
        size_t pos = 0;
        size_t len = 0;
        bool isNormal = false;
        while ((len = Document::getBorder(llocs.chars.data() + pos, &isNormal)) != 0) {
                assert(pos < llocs.chars.size());
                document.pushBackToken(llocs.chars.data() + pos, len, isNormal);
                Tloc tloc;
                tloc.path = llocs.path;
                tloc.start = llocs.offsets[pos];
                pos += len;
                assert(pos <= llocs.chars.size());
                assert(pos < llocs.offsets.size());
                tloc.end = llocs.offsets[pos];
                tlocs_.push_back(tloc);
        }
}

////////////////////////////////////////////////////////////////////////////////
const OCRCorrection::LlocsReader::Tloc&
OCRCorrection::LlocsReader::getTloc(const Token& token) const
{
        const auto n = token.getIndexInDocument();
        assert(n < tlocs_.size());
        return tlocs_[n];
}

////////////////////////////////////////////////////////////////////////////////
OCRCorrection::LlocsReader::Llocs
OCRCorrection::LlocsReader::read(const char* llocs)
{
        Llocs lp;
        lp.offsets.push_back(0.0); // push back 0 sentinel value
        lp.path = llocs;
        std::wifstream is(llocs);
        if (not is.good())
                throw std::runtime_error("Could not open: " + std::string(llocs));
        wchar_t c, sep;
        std::wstring line;
        while (is.get(c)) {
                if (c == '\t' and std::getline(is, line)) {
                        lp.chars.push_back(' ');
                        lp.offsets.push_back(std::stod(line));
                } else if (is.get(sep) and sep == '\t' and std::getline(is, line)) {
                        lp.chars.push_back(c);
                        lp.offsets.push_back(std::stod(line));
                } else {
                        throw std::runtime_error(std::string(llocs) +
                                                 " Invalid llocs line");
                }
        }
        assert(lp.chars.size() + 1 == lp.offsets.size());
        return lp;
}
