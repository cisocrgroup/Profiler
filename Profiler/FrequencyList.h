#ifndef OCRC_FREQUENCYLIST_H
#define OCRC_FREQUENCYLIST_H OCRC_FREQUENCYLIST_H

#include<stdexcept>
#include<algorithm>
#include<stdexcept>

#include<DictSearch/DictSearch.h>
#include<Stopwatch.h>
#include<MinDic/MinDic.h>
#include<Pattern/PatternProbabilities.h>

#include <TXTReader/TXTReader.h>
#include<Document/Document.h>
#include<LevenshteinWeights/LevenshteinWeights.h>


namespace OCRCorrection {

    class FrequencyList {

	typedef unsigned long long bits64;

    public:

	/**
	 * @name Constructors/ Destructor
	 */
	//@{


	inline FrequencyList( char const* freqFile = 0, char const* weightFile = 0 ) :
	    interpFrequency_( 0 ),
	    baseWordFrequency_( 0 ),
	    patternWeights_( 0 ),
	    myOwnPatternWeights_( false ),
	    myOwnBaseWordFrequency_( false ),
	    histPatternSmoothingProb_( 0.00001 ),
	    applyStaticFreqs_( true ) {

	    //std::wcerr << "OCRC::Profiler::FrequencyList::Constructor: Enter" << std::endl;

	    if( freqFile ) {
		if( ! weightFile ) {
		    throw std::runtime_error( "OCRC::FrequencyList: For initialisation, give a freqFile and a weightFile" );
		}
		loadFromFile( freqFile, weightFile );
	    }

	    //std::wcerr << "OCRC::Profiler::FrequencyList::Constructor: Bye" << std::endl;
	}

	~FrequencyList() {
	    if( baseWordFrequency_ && myOwnBaseWordFrequency_ ) delete( baseWordFrequency_ );
	    if( interpFrequency_ ) delete( interpFrequency_ );
	    if( patternWeights_ && myOwnPatternWeights_ ) delete( patternWeights_ );
	}


	// @}
	/**
	 *
	 * @name Initialization/ Configuration
	 */
	//@{

	/**
	 * @brief Let the frequencyList use pattern probabilities you computed somewhere else.
	 *
	 * Note that FrequencyList will NOT delete this data structure in the destructor.
	 */
	void connectPatternProbabilities( csl::PatternWeights* pw ) {
	    if( patternWeights_ && myOwnPatternWeights_ ) delete( patternWeights_ );
	    myOwnPatternWeights_ = false;
	    patternWeights_ = pw;
	}


	/**
	 * @brief Let the frequencyList use baseWord frequencies you computed somewhere else.
	 *
	 * Note that FrequencyList will NOT delete this data structure in the destructor.
	 */
	void connectBaseWordFrequency( csl::MinDic< float >* bwf ) {
	    if( baseWordFrequency_ && myOwnBaseWordFrequency_ ) {
		delete( baseWordFrequency_ );
	    }
	    myOwnBaseWordFrequency_ = false;
	    baseWordFrequency_ = bwf;
	}

	void setNrOfTrainingTokens( size_t nr ) {
	    nrOfTrainingTokens_ = nr;
	}

	void setHistPatternSmoothingProb( float p ) {
	    if( ( p != OCRCorrection::LevenshteinWeights::UNDEF ) &&
		( ( p < 0 ) || ( p > 1 ) )
		) throw OCRCException( "ORC::Profiler::FrequencyList::setHistPatternSmoothingProb: out of range" );
	    histPatternSmoothingProb_ = p;
	}

	void doApplyStaticFreqs( bool b ) {
	    applyStaticFreqs_ = b;
	}


	// @}
	/**
	 *
	 * @name Lookup methods
	 */
	//@{


	/**
	 * @brief query an interpretation frequency using a csl::DictSearch::Interpretation object
	 *
	 */
	float getInterpretationFrequency( csl::Interpretation const& interp ) const {
	    size_t freq = getInterpretationCount( interp );
	    if( applyStaticFreqs_ && ( freq > 0 ) ) {
//		std::wcerr << interp.toString() << "::static: " << freq << std::endl;
		return freq;
	    }
	    else { // apply the estimation formula
//		std::wcerr << interp.toString() << "::dynamic: " << "(" << getBaseWordFrequency( interp.getBaseWord() ) << " * " << getInstructionProb( interp.getInstruction() ) << " ) = "  << ( getBaseWordFrequency( interp.getBaseWord() ) * getInstructionProb( interp.getInstruction() ) ) << std::endl;
		return ( getBaseWordFrequency( interp.getBaseWord() ) * getInstructionProb( interp.getInstruction() ) );
	    }
	}


	float getLanguageProbability( csl::Interpretation const& interp ) const {
 	//    std::wcerr << getInterpretationFrequency( interp ) << "/" << getNrOfTrainingTokens()
 	//	       << " = " << getInterpretationFrequency( interp ) / getNrOfTrainingTokens() << std::endl;
	    return getInterpretationFrequency( interp ) / getNrOfTrainingTokens();
	}





	float getBaseWordFrequency( std::wstring const& word ) const {
	    float f = 0;
	    baseWordFrequency_->lookup(word, &f);
	    if (f <= 0)
		f = 1; // TRIVIAL SMOOTHING FOR BASEWORD FREQUENCIES
	    return f;
	}

	size_t getInterpretationCount( csl::Interpretation const& interp ) const {
	    return getInterpretationCount( interpretation2String( interp ) );
	}

	size_t getInterpretationCount( std::wstring const& interp ) const {
	    int f = 0;
	    if( interpFrequency_->lookup( interp, &f ) ) {
		return (size_t)f;
	    }
	    else {
		return 0;
	    }
	}

	/**
	 * @brief returns the pattern probability as given by the patternWeights_-object, but
	 *        performs smoothing if necessary
	 */
	float getPatternProb( csl::Pattern const& pattern ) const {

	    float prob = patternWeights_->getWeight( pattern );

	    // VERY RUDIMENTARY SMOOTHING HERE !!!!!!!!!!!!!!!!!!
	    return ( prob != csl::PatternProbabilities::UNDEF ) ? prob : histPatternSmoothingProb_;
	}

	/**
	 * @brief returns the product of all pattern probabilities of the instruction.
	 *        Returns 0 if for at least one of the patterns no probability exists.
	 *
	 */
	float getInstructionProb( csl::Instruction const& instruction ) const {
	    float prob = 1;
	    float patternProb = 0;
	    // iterate through all patterns of the instruction
	    // (actually we have a list of csl::PosPattern, but this class inherits from csl::Pattern )
	    for( csl::Instruction::const_iterator it = instruction.begin(); it != instruction.end(); ++it ) {
		patternProb = getPatternProb( *it );
		if( patternProb == csl::PatternProbabilities::UNDEF ) return 0;
		prob *= getPatternProb( *it );
	    }

	    return prob;
	}


	size_t getNrOfTrainingTokens() const {
	    return nrOfTrainingTokens_;
	}


	// @}
	/**
	 *
	 * @name File load/ dump
	 */
	//@{


	void loadFromFile( char const* freqFile, char const* patternWeightFile ) {
	    if( interpFrequency_ || baseWordFrequency_ || patternWeights_ ) {
		throw OCRCException( "OCRC::FrequencyList::loadFromFile: FrequencyList seems to have been loaded before." );
	    }

	    FILE* fi = fopen( freqFile, "rb" );

	    if( ! fi ) {
		throw OCRCException( std::string( "OCRC::FrequencyList: Couldn't open file '" ) +
					  std::string( freqFile ) +
					  std::string( "' for reading." ) );
	    }

	    // read the Header
	    if( fread( &header_, sizeof( Header ), 1, fi ) != 1 ) {
		throw OCRCException( "OCRC::FrequencyList: Error while reading header" );
	    }

	    if( header_.getMagicNumber() != magicNumber_ ) {
		throw OCRCException( "OCRC::FrequencyList: Magic number comparison failed.\n" );
	    }

	    nrOfTrainingTokens_ = header_.getNrOfTrainingTokens();


	    interpFrequency_ = new csl::MinDic<>;
	    interpFrequency_->loadFromStream( fi );

	    baseWordFrequency_ = new csl::MinDic< float >;
	    baseWordFrequency_->loadFromStream( fi );


	    fclose( fi );

	    patternWeights_ = new csl::PatternWeights;
	    patternWeights_->loadFromFile( patternWeightFile );
	}

	void writeToFile( char const* freqFile, char const* patternWeightFile ) {
	    FILE* fo = fopen( freqFile, "wb" );
	    if( ! fo ) {
		throw std::runtime_error( "OCRC::FrequencyList: Couldn't open file '" +
					  std::string( freqFile ) +
					  "' for writing." );
	    }

	    // write the header
	    header_.set( *this );
	    fwrite( &header_, sizeof( Header ), 1, fo );

	    interpFrequency_->writeToStream( fo );

	    baseWordFrequency_->writeToStream( fo );

	    fclose( fo );

	    patternWeights_->writeToFile( patternWeightFile );


	};

	// @}


	class Trainer;

    private:
	static std::wstring interpretation2String( csl::Interpretation const& interp ) {
	    return interp.getWord() + L":" + interp.getBaseWord() + L"+" + interp.getInstruction().toString();
	}

	/**
	 * @brief stores the frequencies of interpretations as seen in the training
	 */
	csl::MinDic< int >*  interpFrequency_;

	/**
	 * @brief stores the frequencies of baseWords as seen in the training
	 */
	csl::MinDic< float >* baseWordFrequency_;

	/**
	 * @brief stores the probabilitites of spelling variant patterns as seen in the training
	 */
	csl::PatternWeights* patternWeights_;

	/**
	 * @brief Indicates if the patternWeights_ object was constructed by the FrequencyList itself
	 *        or if it was connected from the outside. This is important for the destructor.
	 */
	bool myOwnPatternWeights_;

	/**
	 * @brief Indicates if the patternWeights_ object was constructed by the FrequencyList itself
	 *        or if it was connected from the outside. This is important for the destructor.
	 */
	bool myOwnBaseWordFrequency_;

	/**
	 * @brief the number of tokens that were used for the training process
	 */
	bits64 nrOfTrainingTokens_;

	float histPatternSmoothingProb_;


	bool applyStaticFreqs_;


	static const bits64 magicNumber_ = 54543616;

	class Header {
	public:
	    bits64 getMagicNumber() const {
		return magicNumber_;
	    }

	    void set( FrequencyList const& freqlist ) {
		magicNumber_ = freqlist.magicNumber_;
		nrOfTrainingTokens_ = freqlist.nrOfTrainingTokens_;
	    }

	    size_t getNrOfTrainingTokens() const {
		return static_cast< size_t >( nrOfTrainingTokens_ );
	    }


	private:

	    bits64 magicNumber_;
	    bits64 maxFrequency_; // obsolete !!!
	    bits64 nrOfTrainingTokens_;
	}; // class Header

	Header header_;

    }; // class FrequencyList



} // eon

#include "./FrequencyList_Trainer.h"


#endif
