#ifndef OCRCorrection_LlocsReader_h__
#define OCRCorrection_LlocsReader_h__

#include <vector>
#include <utility>
#include <string>
#include "Llocs.hpp"

namespace OCRCorrection {
        class Document;
        class Token;

        class LlocsReader {
        public:
                using LlocsPtr = std::shared_ptr<Llocs>;
                struct Tlocs {
                     Tlocs() = default;
                     LlocsPtr llocs;
                     Token* token;
                     Llocs::iterator b, e;
                     std::wstring string() const;
                };

                void parse(char** llocs, Document& document);
                const std::vector<Tlocs>& tlocs() const {return tlocs_;}
                const std::vector<LlocsPtr>& llocs() const {return llocs_;}
                const Tlocs& operator[](const Token& token) const;

        private:
                void add(const LlocsPtr& llocs, Document& document);
                std::vector<Tlocs> tlocs_;
                std::vector<LlocsPtr> llocs_;
        };
}

#endif // OCRCorrection_LlocsReader_h__
