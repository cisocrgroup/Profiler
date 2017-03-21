#ifndef OCRCorrection_GtDoc_hpp__
#define OCRCorrection_GtDoc_hpp__

#include <string>
#include <vector>
#include <tuple>
#include <memory>
#include <system_error>
#include "Document/Document.h"

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
		bool is_error() const noexcept {return type_ != Type::None;}
		bool is_substitution() const noexcept {return type_ == Type::Substitution;}
		bool is_deletion() const noexcept {return type_ == Type::Deletion;}
		bool is_insertion() const noexcept {return type_ == Type::Insertion;}

	private:
		static Type fromChar(wchar_t c);
		Type type_;
	};

	struct GtChar {
		GtChar() = default;
		GtChar(wchar_t g, wchar_t o, EditOperation e)
			: gt(g), ocr(o), op(e) {}
		bool is_error() const noexcept {return not op.is_none();}
		bool copy_gt() const noexcept {return gt != L'~' or op.is_none();}
		bool copy_ocr() const noexcept {return ocr != L'~' or op.is_none();}
		bool is_normal() const noexcept {
			return copy_ocr() ? Document::isWord(ocr) : true;
		}

		wchar_t gt, ocr;
		EditOperation op;
	};

	struct GtToken;

	class GtLine {
	public:
		using Chars = std::vector<GtChar>;
		using const_iterator = Chars::const_iterator;
		using iterator = Chars::iterator;
		using range = GtToken;

		GtLine(): file_(), chars_() {}
		GtLine(const std::string& file,const std::wstring& gt,
				const std::wstring& ops, const std::wstring& ocr);
		const_iterator cbegin() const noexcept {return chars_.begin();}
		const_iterator cend() const noexcept {return chars_.end();}
		const_iterator begin() const noexcept {return chars_.begin();}
		const_iterator end() const noexcept {return chars_.end();}
		iterator begin() noexcept {return chars_.begin();}
		iterator end() noexcept {return chars_.end();}

		size_t size() const noexcept {return chars_.size();}
		bool empty() const noexcept {return chars_.empty();}
		const std::string& file() const noexcept {return file_;}
		void parse(Document& doc) const;
		template<class F>
		void each_token_gt(F f) const;
		template<class F>
		void each_token_ocr(F f) const;
		template<class F, class G>
		void each_token(G g, F f) const;

	private:
		std::string file_;
		Chars chars_;
	};

	struct GtToken {
		using const_iterator = GtLine::const_iterator;
		GtToken(const_iterator begin, const_iterator end)
			: b(begin), e(end) {}

		const_iterator begin() const noexcept {return b;}
		const_iterator end() const noexcept {return e;}
		bool touch() const noexcept {return normal() and size() > 3;}
		bool normal() const noexcept;
		size_t size() const noexcept {return std::distance(b, e);}
		bool empty() const noexcept {return b == e;}
		const_iterator b, e;
	};

	class GtDoc {
	public:
		using Lines = std::vector<GtLine>;

		void clear() {lines_.clear();}
		const Lines& lines() const noexcept {return lines_;}
		Lines& lines() noexcept {return lines_;}
		void load(const std::string& file);
		void parse(Document& doc) const;
		void parse(const std::string& file, Document& doc);

	private:
		Lines lines_;
	};

	template<class It, class G> It border(It b, It e, G g);
	template<class It> It border_gt(It b, It e);
	template<class It> It border_ocr(It b, It e);

	std::wistream& operator>>(std::wistream& is, GtLine& line);
	std::wistream& operator>>(std::wistream& is, GtDoc& doc);
}

////////////////////////////////////////////////////////////////////////////////
template<class F, class G>
void
OCRCorrection::GtLine::each_token(G g, F f) const
{
	const auto b = begin();
	const auto e = end();
	for (auto i = b; i != e;) {
		auto t = border(i, e, g);
		if (i != t)
			f({i, t});
		i = t;
	}
}

////////////////////////////////////////////////////////////////////////////////
template<class F>
void
OCRCorrection::GtLine::each_token_gt(F f) const
{
	each_token([](const GtChar& c){return c.gt;}, f);
}

////////////////////////////////////////////////////////////////////////////////
template<class F>
void
OCRCorrection::GtLine::each_token_ocr(F f) const
{
	each_token([](const GtChar& c){return c.ocr;}, f);
}

////////////////////////////////////////////////////////////////////////////////
template<class It>
It
OCRCorrection::border_gt(It b, It e)
{
	return border(b, e, [](const GtChar& c) {return c.gt;});
}

////////////////////////////////////////////////////////////////////////////////
template<class It>
It
OCRCorrection::border_ocr(It b, It e)
{
	return border(b, e, [](const GtChar& c) {return c.ocr;});
}

////////////////////////////////////////////////////////////////////////////////
template<class It, class G>
It
OCRCorrection::border(It b, It e, G g)
{
	if (b == e)
		return e;

	if (Document::isSpace(g(*b)))
		return std::find_if(std::next(b), e, [g](const GtChar& c) {
			return g(c) != L'~' and not Document::isSpace(g(c));
		});
	else if (Document::isWord(g(*b)))
		return std::find_if(std::next(b), e, [g](const GtChar& c) {
			return g(c) != L'~' and not Document::isWord(g(c));
		});
	else
		return std::find_if(std::next(b), e, [g](const GtChar& c) {
			return g(c) != L'~' and (
					Document::isWord(g(c)) or
					Document::isSpace(g(c)));
		});
}

#endif // OCRCorrection_GtDoc_hpp__
