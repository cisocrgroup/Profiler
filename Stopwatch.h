#ifndef CSL_STOPWATCH_H
#define CSL_STOPWATCH_H CSL_STOPWATCH_H

#include <chrono>

namespace csl {
    class Stopwatch {
    public:
	Stopwatch();
	inline void start();
	inline unsigned long long readSeconds() const;
	inline unsigned long long readMilliseconds() const;
	
    private:
      std::chrono::steady_clock::time_point start_;
    };
    
    inline Stopwatch::Stopwatch() {
	start();
    }
    
    inline void Stopwatch::start() {
      start_ = std::chrono::steady_clock::now();      
    }
    
    inline unsigned long long Stopwatch::readSeconds() const {
      std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
      return std::chrono::duration_cast<std::chrono::seconds>(now - start_).count();
    }
    
    inline unsigned long long Stopwatch::readMilliseconds() const {
      std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
      return std::chrono::duration_cast<std::chrono::milliseconds>(now - start_).count();
    }
} // ns csl
    
#endif
