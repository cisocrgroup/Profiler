#include <system_error>
#include "Document/Document.h"
#include "AdaptiveHistLex.h"

using namespace OCRCorrection;

////////////////////////////////////////////////////////////////////////////////
void
AdaptiveHistLex::load(const std::string& path)
{
	std::wifstream is(path);
	if (not is.good())
		throw std::system_error(errno, std::system_category(), path);
	load(is);
}

////////////////////////////////////////////////////////////////////////////////
void
AdaptiveHistLex::save(const std::string& path) const
{
	std::wofstream os(path);
	if (not os.good())
		throw std::system_error(errno, std::system_category(), path);
	save(os);
}

////////////////////////////////////////////////////////////////////////////////
void
AdaptiveHistLex::load(std::wistream& is)
{
	std::wstring line;
	while (std::getline(is, line))
		add(line);
}

////////////////////////////////////////////////////////////////////////////////
void
AdaptiveHistLex::save(std::wostream& os) const
{
	for (const auto& p: lex_)
		os << p.first << "\n";
}

////////////////////////////////////////////////////////////////////////////////
void
AdaptiveHistLex::add(const Document& doc)
{
	for (const auto& token: doc)
		add(token);
}

////////////////////////////////////////////////////////////////////////////////
void
AdaptiveHistLex::add(const Token& token)
{
	if (token.isNormal()) // only add normal token
		add(token);
}

////////////////////////////////////////////////////////////////////////////////
void
AdaptiveHistLex::add(const std::wstring& str)
{
	if (not str.empty()) { // only add non empty words
		auto i = lex_.find(str);
		if (i == end(lex_)) {
			lex_.emplace_hint(i, str, tolower(str));
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
const std::wstring&
AdaptiveHistLex::operator[](const Token& token) const
{
	return (*this)[token.getWCorr()];
}

////////////////////////////////////////////////////////////////////////////////
const std::wstring&
AdaptiveHistLex::operator[](const std::wstring& str) const
{
	static const std::wstring empty;
	auto i = lex_.find(str);
	return i == end(lex_) ? empty : i->second;
}

////////////////////////////////////////////////////////////////////////////////
std::wstring
AdaptiveHistLex::tolower(const std::wstring& str)
{
	std::wstring res(str);
	std::transform(begin(str), end(str), begin(res), towlower);
	return res;
}
