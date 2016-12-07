#ifndef OCRCorrection_RecPrec_hpp__
#define OCRCorrection_RecPrec_hpp__

#include "Document/Document.h"
#include <array>

namespace OCRCorrection {

	class RecPrec {
	public:
		enum class Class {
			TruePositive,
			TrueNegative,
			FalsePositive,
			FalseNegative,
		};
		enum class Mode {
			Normal,
			Strict,
			VeryStrict,
		};
		const std::vector<size_t>& operator[](Class c) const noexcept {
			return classes_[static_cast<size_t>(c)];
		}
		size_t true_positives() const noexcept {
			return (*this)[Class::TruePositive].size();
		}
		size_t true_negatives() const noexcept {
			return (*this)[Class::TrueNegative].size();
		}
		size_t false_positives() const noexcept {
			return (*this)[Class::FalsePositive].size();
		}
		size_t false_negatives() const noexcept {
			return (*this)[Class::FalseNegative].size();
		}
		size_t sum() const noexcept {
			return true_positives() +
				true_negatives() +
				false_positives() +
				false_negatives();
		}
		void clear() noexcept {
			std::fill(begin(classes_), end(classes_),
					std::vector<size_t>());
		}
		void set_mode(Mode mode) {mode_ = mode;}

		double precision() const noexcept;
		double recall() const noexcept;
		double accuracy() const noexcept;
		double f_measure(double beta = 1) const noexcept;

		template<class Gt>
		void classify(const Document& doc, const Gt& gt);
		template<class GtToken>
		Class classify(const Token& token, const GtToken& gt) const;

	private:
		struct ModeNormal{};
		struct ModeStrict{};
		struct ModeVeryStrict{};
		struct CandidateRange {
			CandidateRange(const Token& token)
				: b_(token.candidatesBegin())
				, e_(token.candidatesEnd())
			{}
			Token::CandidateIterator begin() const noexcept {return b_;}
			Token::CandidateIterator end() const noexcept {return e_;}
			bool empty() const noexcept {return b_ == e_;}
			Token::CandidateIterator b_, e_;
		};
		template<class GtToken>
		static Class classify(const Token& token, const GtToken& gt,
				ModeNormal);
		template<class GtToken>
		static Class classify(const Token& token, const GtToken& gt,
				ModeStrict);
		template<class GtToken>
		static Class classify(const Token& token, const GtToken& gt,
				ModeVeryStrict);
		std::array<std::vector<size_t>, 4> classes_;
		Mode mode_;
	};
}

////////////////////////////////////////////////////////////////////////////////
template<class Gt>
void
OCRCorrection::RecPrec::classify(const Document& doc, const Gt& gt)
{
	for (const auto& token: doc) {
		const auto idx = token.getIndexInDocument();
		classes_[classify(token, gt.tokens()[idx])].push_back(idx);
	}
}

////////////////////////////////////////////////////////////////////////////////
template<class GtToken>
OCRCorrection::RecPrec::Class
OCRCorrection::RecPrec::classify(const Token& token, const GtToken& gt) const
{
	switch (mode_) {
	case Mode::Normal:
		return classify(token, gt, ModeNormal());
	case Mode::Strict:
		return classify(token, gt, ModeStrict());
	case Mode::VeryStrict:
		return classify(token, gt, ModeVeryStrict());
	}
}

////////////////////////////////////////////////////////////////////////////////
template<class GtToken>
OCRCorrection::RecPrec::Class
OCRCorrection::RecPrec::classify(const Token& token, const GtToken& gt,
		ModeNormal)
{
	CandidateRange r(token);
	if (r.empty()) {
		if (gt.is_ok())
			return Class::TrueNegative;
		else
			return Class::FalseNegative;
	} else {
		if (gt.is_error())
			return Class::TruePositive;
		else
			return Class::FalsePositive;
	}
}

////////////////////////////////////////////////////////////////////////////////
template<class GtToken>
OCRCorrection::RecPrec::Class
OCRCorrection::RecPrec::classify(const Token& token, const GtToken& gt,
		ModeStrict)
{
	CandidateRange r(token);
	if (r.empty()) {
		if (gt.is_ok())
			return Class::TrueNegative;
		else
			return Class::FalseNegative;
	} else if (gt.is_ok()) {
			return Class::FalsePositive;
	} else {
		auto i = std::find_if(r.begin(), r.end(), [&gt](const Candidate& cand) {
			return gt.gt() == cand.getWord();
		});
		return i == r.end() ? Class::FalsePositive : Class::TruePositive;
	}
}

////////////////////////////////////////////////////////////////////////////////
template<class GtToken>
OCRCorrection::RecPrec::Class
OCRCorrection::RecPrec::classify(const Token& token, const GtToken& gt,
		ModeVeryStrict)
{
	CandidateRange r(token);
	if (r.empty()) {
		if (gt.is_ok())
			return Class::TrueNegative;
		else
			return Class::FalseNegative;
	} else if (gt.is_ok()) {
			return Class::FalsePositive;
	} else {
		return gt.gt() == r.begin()->getWord() ?
			Class::FalsePositive : Class::TruePositive;
	}
}

#endif // OCRCorrection_RecPrec_hpp__
