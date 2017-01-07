#ifndef CSL_UnknownVirtualLex_hpp__
#define CSL_UnknownVirtualLex_hpp__

#include <unordered_map>
#include "DictSearch.h"

namespace csl {
	// Virtual lexicon, that handles tokens, for which no other lexicon
	// could generate valid candidates. It has maximum rank.
	class UnknownVirtualLex: public csl::DictSearch::iDictModule {
	public:
		using DictModule = csl::DictSearch::iDictModule;
		using Receiver = csl::DictSearch::iResultReceiver;

		UnknownVirtualLex();
		virtual ~UnknownVirtualLex() noexcept override = default;
		virtual const std::wstring& getName() const override {return name_;}
		virtual bool query(const std::wstring& q, Receiver* res) override {
			return doquery(q, *res);
		}

	private:
		bool doquery(const std::wstring& q, Receiver& res);
		static const std::wstring name_;
	};
}

#endif // CSL_UnknownVirtualLex_hpp__
