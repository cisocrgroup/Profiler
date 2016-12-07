#ifndef OCRCorrection_GtDoc_hpp__
#define OCRCorrection_GtDoc_hpp__

#include <string>
#include <vector>
#include <tuple>

namespace OCRCorrection {
	class EditOperation {
	public:
		enum class Type {
			None = L'|',
			Substitution = L'#',
			Deletion = L'-',
			Insertion = L'+'
		};
		EditOperation(): type_() {}
		EditOperation(Type type): type_(type) {}
		EditOperation(wchar_t c);
		operator Type() const noexcept {return type_;}
		operator wchar_t() const noexcept {return getChar();}
		wchar_t getChar() const noexcept {
			return static_cast<wchar_t>(type_);
		}
		Type getType() const noexcept {return type_;}
		bool is_none() const noexcept {return type_ == Type::None;}
		bool is_substitution() const noexcept {return type_ == Type::Substitution;}
		bool is_deletion() const noexcept {return type_ == Type::Deletion;}
		bool is_insertion() const noexcept {return type_ == Type::Insertion;}

	private:
		static Type fromChar(wchar_t c);
		Type type_;
	};
	using Trace = std::vector<EditOperation>;
	class GtToken;

	class GtLine {
	public:
		using str_const_iterator = std::wstring::const_iterator;
		using trace_const_iterator = Trace::const_iterator;
		template<class It>
		class Range {
		public:
			Range(It b, It e): b_(b), e_(e) {}
			It begin() const noexcept {return e_;}
			It end() const noexcept {return b_;}
		private:
			const It b_, e_;
		};
		using ConstStringRange = Range<str_const_iterator>;
		using ConstTraceRange = Range<trace_const_iterator>;

		GtLine(): file_(), gt_(), ocr_(), trace_() {}
		GtLine(const std::string& file,const std::wstring& gt,
				const std::wstring& ops, const std::wstring& ocr);
		str_const_iterator gt_begin() const noexcept {return gt_.begin();}
		str_const_iterator gt_end() const noexcept {return gt_.end();}
		ConstStringRange gt_range() const noexcept {return {gt_begin(), gt_end()};}
		str_const_iterator ocr_begin() const noexcept {return ocr_.begin();}
		str_const_iterator ocr_end() const noexcept {return ocr_.end();}
		ConstStringRange ocr_range() const noexcept {return {ocr_begin(), ocr_end()};}
		trace_const_iterator trace_begin() const noexcept {return trace_.begin();}
		trace_const_iterator trace_end() const noexcept {return trace_.end();}
		ConstTraceRange trace_range() const noexcept {return {trace_begin(), trace_end()};}
		inline GtToken as_token() const noexcept;

		size_t size() const noexcept {return gt_.size();}
		bool empty() const noexcept {return gt_.empty();}
		const std::string& file() const noexcept {return file_;}
		const std::wstring& gt() const noexcept {return gt_;}
		const std::wstring& ocr() const noexcept {return ocr_;}
		const Trace& trace() const noexcept {return trace_;}

	private:
		std::string file_;
		std::wstring gt_, ocr_;
		Trace trace_;
	};

	class GtToken {
	public:
		using trace_const_iterator = GtLine::trace_const_iterator;
		using str_const_iterator = GtLine::str_const_iterator;
		using ConstStringRange = GtLine::ConstStringRange;
		using ConstTraceRange = GtLine::ConstTraceRange;

		GtToken(size_t id, size_t b, size_t e, const GtLine* line)
			: id_(id), b_(b), e_(e), line_(line)
		{}
		trace_const_iterator trace_begin() const noexcept {
			return line_->trace_begin() + b_;
		}
		trace_const_iterator trace_end() const noexcept {
			return line_->trace_begin() + e_;
		}
		ConstTraceRange trace_range() const noexcept {
			return {trace_begin(), trace_end()};
		}
		str_const_iterator gt_begin() const noexcept {
			return line_->gt_begin() + b_;
		}
		str_const_iterator gt_end() const noexcept {
			return line_->gt_begin() + e_;
		}
		ConstStringRange gt_range() const noexcept {
			return {gt_begin(), gt_end()};
		}
		str_const_iterator ocr_begin() const noexcept {
			return line_->ocr_begin() + b_;
		}
		str_const_iterator ocr_end() const noexcept {
			return line_->ocr_begin() + e_;
		}
		ConstStringRange ocr_range() const noexcept {
			return {ocr_begin(), ocr_end()};
		}

		bool is_ok() const noexcept;
		bool is_error() const noexcept {return not is_ok();}

	private:
		size_t id_, b_, e_;
		const GtLine* line_;
	};

	class Document;
	class GtDoc {
	public:
		using Lines = std::vector<GtLine>;
		using Tokens = std::vector<GtToken>;

		void clear() {lines_.clear(); tokens_.clear();}
		const Lines& lines() const noexcept {return lines_;}
		Lines& lines() noexcept {return lines_;}
		const Tokens& tokens() const noexcept {return tokens_;}
		void load(const std::string& file);
		void parse(Document& document);

	private:
		Lines lines_;
		Tokens tokens_;
	};

	std::wostream& operator<<(std::wostream& os, const EditOperation& op);
	std::wostream& operator<<(std::wostream& os, const GtLine& line);
	std::wistream& operator>>(std::wistream& is, GtLine& line);
	std::wostream& operator<<(std::wostream& os, const GtToken& token);
	std::wostream& operator<<(std::wostream& os, const Trace& trace);
	std::wostream& operator>>(std::wostream& os, const GtDoc& doc);
	std::wistream& operator>>(std::wistream& is, GtDoc& doc);
}

////////////////////////////////////////////////////////////////////////////////
OCRCorrection::GtToken
OCRCorrection::GtLine::as_token() const noexcept {return {0, 0, size(), this};}

#endif // OCRCorrection_GtDoc_hpp__
