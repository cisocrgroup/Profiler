#ifndef OCRCorrection_SimpleOutputWriter_h__
#define OCRCorrection_SimpleOutputWriter_h__

#include <iostream>

namespace OCRCorrection {
        class Document;
        class Token;

        class SimpleOutputWriter {
        public:
                SimpleOutputWriter(std::wostream& os, const Document& document);
                void write() const;

        private:
                void writeNormalToken(const Token& token) const;
                const Document& document_;
								std::wostream& os_;
        };
}

#endif // OCRCorrection_SimpleOutputWriter_h__
