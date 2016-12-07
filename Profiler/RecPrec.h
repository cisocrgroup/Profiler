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
		size_t operator[](Class c) const noexcept {
			return counts_[static_cast<size_t>(c)];
		}
		size_t true_positives() const noexcept {
			return (*this)[Class::TruePositive];
		}
		size_t true_negatives() const noexcept {
			return (*this)[Class::TrueNegative];
		}
		size_t false_positives() const noexcept {
			return (*this)[Class::FalsePositive];
		}
		size_t false_negatives() const noexcept {
			return (*this)[Class::FalseNegative];
		}
		size_t sum() const noexcept {
			return true_positives() +
				true_negatives() +
				false_positives() +
				false_negatives();
		}
		void clear() noexcept {
			std::fill(begin(counts_), end(counts_), 0);
		}
		void set_mode(Mode mode) {mode_ = mode;}

		double precision() const noexcept;
		double recall() const noexcept;
		double accuracy() const noexcept;
		double f_measure(double beta = 1) const noexcept;

		template<class Gt>
		void classify(const Document& doc, const Gt& gt);

		template<class Gt>
		Class classify(const Token& token, const Gt& gt) const;

	private:
		size_t& operator[](Class c) noexcept {
			return counts_[static_cast<size_t>(c)];
		}
		std::array<size_t, 4> counts_;
		Mode mode_;
	};
}

////////////////////////////////////////////////////////////////////////////////
template<class Gt>
void
OCRCorrection::RecPrec::classify(const Document& doc, const Gt& gt)
{
	for (const auto& token: doc)
		(*this)[classify(token, gt)]++;
}

////////////////////////////////////////////////////////////////////////////////
template<class Gt>
OCRCorrection::RecPrec::Class
OCRCorrection::RecPrec::classify(const Token& token, const Gt& gt) const
{
	return Class::TruePositive;
}

#endif // OCRCorrection_RecPrec_hpp__
