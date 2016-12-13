#ifndef OCRC_AdaptiveLex_hpp__
#define OCRC_AdaptiveLex_hpp__

#include <unordered_map>
#include "DictSearch/DictSearch.h"

namespace OCRCorrection {
	class AdaptiveLex: public csl::DictSearch::iDictModule {
	public:
		using DictModule = csl::DictSearch::iDictModule;
		using Receiver = csl::DictSearch::iResultReceiver;

		static const std::wstring& name() noexcept {return NAME;}

		AdaptiveLex(size_t rank, size_t max_lev_);
		virtual ~AdaptiveLex() noexcept override = default;
		virtual const std::wstring& getName() const override {return name();}
		virtual bool query(const std::wstring& q, Receiver* res) override {
			return doquery(q, *res);
		}
		static void add(const std::wstring& gt, const std::wstring& ocr,
				Receiver& receiver);

	private:
		bool doquery(const std::wstring& q, Receiver& res);
		static size_t lev(const std::wstring& a, const std::wstring& b);
		static void add_candidate(const std::wstring& gt, size_t lev,
				Receiver& receiver);

		const size_t max_lev_;

		static std::map<std::tuple<std::wstring, std::wstring>, size_t> CACHE;
		static std::unordered_map<std::wstring, size_t> LEX;
		static std::vector<size_t> COSTS;
		static const std::wstring NAME;
	};
}

#endif // OCRC_AdaptiveLex_hpp__
