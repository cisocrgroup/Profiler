#ifndef _FAKE_COUNTER_H__
#define _FAKE_COUNTER_H__

struct Counter {
        enum Type {error, error_suspicious, error_destroyed, correct, error_correctable,
                   error_correctable_hasCandidates, not_normal, not_long, normal_and_long,
                   error_correctable_hasCandidates_nohit,
                   error_correctable_hasCandidates_candidateSum,
                   error_correctable_hasNoCandidates,
                   error_correctable_hasCandidates_1best,
                   error_correctable_hasCandidates_3best,
                   error_correctable_hasCandidates_5best,
                   error_correctable_hasCandidates_10best,
                   error_correctable_hasCandidates_50best,
        };

        Counter(): counts_(100) {}
        void clear() {}
        size_t& operator()(Type t) {
                return counts_[t];
        }

        std::vector<size_t> counts_;
};

#endif /* _FAKE_COUNTER_H__ */
