/**
 * @author $Author: uli $
 * @lastrevision $LastChangedBy: uli $
 */

#ifndef OCRCORRECTION_EXCEPTIONS_H
#define OCRCORRECTION_EXCEPTIONS_H OCRCORRECTION_EXCEPTIONS_H

#include <string>
#include <exception>
#include "Utils/Utils.h"

namespace OCRCorrection {

    class OCRCException : public std::exception {
    public:
	OCRCException() {
	}

	OCRCException( std::string const& msg ) : msg_( msg ) {
	}

        OCRCException( std::wstring const& msg ) {}//: msg_(Utils::utf8(msg)) {}

	virtual ~OCRCException() throw() {}

	virtual const char* what() const throw() {
	    return msg_.c_str();
	}

    private:
	std::string msg_;
    };

} // eon

#endif
