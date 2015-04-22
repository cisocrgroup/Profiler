


/**
 * @brief Creates extensive debug and presentation output for a profiling procedure.
 */
class HTMLWriter {
public:

    HTMLWriter();

    void readConfiguration( csl::INIConfig const& iniConf );

    std::wostream& topStream();

    std::wostream& bottomStream();

    void newIteration( int nr, bool doPrintText );

    void registerToken( 
	Profiler_Token const& token,
	Evaluation_Token const& evalToken,
	std::vector< Profiler_Interpretation > const& candlist );

    void registerStatistics( std::map< std::wstring, double > const& counter,
			     GlobalProfile const& globalProfile,
			     Evaluation const& evaluation );

    /**
     * A helper for registerStatistics
     */
    void printListComparison( Evaluation::TopKListStatistics const& topKListStats, 
			      PatternContainer const& groundtruthPatterns, 
			      PatternContainer const& profiledPatterns,
			      std::wstring const& description );

    void print( std::wostream& os = std::wcout );


    
private:

    std::wstring xml_escape( std::wstring const& input ) const;

    void adjustLayout( Profiler_Token const& token, 
		       std::vector< Profiler_Interpretation > const& candlist,
		       std::wstring* color, 
		       std::wstring* border,
		       std::wstring* bgcolor,
		       std::wstring* font,
		       std::wstring* description );


    size_t nrOfTokens_;
    int iterationNumber_;
    bool doPrintText_;

    /**
     * @brief Matches each hist pattern to a list of all tokens that detect this pattern
     */
    std::map< csl::Pattern, std::vector< int > > histPatterns2tokenNumbers_;

    /**
     * @brief Matches each ocr pattern to a list of all tokens that detect this pattern
     */
    std::map< csl::Pattern, std::vector< int > > ocrPatterns2tokenNumbers_;

    std::wostringstream candBoxes_;
    std::wostringstream textStream_;
    std::wostringstream statisticStream_;

    std::wostringstream topStream_;
    std::wostringstream bottomStream_;


    /**
     * @brief In the candlists, only cands with a greater voteWeight are displayed
     *
     * limit_voteWeight_ = 0 means all cands are displayed
     */
    double limit_voteWeight_;
    
    /**
     * @brief Of all text tokens, only this initial portion is printed 
     */
    size_t limit_nrOfTextTokens_;

    bool candlistsForAllIterations_;

}; // HTMLWriter
