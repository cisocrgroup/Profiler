#ifndef lev_WagnerFischer_hpp__
#define lev_WagnerFischer_hpp__

#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>
#include <string>
#include <tuple>
#include <cstddef>

namespace lev {
        class Line;
        class WagnerFischer {
        public:
                enum class EditOp {N=L'|', S=L'#', D=L'-', I=L'+'};
                using Trace = std::vector<EditOp>;
                size_t operator()(const std::wstring& truth,
                                  const std::wstring& pred);
                const std::wstring& pred() const noexcept {
                        return pred_;
                }
                const std::wstring& truth() const noexcept {
                        return truth_;
                }
                const Trace& trace() const noexcept {
                        return trace_;
                }
                void backtrack();

        private:
                size_t getMin(size_t i, size_t j) const noexcept;
                void finishTrace();
                static void cleanCombiningCharacters(std::wstring& str) noexcept;
                std::tuple<EditOp, size_t, size_t>
                backtrack(size_t i, size_t j) const noexcept;

                using Table = std::vector<std::vector<size_t> >;
                std::wstring truth_, pred_;
                Trace trace_;
                Table l_;
        };

        template<class Os>
        Os& operator<<(Os& os, const WagnerFischer::Trace& trace)
        {
                static const auto transform = [](WagnerFischer::EditOp op) {
                        return static_cast<wchar_t>(op);
                };
                std::transform(begin(trace), end(trace),
                               std::ostream_iterator<wchar_t, wchar_t>(os),
                               transform);
                return os;
        }
};

#endif // lev_WagnerFischer_hpp__
