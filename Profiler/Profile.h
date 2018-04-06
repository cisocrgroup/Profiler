#ifndef OCRC_PROFILE_H__
#define OCRC_PROFILE_H__

#include <unordered_map>
#include <functional>
#include <DictSearch/DictSearch.h>

namespace OCRCorrection {
	class Token;
	class Profile;
	class LanguageModel;

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
		void profile(size_t i, bool last, const LanguageModel& lm);
		ConstIterator begin() const {return types_.begin();}
		ConstIterator end() const {return types_.end();}
		Iterator begin() {return types_.begin();}
		Iterator end()  {return types_.end();}

	private:
		void updateCounts(const Token& token);

		Map types_;
		std::map<std::wstring, size_t> counter_;
	};
}

#endif // OCRC_PROFILE_H__
