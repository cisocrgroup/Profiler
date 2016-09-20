#include <cassert>
#include <algorithm>
#include <unicode/uchar.h>
#include "WagnerFischer.hpp"

////////////////////////////////////////////////////////////////////////////////
size_t
lev::WagnerFischer::operator()(const std::wstring& truth,
                               const std::wstring& pred)
{
        truth_ = truth;
        pred_ = pred;
        const auto truthn = truth_.size();
        const auto predn = pred_.size();
        l_.clear();
        l_.emplace_back(truthn + 1, 0);
        std::iota(l_[0].begin(), l_[0].end(), 0);
        for (auto i = 1U; i < predn + 1; ++i) {
                l_.emplace_back(truthn + 1, 0);
                l_[i][0] = i;
                for (auto j = 1U; j < truthn + 1; ++j) {
                        l_[i][j] = getMin(i, j);
                }
        }
        return l_[predn][truthn];
}

////////////////////////////////////////////////////////////////////////////////
size_t
lev::WagnerFischer::getMin(size_t i, size_t j) const noexcept
{
        assert(i > 0);
        assert(j > 0);
        if (pred_[i - 1] == truth_[j - 1]) {
                return l_[i - 1][j - 1];
        } else {
                return std::min(l_[i - 1][j - 1] + 1,
                                std::min(l_[i - 1][j] + 1, l_[i][j - 1] + 1));

        }
}

////////////////////////////////////////////////////////////////////////////////
void
lev::WagnerFischer::backtrack()
{
        trace_.clear();
        trace_.reserve(std::max(truth_.size(), pred_.size()));
        for (size_t i = pred_.size(), j = truth_.size(); i > 0 or j > 0;) {
                auto t = backtrack(i, j);
                i = std::get<1>(t);
                j = std::get<2>(t);
                trace_.push_back(std::get<0>(t));
        }
        finishTrace();
}

////////////////////////////////////////////////////////////////////////////////
void
lev::WagnerFischer::finishTrace()
{
        std::reverse(begin(trace_), end(trace_));
        for (size_t i = 0; i < trace_.size(); ++i) {
                switch (trace_[i]) {
                case EditOp::N:
                case EditOp::S:
                        break;
                case EditOp::D:
                        pred_.insert(i, 1, L'_');
                        break;
                case EditOp::I:
                        truth_.insert(i, 1, L'_');
                        break;
                default:
                        assert(false);
                }
        }
        assert(trace_.size() == truth_.size());
        assert(trace_.size() == pred_.size());
//        cleanCombiningCharacters(pred_);
//        cleanCombiningCharacters(truth_);
}

////////////////////////////////////////////////////////////////////////////////
void
lev::WagnerFischer::cleanCombiningCharacters(std::wstring& str) noexcept
{
        for (size_t i = 0; i < str.size(); ++i) {
                if (u_charType(str[i]) == U_NON_SPACING_MARK) {
                        //str.insert(i, 1, L'◌');
                        str.insert(i, 1, L'x');
                        ++i;
                }
        }
}


////////////////////////////////////////////////////////////////////////////////
std::tuple<lev::WagnerFischer::EditOp, size_t, size_t>
lev::WagnerFischer::backtrack(size_t i, size_t j) const noexcept
{
        assert(i > 0 or j > 0);
        if (i > 0 and j > 0) {
                const size_t x[] = {l_[i-1][j-1], l_[i][j-1], l_[i-1][j]};
                auto m = std::min_element(std::begin(x), std::end(x));
                switch (std::distance(std::begin(x), m)) {
                case 0:
                        return x[0] == l_[i][j] ?
                                std::make_tuple(EditOp::N, i-1, j-1) :
                                std::make_tuple(EditOp::S, i-1, j-1);
                case 1:
                        return std::make_tuple(EditOp::D, i, j-1);
                case 2:
                        return std::make_tuple(EditOp::I, i-1, j);
                default:
                        assert(false);
                }
        } else if (i > 0) {
                return std::make_tuple(EditOp::I, i-1, j);
        } else if (j > 0) {
                return std::make_tuple(EditOp::D, i, j-1);
        } else {
                assert(false);
        }
}

// ////////////////////////////////////////////////////////////////////////////////
// std::wostream&
// lev::operator<<(std::wostream& os, const WagnerFischer& wf)
// {
//         const auto n = wf.getTrace().size();
//         for (size_t i = 0, j = 0; i < n; ++i) {
//                 if (wf.getTrace()[i] == WagnerFischer::EditOp::I) {
//                         os << L'_';
//                 } else {
//                         os << wf.getPred()[j];
//                         ++j;
//                 }
//         }
//         os << "\n";
//         for (size_t i = 0; i < n; ++i) {
//                 os << static_cast<wchar_t>(wf.getTrace()[i]);
//         }
//         os << "\n";
//         for (size_t i = 0, j = 0; i < n; ++i) {
//                 if (wf.getTrace()[i] == WagnerFischer::EditOp::D) {
//                         os << L'_';
//                 } else {
//                         os << wf.getTruth()[j];
//                         ++j;
//                 }
//         }
//         return os;
// }
