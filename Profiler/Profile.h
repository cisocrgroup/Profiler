#ifndef OCRC_PROFILE_H__
#define OCRC_PROFILE_H__

#include <unordered_map>
#include <functional>
#include <DictSearch/DictSearch.h>

namespace OCRCorrection {
	class Token;
	class Profile;

	class Profile {
	public:
		struct TokenType {
			static const std::wstring NOT_NORMAL, DONT_TOUCH, SHORT, NORMAL;
		};
		using F = std::function<void(const Token&, csl::DictSearch::CandidateSet&)>;
		using Pair = std::pair<size_t, csl::DictSearch::CandidateSet>;
		using Map = std::unordered_map<std::wstring, Pair>;
		using Iterator = Map::iterator;
		using ConstIterator = Map::const_iterator;

		Profile() = default;
		size_t count(const Token& token) const;
		const csl::DictSearch::CandidateSet& candiates(const Token& token) const;
		void put(const Token& token, F f);
		ConstIterator begin() const {return map_.begin();}
		ConstIterator end() const {return map_.end();}
		Iterator begin() {return map_.begin();}
		Iterator end()  {return map_.end();}

	private:
		void updateCounts(const Token& token);

		Map map_;
		std::map<std::wstring, size_t> counter_;
	};
}

#endif // OCRC_PROFILE_H__
