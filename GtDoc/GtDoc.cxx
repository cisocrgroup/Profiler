#include <algorithm>
#include <stdexcept>
#include <iterator>
#include <unicode/uchar.h>
#include <fstream>
#include "Utils/Utils.h"
#include "Document/Document.h"
#include "GtDoc.h"

using namespace OCRCorrection;

////////////////////////////////////////////////////////////////////////////////
EditOperation::EditOperation(wchar_t c)
	: type_(fromChar(c))
{
}

////////////////////////////////////////////////////////////////////////////////
EditOperation::Type
EditOperation::fromChar(wchar_t c)
{
	switch (c) {
	case L'|': return Type::None;
	case L'#': return Type::Substitution;
	case L'-': return Type::Deletion;
	case L'+': return Type::Insertion;
	default:
		   throw std::runtime_error("(EditOperation) Invalid character");
	}
}

////////////////////////////////////////////////////////////////////////////////
GtLine::GtLine(const std::string& file, const std::wstring& gt,
		const std::wstring& ops, const std::wstring& ocr)
	: file_(file)
	, gt_(gt)
	, ocr_(ocr)
	, trace_(gt.size())
{
	std::copy(begin(ops), end(ops), begin(trace_));
	if (gt_.size() != trace_.size() or gt_.size() != ocr_.size()) {
		throw std::runtime_error("(GtLine) gt, ops and ocr do "
				"not have the same length");
	}
}

////////////////////////////////////////////////////////////////////////////////
std::wstring
GtToken::gt() const
{
	std::wstring res;
	res.reserve(size());
	for (auto i = b_; i != e_; ++i) {
		if (not std::next(trace_begin(), i)->is_insertion())
			res.push_back(*(std::next(gt_begin(), i)));
	}
	return res;
}

////////////////////////////////////////////////////////////////////////////////
Trace
GtToken::trace() const
{
	Trace trace(size());
	std::copy(trace_begin(), trace_end(), begin(trace));
	return trace;
}

////////////////////////////////////////////////////////////////////////////////
std::wstring
GtToken::ocr() const
{
	std::wstring res;
	res.reserve(size());
	for (auto i = b_; i != e_; ++i) {
		if (not std::next(trace_begin(), i)->is_deletion())
			res.push_back(*(std::next(gt_begin(), i)));
	}
	return res;
}

////////////////////////////////////////////////////////////////////////////////
bool
GtToken::is_ok() const noexcept
{
	return std::all_of(trace_begin(), trace_end(), [](const EditOperation& op) {
		return op.is_none();
	});
}

////////////////////////////////////////////////////////////////////////////////
void
GtDoc::load(const std::string& file)
{
	std::wifstream is(file);
	if (not is.good())
		throw std::system_error(errno, std::system_category(), file);
	is >> *this;
	is.close();
}

////////////////////////////////////////////////////////////////////////////////
void
GtDoc::parse(Document& document)
{
	document.clear();
	for (const auto& line: lines_) {
		add(line, document);
	}
}

////////////////////////////////////////////////////////////////////////////////
void
GtDoc::add(const GtLine& line, Document& document)
{
	for (auto ofs = 0U; ofs < line.ocr().size();) {
		bool normal = false;
		const auto n = document.findBorder(line.ocr(), ofs, &normal);
		if (n != std::wstring::npos) {
			document.pushBackToken(line.ocr().substr(ofs, n - ofs), normal);
			assert(document.getNrOfTokens() > 0);
			const auto idx  = document.at(document.getNrOfTokens() - 1).
				getIndexInDocument();
			tokens_.resize(idx + 1);
			tokens_[idx] = line.token(idx, ofs, n);
		}
		ofs = n;
	}
}

////////////////////////////////////////////////////////////////////////////////
std::wostream&
OCRCorrection::operator<<(std::wostream& os, const EditOperation& op)
{
	return os << static_cast<wchar_t>(op);
}

////////////////////////////////////////////////////////////////////////////////
std::wostream&
OCRCorrection::operator<<(std::wostream& os, const GtLine& line)
{
	return os << Utils::utf8(line.file()) << "\n" << line.token();
}

////////////////////////////////////////////////////////////////////////////////
static std::wstring&
remove_dottet_circles(std::wstring& str)
{
	auto e = std::remove(begin(str), end(str), 0x25cc /* dottet circle '◌' */);
	str.erase(e, end(str));
	return str;
}

////////////////////////////////////////////////////////////////////////////////
std::wistream&
OCRCorrection::operator>>(std::wistream& is, GtLine& line)
{
	std::wstring file;
	if (not std::getline(is, file))
		return is;
	std::wstring gt;
	if (not std::getline(is, gt))
		return is;
	std::wstring trace(gt.size(), 0);
	if (not std::getline(is, trace))
		return is;
	std::wstring ocr(gt.size(), 0);
	if (not std::getline(is, ocr))
		return is;
	line = GtLine(Utils::utf8(file), remove_dottet_circles(gt),
			trace, remove_dottet_circles(ocr));
	return std::getline(is, file); // read empty line;
}

////////////////////////////////////////////////////////////////////////////////
std::wostream&
OCRCorrection::operator<<(std::wostream& os, const GtToken& token)
{
	os << Utils::utf8(token.file()) << "\n";
	for (wchar_t c: token.gt_range()) {
		if (u_charType(c) == U_NON_SPACING_MARK)
			os << L'◌';
		os << c;
	}
	os << "\n";
	std::copy(token.trace_begin(), token.trace_end(),
			std::ostream_iterator<wchar_t, wchar_t>(os));
	os << "\n";
	for (wchar_t c: token.ocr_range()) {
		if (u_charType(c) == U_NON_SPACING_MARK)
			os << L'◌';
		os << c;
	}
	return os;
}

////////////////////////////////////////////////////////////////////////////////
std::wostream&
OCRCorrection::operator<<(std::wostream& os, const Trace& trace)
{
	std::copy(begin(trace), end(trace),
			std::ostream_iterator<wchar_t, wchar_t>(os));
	return os;
}

////////////////////////////////////////////////////////////////////////////////
std::wostream&
OCRCorrection::operator<<(std::wostream& os, const GtDoc& doc)
{
	for (const auto& line: doc.lines())
		os << line << "\n";
	return os;
}

////////////////////////////////////////////////////////////////////////////////
std::wistream&
OCRCorrection::operator>>(std::wistream& is, GtDoc& doc)
{
	doc.clear();
	GtLine line;
	while (is >> line)
		doc.lines().push_back(line);
	return is;
}
