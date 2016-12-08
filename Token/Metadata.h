#ifndef OCRCorrection_Metadata_hpp__
#define OCRCorrection_Metadata_hpp__

#include <map>

namespace OCRCorrection {
	class Token;
	class Metadata {
	public:
		enum class Type {
			Correction,
			GroundTruth,
		};

		Metadata(Token& token)
			: map_()
			, token_(&token)
		{}

		const Token& token() const noexcept {
			return *token_;
		}
		Token& token() noexcept {
			return *token_;
		}
		const std::wstring& operator[](Type type) const {
			return map_.at(type);
		}
		std::wstring& operator[](Type type) {
			return map_[type];
		}
		bool has(Type type) const noexcept {
			return map_.count(type);
		}
		std::unique_ptr<Metadata> clone(Token& token) const {
			std::unique_ptr<Metadata> clone(new Metadata(token));
			clone->map_ = map_;
			return clone;
		}

	private:
		std::map<Type, std::wstring> map_;
		Token* token_;
	};
}

#endif // OCRCorrection_Metadata_hpp__
