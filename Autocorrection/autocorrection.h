#ifndef OCRCorrection_autocorrection_h__
#define OCRCorrection_autocorrection_h__

#include <cstddef>

namespace OCRCorrection {
        class Document;

        // k = 0 means full autocorrect;
        // k > 0 means autocorrect k largest error pattern sets
        void autocorrect(Document& doc, size_t k = 0);
}

#endif // OCRCorrection_autocorrection_h__
