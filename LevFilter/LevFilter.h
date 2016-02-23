#ifndef LEVFILTER_H
#define LEVFILTER_H LEVFILTER_H

#include "../Global.h"

namespace csl {
    /**
     * @author Uli Reffle
     * @date 2006
     *
     * @description
     * LevFilter - an interface for classes providing approximate search in
     * dictionaries.
     */
    class LevFilter {
    private:
    public:
	// documentation for ResultSet_if: see below
	class CandidateReceiver;

	/**
	 * compute candidates for \c query and report them to \c candReceiver
	 * @param query
	 * @param candReceiver
	 */
	virtual bool query( const wchar_t* query, CandidateReceiver& candReceiver ) = 0;

	virtual ~LevFilter() {
	    // compiler wants a virtual destructor for abstract classes
	}
    };


    /**
     * This is an interface to be implemented by each class that wants to receive candidates from 
     * some LevFilter implementation.
     *
     */
    class LevFilter::CandidateReceiver {
    public:
	virtual ~CandidateReceiver() {
	}
	
	/**
	 * LevFilter calls this method for each candidate.
	 * @todo there should be a template parameter to specify annotation types other than integer.
	 */
	virtual void receive( const wchar_t* str, int levDistance, int annotation ) = 0;
	
	/**
	 * @todo this method might not be needed anymore. Should be deprecated.
	 */
	virtual void reset() = 0;
    };
    
} // eon

#endif
