#include "math.h"
#include<iostream>
#include <vector>
#include <algorithm>
#include<string>
#include<limits>
#include<string.h>
#include<Global.h>
#include<Pattern/Instruction.h>
#include<Pattern/PatternProbabilities.h>

namespace csl {

/**
* @brief Computes the weighted levenshtein distance of two words
*
* @author Nurhan Sayhan, Musa Ilker Hatipoglu, Ahmet Saglam
*/

    class ComputeInstruction {
    public:

	/**
	 * @brief Constructor
	 */
	ComputeInstruction();

	/**
	 * @brief Destructor
	 */
	~ComputeInstruction();

	/**
	 * @brief Computes the weight levenshtein distance of two words
	 * @param wCorr The correct word
	 * @param wErr The incorrect word
	 * @param[out] instructions is a pointer to a vector of instructions, which is filled with all (equally) best ways to transform the strings.
	 */
	bool computeInstruction( std::wstring const& wCorr, std::wstring const& wErr, std::vector< Instruction >* instruction = 0 );


	/**
	 * @brief Creates a connection to a LevenshteinProbabilities object
	 */
	void connectPatternProbabilities( PatternProbabilities const& levW );

	void setMaxNumberOfInstructions( size_t n ) {
	    maxNumberOfInstructions_ = n;
	}

	/**
	 * @brief Sets debug_
	 */
	void setDebug(bool d);

    private:
	bool debug_;
	size_t maxNumberOfInstructions_;
	class PatternTypeChain : public PatternProbabilities::PatternType {
	public:
	    PatternTypeChain( PatternProbabilities::PatternType const& patternType, PatternTypeChain* n ) :
		PatternType( patternType ),
		next( n ) {
	    }

	    PatternTypeChain* next;
	};

	class MatrixItem {
	public:
	    MatrixItem() :
		value( PatternProbabilities::UNDEF ),
		patternTypes( 0 ) {
	    }

	    ~MatrixItem() {
		// std::wcerr << "csl::ComputeInstruction::MatrixItem: destructor" << std::endl;
		reset();
	    }
	    void reset() {
		value = PatternProbabilities::UNDEF;
		removePatternTypes();
	    }

	    void removePatternTypes() {
		PatternTypeChain* next = 0;
		while( patternTypes ) {
		    next = patternTypes->next;
		    delete( patternTypes );
		    patternTypes = next;
		}
	    }

	    void addPatternType( PatternProbabilities::PatternType const& patternType ) {
		patternTypes = new PatternTypeChain( patternType, patternTypes );
	    }

	    double value;
	    PatternTypeChain* patternTypes;
	}; // class MatrixItem


	void getInstructions( int x, int y, size_t instructionIndex );

	inline static bool positiveProb( double prob ) {
	    return ( ( prob != PatternProbabilities::UNDEF ) && ( prob > 0 ) );
	}

	/**
	 * @todo Find a good, dynamic solution here!
	 */
	MatrixItem matrix_[50][50];
	std::wstring wordCorr_;
	std::wstring wordErr_;
	const PatternProbabilities* patternProbabilities_;

	std::vector< Instruction >* instructions_;


    }; // class ComputeInstruction

} // ns csl
