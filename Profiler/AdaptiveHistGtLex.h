#ifndef OCRC_AdaptiveHistGtLex_hpp__
#define OCRC_AdaptiveHistGtLex_hpp__

#include <unordered_map>
#include "DictSearch/DictSearch.h"

namespace OCRCorrection {
	class AdaptiveHistGtLex: public csl::DictSearch::iDictModule {
	public:
		using DictModule = csl::DictSearch::iDictModule;
		using Receiver = csl::DictSearch::iResultReceiver;

		static const std::wstring& name() noexcept {return NAME_;}

		AdaptiveHistGtLex(size_t rank, size_t max_lev_);
		virtual ~AdaptiveHistGtLex() noexcept override = default;
		virtual const std::wstring& getName() const override {return name();}
		virtual bool query(const std::wstring& q, Receiver* res) override {
			return doquery(q, *res);
		}
		void add(const std::wstring& gt, const std::wstring& ocr,
				Receiver& receiver);

	private:
		bool doquery(const std::wstring& q, Receiver& res);
		size_t lev(const std::wstring& a, const std::wstring& b);
		void add_candidate(const std::wstring& gt, size_t lev,
				Receiver& receiver);

		std::unordered_map<std::wstring, size_t> gt_;
		std::vector<size_t> costs_;
		const size_t max_lev_;

		static const std::wstring NAME_;
	};
}

#endif // OCRC_AdaptiveHistGtLex_hpp__
