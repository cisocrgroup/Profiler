#ifndef OCRCorrection_RecPrec_hpp__
#define OCRCorrection_RecPrec_hpp__

#include <sys/stat.h>
#include <sys/types.h>
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
		void write(const std::string& path, const Document& doc,
				const Gt& gt) const;
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
		template<class Gt>
		void write(std::wostream& os, Class c, const Document& doc,
				const Gt& gt) const;
		std::vector<size_t>& operator[](Class c) noexcept {
			return classes_[static_cast<size_t>(c)];
		}

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
		if (token.isNormal()) { // handle normal tokens only
			const auto idx = token.getIndexInDocument();
			(*this)[classify(token, gt.tokens()[idx])].push_back(idx);
		}
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
	default:
		throw std::logic_error("default label in `switch(mode_)` encountered");
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

////////////////////////////////////////////////////////////////////////////////
template<class Gt>
void
OCRCorrection::RecPrec::write(const std::string& dir, const Document& doc,
		const Gt& gt) const
{
	auto info = dir + "/info.txt";
	auto tp = dir + "/true_positive.txt";
	auto tn = dir + "/true_negative.txt";
	auto fp = dir + "/false_positive.txt";
	auto fn = dir + "/false_negative.txt";

	if (mkdir(dir.data(), 0752) != 0)
		throw std::system_error(errno, std::system_category(), dir);

	std::wofstream os;
	os.open(info);
	if (not os.good())
		throw std::system_error(errno, std::system_category(), info);
	os << "# " << Utils::utf8(info) << "\n"
	   << "True positive:  " << true_positives() << "\n"
	   << "True negative:  " << true_negatives() << "\n"
	   << "False positive: " << false_positives() << "\n"
	   << "False negative: " << false_negatives() << "\n"
	   << "Precision:      " << precision() << "\n"
	   << "Recall:         " << recall() << "\n";
	os.close();

	os.open(tp);
	if (not os.good())
		throw std::system_error(errno, std::system_category(), tp);
	os << "# " << Utils::utf8(tp) << "\n";
	write(os, Class::TruePositive, doc, gt);
	os.close();

	os.open(tn);
	if (not os.good())
		throw std::system_error(errno, std::system_category(), tn);
	os << "# " << Utils::utf8(tn) << "\n";
	write(os, Class::TrueNegative, doc, gt);
	os.close();

	os.open(fp);
	if (not os.good())
		throw std::system_error(errno, std::system_category(), fp);
	os << "# " << Utils::utf8(fp) << "\n";
	write(os, Class::FalsePositive, doc, gt);
	os.close();

	os.open(fn);
	if (not os.good())
		throw std::system_error(errno, std::system_category(), fn);
	os << "# " << Utils::utf8(fn) << "\n";
	write(os, Class::FalseNegative, doc, gt);
	os.close();
}

////////////////////////////////////////////////////////////////////////////////
template<class Gt>
void
OCRCorrection::RecPrec::write(std::wostream& os, Class c, const Document& doc,
		const Gt& gt) const
{
	struct CandRange {
		CandRange(const Token& token): token_(token) {}
		Token::CandidateIterator begin() const {return token_.candidatesBegin();}
		Token::CandidateIterator end() const {return token_.candidatesEnd();}
		const Token& token_;
	};

	for (const size_t id: classes_[static_cast<size_t>(c)]) {
		os << gt.tokens()[id] << "\n";
		for (const auto& cand: CandRange(doc.at(id))) {
			os << cand << "\n";
		}
	}
}

#endif // OCRCorrection_RecPrec_hpp__
