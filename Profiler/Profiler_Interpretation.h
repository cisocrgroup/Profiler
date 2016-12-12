/****************************************************************************/
/*****  DO NEVER EVER INCLUDE THIS FILE ANYWHERE !!!!!!!   ******************/
/*****  Include Profiler.h instead.                        ******************/
/****************************************************************************/



/**
 * @brief This is a specialization of csl::DictSearch::Interpretation which adds some
 *        Profiler-related data such as probibilities. It also provides a slot for
 *        the ocr trace (or: ocr instruction) that is not (yet) part of the
 *        Interpretation type in csl.
 */
class Profiler_Interpretation : public Candidate {
public:
    Profiler_Interpretation( csl::DictSearch::Interpretation const& inter ) :
	Candidate( inter )
	{

	}

    /**
     * @name Getters
     */
    //@{

    /**
     * @brief create an alias for csl::DictSearch::Interpretation::getTrace
     */
    csl::Trace const& getHistTrace() const {
	return csl::DictSearch::Interpretation::getInstruction();
    }

    float getCombinedProbability() const {
	return combinedProbability_;
    }

    float getLangProbability() const {
	return langProbability_;
    }

    float getChannelProbability() const {
	return channelProbability_;
    }
    //@}

    /**
     * @name Setters
     */
    //@{


    void setCombinedProbability( float p ) {
	combinedProbability_ = p;
    }

    void setLangProbability( float p ) {
	langProbability_ = p;
    }

    void setChannelProbability( float p ) {
	channelProbability_ = p;
    }

    //@}

    void print( std::wostream& os = std::wcout ) const {
	os << getWord() << ":<b>{" << getBaseWord() << "+";
	getInstruction().print( os );
	os << "}+ocr";
	getOCRTrace().print( os );
	os << "</b>, lang=" << getLangProbability()
	   << ",channel=" << getChannelProbability()
	   << ",comb=" << getCombinedProbability()
	   << "(dict="<<getDictModule().getName() << ")"
	    ;
    }

private:
    float combinedProbability_;
    float langProbability_;
    float channelProbability_;

}; // class Profiler_Interpretation
