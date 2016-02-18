#ifndef CSL_VAAM_H
#define CSL_VAAM_H CSL_VAAM_H
#include<iostream>

#include<vector>
#include<stack>
#include<set>
#include<locale>

#include "../Global.h"
#include "../Stopwatch.h"
#include "../MinDic/MinDic.h"
#include "../LevDEA/LevDEA.h"

#include <Pattern/PatternGraph.h>
#include <Pattern/Interpretation.h>


namespace csl {

    /**
     * @brief iInterpretationReceiver is an interface to be implemented by classes which are supposed to
     *        receive results from the Vaam matcher (see below).
     *
     * A straight-forward implementation of this interface is Vaam::CandidateReceiver
     */
    class iInterpretationReceiver {
    public:
	virtual ~iInterpretationReceiver() {}
	virtual void receive( Interpretation const& interpretation ) = 0;
    };


    /*
     * Vaam - Variant-Aware Approximate Matching
     */

    /**
     * @brief Vaam is a module for approximate matching in large dictionaries, considering
     * spelling variants (defined with a set of rewrite patterns) and also spelling errors
     * (using levenshtein distance).
     *
     * Besides this class reference, please read the @link vaam_manual Vaam Manual @endlink to get an idea of how to create a Vaam object,
     * how to send queries and how to interpret Vaam's answer.
     *
     * As input the tool needs
     * - a wordList @c baseDic compiled as FSA (use the tool compileMD to get such an automaton from a word list.
     *
     * - a file containing a list of patterns, one per line. A line containing 't th' indicates that a 't' in
     *   @c baseDic could be transformed to 'th'.
     *
     * - a threshold @c maxNrOfPatterns specifying the maximum number of pattern applications in one word
     *   (default: 1000, which is near to infinite ).
     *
     * - a distance threshold @c distance specifying the maximum number of edit operations allowed on top of the
     *   application of variant patterns (default: 0, allow variant patterns only).
     *
     * Definition of the output:
     *
     * We define a so-called hypothetical dictionary @c hypotheticalDic that contains all spelling variants
     * which can be obtained by applying a sequence of up to @c maxNrOfPatterns variant patterns to any
     * word of @c baseDic. One entry of @c hypotheticalDic is a triple @c{<word,baseWord,instruction>} where
     * @c instruction is the above mentioned sequence to get the variant @c word from the original word
     * @c baseWord from @c baseDic.
     *
     * For a query @c q the algorithm returns all such triples @c{<word,baseWord,instruction>} from @c hypotheticalDic
     * where the standard levenshtein distance between @c word and @c q does not exceed
     *
     * @see csl::Interpretation, csl::Instruction, csl::PosPattern, csl::Pattern
     *
     * @author Ulrich Reffle, 2008
     */
    template< typename MinDicType = MinDic< int > >
    class Vaam {

    public:
	/**
	 * @brief The type of MinDic used as base dictionary.
	 *
	 * Note that other dictionaries used here (e.g. the filterDic) are not necessarily of this type.
	 */
	typedef MinDicType MinDic_t;



	/**
	 * A straight-forward implementation of the interface above.
	 * It inherits from std::vector, has all its features.
	 */
	class CandidateReceiver : public iInterpretationReceiver,
				  public std::vector< Interpretation > {
	public:
	    void receive( csl::Interpretation const& interpretation ) {
		push_back( interpretation );
	    }
	};



    private:

	/**
	 * A state of the MinDic
	 */
	typedef typename MinDic_t::State MDState_t;

    public:

	static const size_t INFINITE = (size_t)-1;

	/**
	 * @name Constructors
	 */
	//@{

	/**
	 * @brief A trivial construtor, taking the input ressources as arguments
	 *
	 * @param baseDic a reference to a MinDic_t that serves as @c baseDic
	 * @param patternFile path to a file containing the spelling variant patterns
	 (see class description for some more details).
	*/
	Vaam( const MinDic_t& baseDic, const char* patternFile );

	//@}


	/**
	 * @name Configuration
	 */
	//@{

	/**
	 * @brief In addition to pattern applications, allow fuzzy search with distance up to @c d
	 */
	inline void setDistance( size_t d );

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
	inline void setBaseDic( MinDic_t const& baseDic );

	/**
	 * @brief set a filter dictionary. This restricts output words to words NOT present in this filterDic.
	 */
	inline void setFilterDic( iDictionary< int > const& filterDic );

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
	 * @return true iff at least one answer was found
	 *
	 */
	inline bool query( std::wstring const& word, iInterpretationReceiver* interpretations ) const;

	//@}

    private:
	inline void query_rec( size_t depth ) const;


	////////////////////// CLASS POSITION /////////////////////////
	/**
	 *
	 */
	class Position {
	public:
	    Position( const LevDEA::Pos& levPos, size_t nrOfPatternsApplied, const std::pair< int, int >& mother = std::make_pair( -1, -1 ) ) :
		levPos_( levPos ),
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

	    LevDEA::Pos levPos_;

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


	////////////////////// CLASS STACKITEM /////////////////////////

	class StackItem : public std::vector< Position > {
	public:
	    StackItem( const Vaam& myVaam ) :
		dicPos_( *( myVaam.baseDic_ ) ),
		patternPos_( myVaam.patternGraph_, 0 ),
		lookAheadDepth_( 0 ) {
	    }

	    void clear() {
		std::vector< Position >::clear();
		// somehow reset dicPos_???
	    }

	    MDState_t dicPos_;
	    PatternGraph::State patternPos_;
	    size_t lookAheadDepth_;
	    // don't forget this class inherits from std::vector< Position >

	}; // class StackItem


	class Stack : public std::vector< StackItem > {
	public:
	    Stack() {
		// reserve( 500 );
	    }
	};

	void printPosition( const Position& pos ) const;

	/**
	 */
	void reportMatch( const Position* cur, int baseWordScore ) const;

	/**
	 * This method picks up all patterns used to get the match and adds them
	 * to the instruction of the \c Interpretation -object
	 *
	 * @param[in]  cur
	 * @param[in]  baseWordScore
	 * @param[out] answer
	 */
	void reportMatch_rec( const Position* cur, Interpretation* answer ) const;

	void applyPatterns( size_t depth ) const;
	void checkForMatches( size_t depth ) const;

	/////   DATA MEMBERS OF VAAM    //////////////////////

	MinDic_t const* baseDic_;
	iDictionary< int > const* filterDic_;

	PatternGraph patternGraph_;
	std::vector< std::wstring > leftSidesList_;
	std::vector< PatternGraph::Replacements_t > rightSides_;

	mutable LevDEA levDEA_;
	mutable std::wstring query_;
	mutable bool wasUpperCase_;

	mutable iInterpretationReceiver* interpretations_;
	mutable bool foundAnswers_;
	mutable Stack stack_;
	/**
	 * The current string under construction
	 */
	mutable std::wstring baseWord_;

	Global::CaseMode caseMode_;

	std::locale locale_;

	size_t minNrOfPatterns_;
	size_t maxNrOfPatterns_;

    }; // class Vaam



} // namespace csl

#include "./Vaam.tcc"
#endif
