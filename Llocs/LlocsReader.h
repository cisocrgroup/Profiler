#ifndef OCRCorrection_LlocsReader_h__
#define OCRCorrection_LlocsReader_h__

#include <vector>
#include <utility>
#include <string>

namespace OCRCorrection {
        class Document;
        class Token;

        class LlocsReader {
        public:
                struct Tloc {
                        const char *path;
                        double start, end;
                };

                void parse(int n, char** llocs, Document& document);
                const Tloc& getTloc(const Token& token) const;

        private:
                struct Llocs {
                        const char *path;
                        std::wstring chars;
                        std::vector<double> offsets;
                };
                void add(const Llocs& llocs, Document& document);
                Llocs read(const char* llocs);

                std::vector<Tloc> tlocs_;
        };
}

#endif // OCRCorrection_LlocsReader_h__
