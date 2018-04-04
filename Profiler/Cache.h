#ifndef OCRC_CACHE_H__
#define OCRC_CACHE_H__

#include <unordered_map>

namespace OCRCorrection {
	class Cache {
	public:
		Cache() = default;
		size_t count() const;
		void* candiates() const;
		void put();

	private:
		std::unordered_map<std::wstring, std::pair<void*, size_t>> map_;
	};
}

#endif // OCRC_CACHE_H__
