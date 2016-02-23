#ifndef CSL_VAL_H
#define CSL_VAL_H CSL_VAL_H

#include<vector>

#include<Pattern/PatternGraph.h>
#include<Vaam/Vaam.h>

namespace csl {


    class Val {
    public:

	typedef MinDic<> MinDic_t;

	typedef Vaam< MinDic_t >::CandidateReceiver CandidateReceiver;

	static const size_t INFINITE = (size_t)-1;

	/**
	 * @brief A trivial construtor, taking the input ressources as arguments
	 *
	 * @param baseDic a reference to a MinDic_t that serves as @c baseDic
	 * @param patternFile path to a file containing the spelling variant patterns
	 (see class description for some more details).
	*/
	inline Val( MinDic_t const& baseDic, const char* patternFile );


	/**
	 * @name Configuration
	 */
	//@{

	/**
	 * @brief restrict allowed number of pattern applications to greater or equal than @c n
	 */
	inline void setMinNrOfPatterns( size_t n );

	/**
	 * @brief restrict allowed number of pattern applications to less or equal than @c n
	 */
	inline void setMaxNrOfPatterns( size_t n );

	/**
	 * @brief set the base dictionary. This restricts output words to words NOT present in this filterDic.
	 */
	inline void setBaseDic( MinDic_t const& baseDic ) {
	    baseDic_ = &baseDic;
	}

	/**
	 * @brief Specify a case mode (one of CaseMode) to decide on the treatment of uppercased input.
	 * @param caseMode
	 */
	inline void setCaseMode( Global::CaseMode caseMode ) {
	    caseMode_ = caseMode;
	}

	// @}

	/**
	 * @name Usage/ Queries
	 */
	//@{
	/**
	 * @brief query a @c word to get possible interpretations as a variant.
	 *
	 * You can write your own class that implements the interface iCandidateReceiver to receive all answers,
	 * but you can also use Vaam's subclass CandidateReceiver.
	 *
	 */
	inline bool query( std::wstring const& word, iInterpretationReceiver* interpretations ) const;

	//@}


    private:


	////////////////////// CLASS POSITION /////////////////////////
	/**
	 *
	 */
	class Position {
	public:
	    Position( MinDic_t::State const& dicPos, size_t nrOfPatternsApplied = 0, const std::pair< int, int >& mother = std::make_pair( -1, -1 ) ) :
		dicPos_( dicPos ),
		posPattern_(),
		nrOfPatternsApplied_( nrOfPatternsApplied ) {
		mother_ = mother;
	    }

	    void addPosPattern( const PosPattern& posPattern ) {
		posPattern_ = posPattern;
	    }

	    size_t getNrOfPatternsApplied() const {
		return nrOfPatternsApplied_;
	    }

	    MinDic_t::State dicPos_;

	    /**
	     * @brief Each Position holds one posPattern - this may be an "empty" or dummy pattern.
	     * @see mother_
	     */
	    PosPattern posPattern_;

	    /**
	     * @brief Every Position-object reflects a certain interpretation of the currently processed prefix, and
	     * this variable tells how many patterns this interpretation involves.
	     *
	     * Remember that the complete Instruction that goes with the current Position is not explicitly stored in
	     * a vector or something - instead it must be collected using the posPattern_ and mother_ members.
	     */
	    size_t nrOfPatternsApplied_;

	    /**
	     * (x,y) indicates that the mother is the y-th element at stackpos x
	     * Every Position was created as the successor ("child") of another position.
	     * We need this information to trace back the whole instruction looking at the
	     * distinct posPattern_ s stored with each Position.
	     */
	    std::pair< int, int > mother_;

	}; // class Position


	class StackItem : public std::vector< Position > {
	public:
	    StackItem() :
		lookAheadDepth_( 0 ) {
	    }

	    void clear() {
		std::vector< Position >::clear();
		// somehow reset dicPos_???
	    }

	    size_t lookAheadDepth_;
	    // don't forget this class inherits from std::vector< Position >

	}; // class StackItem


	class Stack : public std::vector< StackItem > {
	public:
	    Stack() {
		// reserve( 500 );
	    }
	}; // class Stack


	inline void applyPatterns( size_t depth, PatternGraph::State const& patternPos ) const;

	inline void reportMatch( const Position* cur, int baseWordScore ) const;

	/**
	 * This method picks up all patterns used to get the match and adds them
	 * to the instruction of the \c Interpretation -object
	 *
	 * @param[in]  cur
	 * @param[in]  baseWordScore
	 * @param[out] answer
	 */

	/**
	 * @return the difference of lengths of modern word <-> variant
	 */
	inline int reportMatch_rec( const Position* cur, Interpretation* answer ) const;


	mutable Stack stack_;

	mutable std::wstring query_;
	mutable bool wasUpperCase_;
	mutable iInterpretationReceiver* interpretations_;

	MinDic_t const* baseDic_;

	PatternGraph patternGraph_;


	Global::CaseMode caseMode_;

	size_t minNrOfPatterns_;
	size_t maxNrOfPatterns_;

    }; // class Val



} // eon

#include "./Val.tcc"

#endif
