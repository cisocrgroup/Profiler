#ifndef Llocs_utils_hpp__
#define Llocs_utils_hpp__

namespace OCRCorrection {
        static inline const char *get(const char *key, const char *def = NULL) {
                const char *val = getenv(key);
                if (val)
                        return val;
                if (def)
                        return def;
                throw std::runtime_error(
                        std::string("missing environment var: ") + key);
        }
}

#endif // Llocs_utils_hpp__
