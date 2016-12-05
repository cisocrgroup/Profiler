#ifndef OCRC_AdaptiveHistLex_hpp__
#define OCRC_AdaptiveHistLex_hpp__

#include <iosfwd>
#include <map>

namespace OCRCorrection {
	class Token;
	class Document;

	class AdaptiveHistLex {
	public:
		AdaptiveHistLex() = default;
		void load(const std::string& path);
		void save(const std::string& path) const;
		void load(std::wistream& is);
		void save(std::wostream& os) const;
		void add(const Document& doc);
		void add(const Token& token);
		void add(const std::wstring& str);
		const std::wstring& operator[](const Token& token) const;
		const std::wstring& operator[](const std::wstring& str) const;

	private:
		static std::wstring tolower(const std::wstring& str);
		// maps normal-case to lower-case
		std::map<std::wstring, std::wstring> lex_;
	};
}

#endif // OCRC_AdaptiveHistLex_hpp__
