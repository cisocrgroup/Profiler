

class Evaluation; 

/**
 * @brief This structure collects evaluation-related information during the processing of
 *        one token. When finished, the Evaluation_Token is passed to {@link Evaluation}.
 *
 */
class Evaluation_Token {
    
public:
    friend class Profiler;
    Evaluation_Token( Profiler_Token& tok );
    
    void reset();

    void registerCandidate( Profiler_Interpretation const& cand );
    void registerNoCandidates();
    
    double& getCounter( std::wstring const& key );    
    double getCounter( std::wstring const& key ) const;    
    void initCounter( std::wstring const& key );
    std::map< std::wstring, double > const& getCounter() const;
    
    


private:
    friend class Evaluation;
    friend class HTMLWriter;
    

    bool shouldBeProfiled_;
    bool isProfiled_;

    bool shouldDetectHistPattern_;
    bool shouldDetectOCRPattern_;

    Profiler_Token const& tok_;
    Profiler_Token::Groundtruth copyOfGroundtruth_;
    
    std::map< std::wstring, double > counter_;

    std::map< csl::Pattern, std::map< std::wstring, double > > counterPerPattern_;

    /**
     * This map collects for each token the computed probability and maps it to
     * a pair of bool: the 1st indicates if the hist-trace of this token was guessed
     * correctly, the second refers to the ocr-trace. Both are considering the top interpretation only.
     *
     */
    //             prob         hist correct  ocr correct
    std::multimap< double, std::pair< bool,     bool >   > probAsIndicator_;

    /**
     * This map collects for each token the computed "psi-factor" and maps it to
     * a pair of bool: the 1st indicates if the hist-trace of this token was guessed
     * correctly, the second refers to the ocr-trace. Both are considering the top interpretation only.
     *
     */
    //             prob         hist correct  ocr correct
    std::multimap< double, std::pair< bool,     bool >   > psiAsIndicator_;
    
}; // class Evaluation_Token
	




/**
 * @brief Accumulates the evaluation data provided by each single {@link Evaluation_Token} and so provides
 *        evaluation and groundtruth data on document level.
 */
class Evaluation {
public:
    friend class HTMLWriter;
    Evaluation( Profiler const& myProfiler );

    void registerToken( Evaluation_Token& evalTok );
    
    void finish();

    PatternContainer const& getHistPatternProbabilities() const;

    PatternContainer const& getOCRPatternProbabilities() const;

    /**
     * @param which either "hist" or "ocr"
     */
    double getPrecisionByPattern( std::wstring const& which, csl::Pattern const& pat ) const;

    /**
     * @param which either "hist" or "ocr"
     */
    double getRecallByPattern( std::wstring const& which, csl::Pattern const& pat ) const;

    void writeStatistics( std::wostream& os, size_t iterationNumber ) const;

    void writeHistPatternPrecisionRecall( PatternContainer const& histPatterns, std::wostream& os = std::wcout ) const;

    void writeOCRPatternPrecisionRecall( PatternContainer const& ocrPatterns, std::wostream& os = std::wcout ) const;

    
private:
    Profiler const& myProfiler_; 

    /**
     * The "mutable" is quite a hack so we can use the [] operator and still remain const
     */
    mutable std::map< std::wstring, double > counter_;

    double& getCounter( std::wstring const& key );    
    double getCounter( std::wstring const& key ) const;    
    void initCounter( std::wstring const& key );
    
    /**
     * @brief Used to count ocr patterns using the groundtruth info
     * After the iteration this data is used to compute histPatternProbabilities_
     */
    PatternCounter ocrPatternCounter_;

    /**
     * @brief Used to count hist patterns using the groundtruth info
     * After the iteration this data is used to compute histPatternProbabilities_
     */
    PatternCounter histPatternCounter_;

    /**    
     * @brief contains the ocr pattern probabilities as computed using the groundtruth information
     */
    PatternContainer ocrPatternProbabilities_;

    /**    
     * @brief contains the hist pattern probabilities as computed using the groundtruth information
     */
    PatternContainer histPatternProbabilities_;
    

    /**
     * @brief A subset of statistics. See the description of the distinct fields
     */
    struct TopKListStatistics {
	
	/**
	 * Says how many of the top k profiled patterns are in the top k groundtruth patterns
	 */
	std::vector< double > matchesInTopK_;
	std::vector< double > shouldDetectPattern_;
	std::vector< double > guessedPattern_;
	std::vector< double > guessedPattern_good_;
	

    }; // struct TopKListStatistics
    
    TopKListStatistics topKListStats_hist_;
    TopKListStatistics topKListStats_ocr_;
    
    /**
     * @param which either "hist" or "ocr"
     */
    void createTopKListStatistics( std::wstring const& which, 
				   PatternContainer const& groundtruthPatterns, 
				   PatternContainer const& profiledPatterns,
				   TopKListStatistics* stats );

    std::map< csl::Pattern, std::map< std::wstring, double > > counterPerPattern_;
    
    bool finished_;
};

