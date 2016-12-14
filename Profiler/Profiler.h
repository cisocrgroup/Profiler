#ifndef OCRC_PROFILER_H
#define OCRC_PROFILER_H OCRC_PROFILER_H

#include<string>
#include<map>
#include<vector>
#include<sstream>
#include<fstream>
#include <iomanip>

#include<TransTable/TransTable.h>
#include<DictSearch/DictSearch.h>
#include<Pattern/PatternWeights.h>
#include<Pattern/ComputeInstruction.h>
#include<Stopwatch.h>
#include<Utils/Utils.h>

#include<Document/Document.h>
#include<LevenshteinWeights/LevenshteinWeights.h>
#include<PatternCounter/PatternCounter.h>
#include<INIConfig/INIConfig.h>

#include <time.h>

#include<AbbyyXmlParser/AbbyyXmlParser.h>

#include<GlobalProfile/GlobalProfile.h>

#include<MinDic/MinDic.h>

#include "./CompoundDictModule.h"
#include "./FrequencyList.h"

namespace OCRCorrection {

	class AdaptiveLex; // fwd



    /**
     * @brief Read the {@link profiler_manual Profiler manual} to find out more about the purpose and use of the Profiler.
     *        Class Profiler analyzes an ocr-ed historical text
     *        and draws a number of conclusions and to return a profile of the text, regarding mainly:
     *        - estimated probabilities for frequent historical orthographical patterns
     *        - estimated probabilities for frequent ocr error patterns
     *
     *
     *
     * Developed at CIS, University of Munich
     * For EU-funded project IMPACT, www.impact-project.eu
     * @author Ulrich Reffle, <uli@cis.uni-muenchen.de>
     * @date 2009
     */
    class Profiler {

    public:
	enum InputFormat {TXT, ALIGN_TXT, DOC_XML, ABBYY_XML, ABBYY_XML_DIR };

	class Profiler_Token;

    private:
	class Profiler_Interpretation;

    public:
#include "./Profiler_Token.h"

	/**
	 * @brief For now, Document_t is a vector of custom tokens of type Profiler_Token
	 *
	 * At a later step it will make sense to solve this using OCRCorrection::Document.
	 */
	typedef std::vector< OCRCorrection::Profiler::Profiler_Token > Document_t;

    private:

#include "./Profiler_Interpretation.h"
	class Evaluation;
	class Evaluation_Token;
	class HTMLWriter;

#include "./Evaluation.h"
#include "./HTMLWriter.h"

	/**
	 * @brief sort operator to sort two {@Profiler_Interpretation}s according to their combined-probability values.
	 */
	class CandSort {
	public:
	    bool operator() ( Profiler_Interpretation const& first, Profiler_Interpretation const& second ) {
		return first.getCombinedProbability() > second.getCombinedProbability();
	    }
	};

    public:


	/**
	 * @brief Constructor.
	 *
	 */
	Profiler();


	~Profiler();


	/**
	 * @brief Load a configuration from a configFile
	 */
	void readConfiguration( char const* configFile );


	/**
	 * @brief Load a configuration from an existing INIConfig object
	 */
	void readConfiguration( csl::INIConfig const& iniConf );


	/**
	 * @brief Set the number of iterations for the profiler to run to @a nr.
	 *
	 *
	 */
	void setNumberOfIterations( size_t nr ) {
	    config_.nrOfIterations_ = nr;
	}

	/**
	 * @brief Enable/disable adaptive profiling
	 */
	void setAdaptive(bool adaptive) noexcept {
		config_.adaptive_ = adaptive;
	}
	bool adaptive() const noexcept {
		return config_.adaptive_;
	}
	bool writeAdaptiveDictionary() const noexcept {
		return config_.writeAdaptiveDictionary_;
	}
	const std::string& getAdaptiveDictionaryPath() const noexcept {
		return config_.adaptiveDictionaryPath_;
	}

	/**
	 * @brief Set the number of initial pages to be processed
	 *
	 *
	 */
	void setPageRestriction( size_t n ) {
	    config_.pageRestriction_ = n;
	}


	/**
	 * @brief Write an xml representation of the text profile to @a filename
	 */
	void profile2xml( char const* filename ) const;

	/**
	 * @brief Write an xml representation of the text profile to the stream @a os
	 */
	void profile2xml( std::wostream& os = std::wcout ) const;

	/**
	 * @brief Compute a profile of the text specified by @a txtFile
	 */
	void createProfile( std::string const& txtFile, InputFormat format = TXT );

	void createProfile( Document& sourceDoc );

	/**
	 * @deprecated
	 */
	PatternContainer const& getHistPatterns() const {
	    return globalProfile_.histPatternProbabilities_;
	}

	/**
	 * @deprecated
	 */
	PatternContainer const& getOCRPatterns() const {
	    return globalProfile_.ocrPatternProbabilities_;
	}

	inline void setHTMLOutFile( std::string const& htmlFile ) {
	    htmlStream_ = new std::wofstream( htmlFile.c_str() );
	    // make sure that no thousands separators are printed
	    std::locale loc = std::locale( std::locale( "" ), new std::numpunct< wchar_t >() );
	    if( ! htmlStream_->good() ) {
		delete( htmlStream_ );
		htmlStream_ = 0;
		throw OCRCException( std::string( "OCRC::Profiler:: setHTMLOutFile: Can not write to ") + htmlFile );
	    }
	}

	/**
	 * @brief print a template for a profiler config file to the specified stream
	 */
	static void printConfigTemplate( std::wostream& os );

    protected:
	/**
	 * @brief Returns a reference to the csl::DictSearch -
	 * object used by the Profiler.
	 *
	 * This is not a const reference so it can be used to change the settings.
	 */
	csl::DictSearch& getDictSearch() {
	    return dictSearch_;
	}


    private:
	// Adaptive Profiling
	void calculateCandidateSet(const Profiler_Token& t,
			csl::DictSearch::CandidateSet& candidates);
	void calculateAdaptiveCandidateSet(const Profiler_Token& t,
			csl::DictSearch::CandidateSet& candidates);
	void calculateNonAdaptiveCandidateSet(const Profiler_Token& t,
			csl::DictSearch::CandidateSet& candidates);
	void createCandidatesWithCorrection(const Profiler_Token& t,
		csl::DictSearch::CandidateSet& candidates);

	void initGlobalOcrPatternProbs(int itn);

	/**
	 * Do calculate the profile
	 */
	void doCreateProfile(Document& sourceDoc);

	/**
	 * @brief Execute one iteration of the profiling process.
	 */
	void doIteration( size_t iterationNumber, bool lastIteration = false );

	/**
	 * @brief Computes the combined probability for a candidate interpretation.
	 *        The name being slightly mis-leading, the function also changes the
	 *         values for the cand's language and channel probabilities.
	 *
	 *
	 */
	double getCombinedProb( Profiler_Interpretation& cand ) const;


	void prepareDocument( Document& sourceDocument );

	/**
	 * @brief Here the document to process is stored.
	 */
	Document_t document_;

	/**
	 * @brief This module is responsible for the language modelling.
	 *
	 * Both the class' naming and its mere existence as an entity separate from the
	 * profiler are a bit outdated. So the Profiler class itself would probably be a better place
	 * for its functionality.
	 */
	FrequencyList freqList_;

	/**
	 * @brief csl::DictSearch handles all the variant- and error-tolerant dictionary lookups.
	 */
	csl::DictSearch dictSearch_;

	/**
	 * @brief Holds all external DictModules that were added to the DictSearch object. We need
	 *        to collect these in order to destrroy them safely.
	 */
	std::vector< csl::DictSearch::iDictModule* > externalDictModules_;

	/**
	 * @brief In {@link doIteration} for each token the candidates (appearing as csl::DictSearch::Interpretation)
	 *        are copied into this vector of {@link Profiler_Interpretation}s.
	 */
	std::vector< Profiler_Interpretation > candidates_;

	/**
	 * @brief This module handles the computation of the ocrInstructions via dynamic programming.
	 */
	csl::ComputeInstruction instructionComputer_;

	/**
	 * @brief During a run through the text, histCounter_ accumulates the necessary statistics related to
	 *        historical variant patterns.
	 */
	PatternCounter histCounter_;


	/**
	 * @brief During a run through the text, histCounter_ accumulates the necessary statistics related to
	 *        ocr error patterns.
	 */
	PatternCounter ocrCounter_;


	GlobalProfile globalProfile_;

	/**
	 * This is only for Evaluation, where aut-correction is simulated
	 */
	double correctionPatternThreshold_;


	/**
	 * @brief We need this as parameter for the FrequencyList
	 */
	size_t nrOfProfiledTokens_;

	/**
	 * @brief This is a simple associative container to collect a type-frequency list of all ocr-ed tokens
	 */
	std::map< std::wstring, size_t > wOCRFreqlist_;

	/**
	 * @brief Number of chars in the ocr. Counted are only normal tokens
	 */
	size_t ocrCharacterCount_;


	/**
	 * @brief
	 */
	csl::MinDic< float >* baseWordFrequencyDic_;

	/**
	 * @brief Matches each ocr pattern to a list of all types that detect this pattern
	 */
	std::map< csl::Pattern, std::vector< std::pair< std::wstring, std::wstring > > > ocrPatterns2Types_;

	/**
	 *
	 */
	HTMLWriter htmlWriter_;

	std::wostream* htmlStream_;

	/**
	 * @brief sorts the std::pairs by their second value
	 */
	static bool sortBySecond( std::pair< csl::Pattern, double > const& a, std::pair< csl::Pattern, double > const& b ) {
	    return ( a.second > b.second ); // reverse ordering
	}


	class Configuration {
	public:
	    /**
	     * @brief The constructor initializes the parameters with zero-values that will
	     *        let the profiler do nothing. So you are bound to explicitly set
	     *        values.
	     */
	    Configuration() :
		nrOfIterations_( 0 ),
		pageRestriction_( (size_t) -1 ),
		patternCutoff_hist_( 0 ),
		patternCutoff_ocr_( 0 ),
		ocrPatternStartProb_( 0 ),
		histPatternSmoothingProb_( 0 ),
		ocrPatternSmoothingProb_( 0 ),
		resetHistPatternProbabilities_( true ),
		resetOCRPatternProbabilities_( true ),
		donttouch_hyphenation_( true ),
		donttouch_lineborders_( false),
		adaptiveDictionaryPath_(),
		writeAdaptiveDictionary_(false),
		adaptive_(false)
		{
		}

	      /**
	       * @todo add doc
	       */
	    size_t nrOfIterations_;

	    /**
	     * @brief Restrict profiling to this initial number of pages
	     */
	    size_t pageRestriction_;

	    /**
	     * @brief This number set to n means that only those hist patterns are kept which
	     *        are estimated to appear at least n times per 1000 profiled tokens
	     */
	    double patternCutoff_hist_;

	    /**
	     * @brief This number set to n means that only those ocr patterns are kept which
	     *        are estimated to appear at least n times per 1000 profiled tokens
	     */
	    double patternCutoff_ocr_;

	    /**
	     * @brief In the first iteration, all ocr patterns start with this uniform probability
	     */
	    double ocrPatternStartProb_;

	    /**
	     * @brief Unknown ocr patterns get this probability as smoothing value.
	     */
	    double histPatternSmoothingProb_;

	    /**
	     * @brief Unknown ocr patterns get this probability as smoothing value.
	     */
	    double ocrPatternSmoothingProb_;

	    /**
	     * @brief this bool value decides if, before setting new hist pattern probabilities,
	     *        the old list of probabilities is emptied. If this value is false, all
	     *        old probabilities are kept and updated only if a new value was determined.
	     */
	    bool resetHistPatternProbabilities_;

	    /**
	     * @brief this bool value decides if, before setting new ocr pattern probabilities,
	     *        the old list of probabilities is emptied. If this value is false, all
	     *        old probabilities are kept and updated only if a new value was determined.
	     */
	    bool resetOCRPatternProbabilities_;


	    bool donttouch_hyphenation_;

	    bool donttouch_lineborders_;

	    std::string adaptiveDictionaryPath_;
	    bool writeAdaptiveDictionary_;
	    bool adaptive_;

	    void print( std::wostream& os = std::wcout ) {
		os << "number of iterations:           " << nrOfIterations_ << std::endl
		   << std::endl
		   << "page restriction:               " << pageRestriction_ << std::endl
		   << std::endl
		   << "ocrPatternStartProb_:           " << ocrPatternStartProb_ << std::endl
		   << std::endl
		   << "patternCutoff_hist_:            " << patternCutoff_hist_ << std::endl
		   << "patternCutoff_ocr_:             " << patternCutoff_ocr_ << std::endl
		   << std::endl
		   << "histPatternSmoothingProb_:      " << histPatternSmoothingProb_ << std::endl
		   << "ocrPatternSmoothingProb_:       " << ocrPatternSmoothingProb_ << std::endl
		   << std::endl
		   << "resetHistPatternProbabilities_: " << resetHistPatternProbabilities_ << std::endl
		   << "resetOCRPatternProbabilities_:  " << resetOCRPatternProbabilities_ << std::endl
		   << std::endl
		   << "donttouch_hyphenation_:         " << donttouch_hyphenation_ << std::endl
		   << "donttouch_lineborders_:         " << donttouch_lineborders_ << std::endl
		   << "writeAdaptiveDictionary:        " << writeAdaptiveDictionary_ << std::endl
		   << "adaptiveDictionaryPath:         " << Utils::utf8(adaptiveDictionaryPath_)
		   << std::endl
		   << "adaptive_:                      " << adaptive_ << std::endl
		    ;
	    }

	}; // class Configuration



	/**
	 * @brief An object holding all configuration parameters
	 */
	Configuration config_;

	int verbose_;

    }; // class Profiler








} // eon


#endif
