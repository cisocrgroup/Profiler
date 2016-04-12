#ifndef OCRCorrection_LlocsReader_h__
#define OCRCorrection_LlocsReader_h__

namespace OCRCorrection {
        class Document;
        class LlocsReader {
        public:
                void parse(const char** llocs, Document& document);
        private:
        };
}

#endif // OCRCorrection_LlocsReader_h__
