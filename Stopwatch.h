/**
 * @author $Author: uli $
 * @lastrevision $LastChangedBy: uli $ 
 */

#ifndef OCRCORRECTION_STOPWATCH_H
#define OCRCORRECTION_STOPWATCH_H OCRCORRECTION_STOPWATCH_H


#include<sys/timeb.h>


namespace OCRCorrection {

/**
 * @brief This class is an easy helper for measuring performance issues.
 *
 * It is mostly for the development phase - at the moment it's not at all granted
 * (or even very unlikely) that it's portable to windows or other operating systems. 
 *
 */
    class Stopwatch {
    public:
	inline Stopwatch();
	inline void start();
	inline unsigned long long readSeconds() const;
	inline unsigned long long readMilliseconds() const;
    
    private:
	timeb start_;
    };
    
    Stopwatch::Stopwatch() {
	start();
    }
    
    void Stopwatch::start() {
	ftime( &start_ );
    }

    unsigned long long Stopwatch::readSeconds() const {
	timeb now;
	ftime( &now );
	return now.time - start_.time;
    }

    unsigned long long Stopwatch::readMilliseconds() const {
	timeb now;
	ftime( &now );
	return ( now.time - start_.time ) * 1000 + ( now.millitm - start_.millitm );
    }


} // eon

#endif
