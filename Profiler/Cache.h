#ifndef OCRC_CACHE_H__
#define OCRC_CACHE_H__

#include <unordered_map>
#include <functional>
#include <DictSearch/DictSearch.h>

namespace OCRCorrection {
	class Token;
	class Cache {
	public:
		Cache() = default;
		using F = std::function<void(const Token&, csl::DictSearch::CandidateSet&)>;
		size_t count(const Token& token) const;
		const csl::DictSearch::CandidateSet& candiates(const Token& token) const;
		void put(const Token& token, F f);

	private:
		std::unordered_map<std::wstring, std::pair<csl::DictSearch::CandidateSet, size_t>> map_;
	};
}

#endif // OCRC_CACHE_H__
