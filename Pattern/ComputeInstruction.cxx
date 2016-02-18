#include "./ComputeInstruction.h"

namespace csl {
    ComputeInstruction::ComputeInstruction() : 
	debug_( false ),
	maxNumberOfInstructions_( (size_t) -1 ),
	patternProbabilities_( 0 ),
	instructions_( 0 ) {


	/** 
	 * @todo Find a good, dynamic solution here!
	 */
	memset( matrix_, 0, 50 * 50 * sizeof( MatrixItem ) );
	
    }
    
    ComputeInstruction::~ComputeInstruction(){
    }

    void ComputeInstruction::connectPatternProbabilities( csl::PatternProbabilities const& patternProbabilities ){
	patternProbabilities_ = &patternProbabilities;
    }
    
    
    bool ComputeInstruction::computeInstruction( std::wstring const& wCorr, 
						 std::wstring const& wErr, 
						 std::vector< Instruction >* instructions ) {

	if( wCorr.size() >= 45 || wErr.size() >= 45 ) {
	    throw exceptions::badInput( "csl::ComputeInstruction; input words too long" );
	}
	
 	//std::wcerr << "compare " << wCorr << ", " << wErr << std::endl;

	if( ! patternProbabilities_ ) {
	    throw exceptions::LogicalError( "csl::ComputeInstruction::ComputeInstruction: No patternProbabilities loaded." );
	    exit( 1 );
	}
	
	//levProbabilities_->printFreqPattern();
	//levProbabilities_->printFreqCorrect();
	//levProbabilities_->printPatternProbabilities();
	
	wordCorr_ = L" " + wCorr;
	wordErr_ = L" " + wErr;
	
	int matrixW = wordCorr_.length();
	int matrixH = wordErr_.length();
	
	//Set the values for the first row
	matrix_[0][0].value = 0;
	for(int i=1; i < matrixW; ++i ) {
	    double prob = patternProbabilities_->getWeight( Pattern( wordCorr_.substr( i, 1 ), L"" ) );
	    if( ( matrix_[0][i-1].value != PatternProbabilities::UNDEF ) && ( positiveProb( prob ) ) ) {
		matrix_[0][i].value = matrix_[0][i-1].value + ( -log( prob ) );
		matrix_[0][i].addPatternType( PatternProbabilities::PatternType( 1, 0 ) );
	    }
	    else {
		matrix_[0][i].value = PatternProbabilities::UNDEF;
		matrix_[0][i].removePatternTypes();
	    }
	}
	
	//Set the values for the first column
	for(int i=1; i < matrixH; ++i ) {
	    double prob = patternProbabilities_->getWeight( Pattern( L"", wordErr_.substr( i, 1 ) ) );
	    if( ( matrix_[i-1][0].value != PatternProbabilities::UNDEF) && ( positiveProb( prob ) ) ) {
		matrix_[i][0].value = matrix_[i-1][0].value + ( -log( prob ) );
		matrix_[i][0].addPatternType( PatternProbabilities::PatternType( 0, 1 ) );
	    }
	    else {
		matrix_[i][0].value = PatternProbabilities::UNDEF;
		matrix_[i][0].removePatternTypes();
	    }
	}

	
	// Set the values of the matrix.
	double prob = 0;

	for( int y=1; y < matrixH; y++ ) {
	    for( int x = 1; x < matrixW; ++x ){
		double minValue =  PatternProbabilities::UNDEF;
		double newValue = 0;

		// match, substitution
		if( matrix_[y-1][x-1].value == PatternProbabilities::UNDEF ) {
		    // do nothing
		}
		else if( wordCorr_.at(x) == wordErr_.at(y) ) {
		    newValue = matrix_[y-1][x-1].value;
		    
		    if( ( minValue == PatternProbabilities::UNDEF ) || ( newValue < minValue ) ) {
			minValue = newValue;
			matrix_[y][x].removePatternTypes();
			matrix_[y][x].addPatternType( PatternProbabilities::PatternType( 1, 1 ) );
		    }
		    else if( newValue == minValue) {
			matrix_[y][x].addPatternType( PatternProbabilities::PatternType( 1, 1 ) );
		    }
		}
		else {
		    prob = patternProbabilities_->getWeight( Pattern( wordCorr_.substr( x, 1 ), wordErr_.substr( y, 1 ) ) );

		    if( ( positiveProb( prob ) ) ) {
			newValue = matrix_[y-1][x-1].value + ( -log( prob ) );
		    
			if( ( minValue == PatternProbabilities::UNDEF ) || ( newValue < minValue ) ) {
			    minValue = newValue;
			    matrix_[y][x].removePatternTypes();
			    matrix_[y][x].addPatternType( PatternProbabilities::PatternType( 1, 1 ) );
			}
			else if( newValue == minValue ) {
			    matrix_[y][x].addPatternType( PatternProbabilities::PatternType( 1, 1 ) );
			}
		    }
		}

		
		
		// Insert
		if( ( matrix_[y-1][x].value != PatternProbabilities::UNDEF ) ) {
		    prob = patternProbabilities_->getWeight( Pattern( L"", wordErr_.substr( y, 1 ) ) );
		    if( ( positiveProb( prob ) ) ) {
			newValue = matrix_[y-1][x].value + ( -log( prob ) );

			if( ( minValue == PatternProbabilities::UNDEF ) || ( newValue < minValue ) ) {
			    minValue = newValue;
			    matrix_[y][x].removePatternTypes();
			    matrix_[y][x].addPatternType( PatternProbabilities::PatternType( 0, 1 ) );
			}
			else if( newValue == minValue ) {
			    matrix_[y][x].addPatternType( PatternProbabilities::PatternType( 0, 1 ) );
			}
		    }
		}
		
		// Delete
		if( ( matrix_[y][x-1].value != PatternProbabilities::UNDEF ) ) {
		    prob = patternProbabilities_->getWeight( Pattern( wordCorr_.substr( x, 1 ), L"" ) );
		    if( ( positiveProb( prob ) ) ) {
			newValue = matrix_[y][x-1].value + ( -log( prob ) );
			
			if( ( minValue == PatternProbabilities::UNDEF ) || ( newValue < minValue ) ) {
			    minValue = newValue;
			    matrix_[y][x].removePatternTypes();
			    matrix_[y][x].addPatternType( PatternProbabilities::PatternType( 1, 0 ) );
			}
			else if(newValue == minValue) {
			    matrix_[y][x].addPatternType( PatternProbabilities::PatternType( 1, 0 ) );
			}
		    }
		}
		
		//std::wcout << "\"" << wordCorr_[x]<< " "<< wordErr_[y] << "\" | ";
		
		//Merge
		if( ( x >= 2 ) && ( matrix_[y-1][x-2].value != PatternProbabilities::UNDEF ) ) {
		    prob = patternProbabilities_->getWeight( Pattern( wordCorr_.substr( x-1, 2 ), wordErr_.substr( y, 1 ) ) );
		    if( positiveProb( prob ) ) {
			newValue = matrix_[y-1][x-2].value + ( -log( prob ) );

			if( ( minValue == PatternProbabilities::UNDEF ) || (newValue < minValue ) ) {
			    minValue = newValue;
			    matrix_[y][x].removePatternTypes();
			    matrix_[y][x].addPatternType( PatternProbabilities::PatternType( 2, 1 ) );
			}
			else if( newValue == minValue ) {
			    matrix_[y][x].addPatternType( PatternProbabilities::PatternType( 2, 1 ) );
			}
		    }
		}

		//Split
		if( ( y >= 2 ) && ( matrix_[y-2][x-1].value != PatternProbabilities::UNDEF ) ) {
		    prob = patternProbabilities_->getWeight( Pattern( wordCorr_.substr( x, 1 ), wordErr_.substr( y-1, 2 ) ) );

		    
		    prob = patternProbabilities_->getWeight( Pattern( wordCorr_.substr( x, 1 ), wordErr_.substr( y-1, 2 ) ) );
		    if( positiveProb( prob ) ) {
			newValue = matrix_[y-2][x-1].value + ( -log( prob ) );
			

			if( ( minValue == PatternProbabilities::UNDEF ) || ( newValue < minValue ) ) {
			    minValue = newValue;
			    matrix_[y][x].removePatternTypes();
			    matrix_[y][x].addPatternType( PatternProbabilities::PatternType( 1, 2 ) );
			}
			else if( newValue == minValue ) {
			    matrix_[y][x].addPatternType( PatternProbabilities::PatternType( 1, 2 ) );
			}
			else {
			}
			
		    }
		} // split
				
		
		matrix_[y][x].value = minValue;				
		
		
	    } // for( y = 1 .. matrixW )
	    
	    //std::wcout << "-------------------------------" <<std::endl;
			
	} // for(y = 1 .. matrixH)

	if( debug_ ) {
	    //std::wcout << "wCorr (wstring&): " << "\'"<< wCorr << "\'"<< std::endl;
	    //std::wcout << "wErr (wstring&): " << "\'"<< wErr << "\'"<< std::endl;
	    std::wcout << "wordCorr_ (wstring): " << "\'" << wordCorr_ << "\'"<< std::endl;
	    std::wcout << "wordErr_ (wstring): " << "\'" << wordErr_ << "\'"<< std::endl;
	    std::wcout << "matrixW (wordCorr_.length()): " << matrixW << std::endl;
	    std::wcout << "matrixH (wordErr_.length()): " << matrixH << std::endl;
	    
	    std::wcout << std::endl << "  |" ;
	    for(int x=0; x<matrixW; x++){
		std::wcout << " " << wordCorr_[x] << " |";
	    }
	    std::wcout << std::endl;
	    
	    for(int y=0; y<matrixH; y++){
		std::wcout << wordErr_[y] << " |";
		for(int x = 0; x < matrixW; x++){
		    std::wcout << " " << matrix_[y][x].value;
		    if( matrix_[y][x].patternTypes ) {
			std::wcout <<  "[" 
				   << matrix_[y][x].patternTypes->first << ","
				   << matrix_[y][x].patternTypes->second << "]";
		    }
		    else std::wcout << "[   ]";
		    
		    std::wcout << " |";
		    
		}
		std::wcout << std::endl;
	    }
	    std::wcout << std::endl;
	    
	    int x = matrixW - 1;
	    int y = matrixH - 1;
	    PatternTypeChain* patternType = matrix_[y][x].patternTypes;
	    while( patternType ) {
		if( ( patternType->first == 1 ) && ( patternType->second == 1 ) && ( wordCorr_.at( x ) == wordErr_.at( y ) ) ) {
		    x -= 1;
		    y -= 1;
		    // do nothing: match
		}
		else {
		    std::wcout<< wordCorr_.substr( x-patternType->first + 1, patternType->first ) << "->" << wordErr_.substr( y - patternType->second + 1, patternType->second ) << " at pos " << x - patternType->first  << std::endl;
		    x -= patternType->first;
		    y -= patternType->second;
		}
		
		patternType = matrix_[y][x].patternTypes;
	    }
	    
	} // if debug_

	bool doReturn = ( matrix_[matrixH-1][matrixW-1].value != PatternProbabilities::UNDEF );
	
	instructions_ = instructions;
	
	if( instructions_ ) { 
	    if( ! instructions_->empty() ) {
		throw csl::exceptions::cslException( "csl::ComputeInstruction::computeInstruction: answer object 'instructions' not empty." );
	    }

	    if( doReturn == 1 ) {
		instructions_->push_back( Instruction() ); // create a first instruction to work on
		getInstructions( matrixW-1, matrixH-1, 0 );
	    }
	}


	for( int y=0; y < matrixH; y++ ) {
	    for( int x = 0; x < matrixW; ++x ){
		matrix_[y][x].reset();
	    }
	}

	return doReturn;

    } // function computeInstruction


    /**
     * @brief recursive method to find all best paths through the levenshtein-matrix and store the resulting csl::Instruction s
     */
    void ComputeInstruction::getInstructions( int x, int y, size_t instructionIndex ) {
	//std::wcout << "Enter getInstructions" << std::endl;

	if( ( ( x == 0 ) && ( y == 0  ) ) ) {
	    // no recursive calls, we're at the end. Reverse the complete instruction
	    std::reverse( instructions_->at( instructionIndex ).begin(), instructions_->at( instructionIndex ).end() );
	}
	else {
	    PatternTypeChain* patternType = matrix_[y][x].patternTypes;
	    
	    if( patternType == 0 ) {
		//return;
		throw std::runtime_error( "patternType can not be 0" );
	    }
	    
	    size_t countPatternTypes = 0;
	    while( patternType ) {
		size_t currentInstructionIndex = 0;
		
		if( patternType->first == 0 && patternType->second == 0 ) {
		    throw std::runtime_error( "patternType can not be (0,0)" );
		}
		
		if( patternType->next == 0  ) { // no need to clone for the last patternType
		    currentInstructionIndex = instructionIndex; 
		    //std::wcerr<<"Do not clone: work on instr["<<instructionIndex<<"]"<<std::endl;
		    //std::wcerr<<"instr.size() is "<<instructions_->size()<<std::endl;
		}
		else { // clone
		    if( ( maxNumberOfInstructions_ != (size_t)-1 ) && ( instructions_->size() >= maxNumberOfInstructions_ ) ) {
			return;
		    }
		    else {
			//std::wcout << "size=" << instructions_->size() << std::endl;
			instructions_->reserve( instructions_->size() + 1 );
			instructions_->push_back( instructions_->at( instructionIndex ) ); // clone the instruction as built so far
			currentInstructionIndex = instructions_->size() - 1;
			//std::wcout << "size=" << instructions_->size() << ", currentInstructionIndex=" << currentInstructionIndex << std::endl;
		    }
		}

		// match: continue
		if( ( patternType->first == 1 ) && ( patternType->second == 1 ) && ( wordCorr_.at( x ) == wordErr_.at( y ) ) ) {
		    getInstructions( x - 1, y - 1, 
				     currentInstructionIndex ); // recursive call
		}
		else {
		    
		//std::wcout<< wordCorr_.substr( x-patternType->first + 1, patternType->first ) << "->" << wordErr_.substr( y - patternType->second + 1, patternType->second ) << " at pos " << x - patternType->first  << std::endl;
		
		    
		    instructions_->at( currentInstructionIndex ).push_back( 
			PosPattern( 
			    wordCorr_.substr( x-patternType->first + 1, patternType->first ),
			    wordErr_.substr( y - patternType->second + 1, patternType->second ),
			    x - patternType->first ) 
			);
		    
		    //std::wcout << "Rec call(" << x - patternType->first<< "," << y - patternType->second << "," << currentInstructionIndex << std::endl;
		    getInstructions(  x - patternType->first, y - patternType->second, 
				      currentInstructionIndex ); // recursive call
		    
		    
		}
		
		patternType = patternType->next;
		++countPatternTypes;

	    }
	    
	} // if not at the left upper corner of the matrix
    } // getInstructions()
    
    
    
    
    void ComputeInstruction::setDebug( bool d ) {
	debug_ = d;
    }
    
} // eon

