#ifndef CSL_ANNOTATEDDICTMODULE_H
#define CSL_ANNOTATEDDICTMODULE_H CSL_ANNOTATEDDICTMODULE_H

#include "./DictSearch.h"
#include "../FBDicString/FBDicString.h"
#include "../Utils/Utils.h"

namespace csl {

    /**
     * @brief This implementation of DictSearch::iDictModule is used for lexica containing historical variants,
     *        including a specification of all possible links to modern words.
     */
    class DictSearch::AnnotatedDictModule : public csl::DictSearch::AbstractDictModule {
    public:
	/**
	 * @brief A standard constructor using a filepath pointing to a dictionary.
	 *
	 * In this case, DictModule will load the dictionary and destroy it when the DictModule dies.
	 */
	AnnotatedDictModule( csl::DictSearch& myDictSearch, std::wstring const& name, std::string const& dicFile, size_t cascadeRank = 0 ) :
	    AbstractDictModule( myDictSearch, name, cascadeRank ),
	    dict_( 0 ),
	    disposeDict_( false ), // will perhaps be changed at setDict()
	    caseMode_( Global::asIs ) {

	    setDict( dicFile.c_str() );
	    setDLev( 0 );
	}


	~AnnotatedDictModule() {
	    if( dict_ && disposeDict_ ) delete( dict_ );
	}

	/**
	 * copy constructor is blocked!!!
	 */
	AnnotatedDictModule( AnnotatedDictModule const& other );


	/**
	 * @brief returns a pointer to the connected dictionary. (May be 0)
	 */
	FBDicString const* getDict() const {
	    return dict_;
	}

	/**
	 * @brief connects the configuration to a certain dictionary
	 *
	 * @param dict a const reference to an existing dictionary.
	 */
	void setDict( FBDicString const& dict ) {
	    if( dict_ && disposeDict_ ) delete dict_;
	    dict_ = &dict;
	    disposeDict_ = false;
	}

	/**
	 * @brief Loads a dictionary from the hard disk and connects it to the configuration
	 *
	 * @param dictFile a path to a file storing a dictionary of type FBDicString
	 */
	void setDict( char const* dictFile ) {
	    setDict( *( new FBDicString( dictFile ) ) );
	    disposeDict_ = true;
	}


	/**
	 * @brief Specify how to handle upper/lower case queries. See Global::CaseMode to find
	 *        out which options are available.

	 * Note that all options belongto the namespace csl::Global.
	 *
	 */
	void setCaseMode( Global::CaseMode const& cm ) {
	    caseMode_ = cm;
	}


	Global::CaseMode getCaseMode() const {
	    return caseMode_;
	}




	bool query( std::wstring const& query, DictSearch::iResultReceiver* answers ) {
	    if( getDict() ) {
		msMatch_.setFBDic( *( getDict() ) );
		msMatch_.setDistance( getDLevByWordlength( query.length() ) );
		msMatch_.setCaseMode( getCaseMode() );

		AnswerProcessor ap( *this, answers );
		return msMatch_.query( query.c_str(), ap );

	    }
	    else return false;
	}

    private:
	class AnswerProcessor : public LevFilter::CandidateReceiver {
	public:
	    AnswerProcessor( AnnotatedDictModule& myDictModule, DictSearch::iResultReceiver* answers ) :
		myDictModule_( myDictModule ),
		answers_( answers ) {
	    }

	    /**
	     * fulfilling the LevFilter::CandidateReceiver interface
	     */
	    void receive( const wchar_t *str, int levDistance, int annotation ) {
		std::wstring interpretationsString =
                        OCRCorrection::Utils::utf8(
                                (const char*) myDictModule_.getDict()->getAnnByOffset(annotation));
//		CSLLocale::string2wstring( (char*)( myDictModule_.getDict()->getAnnByOffset( annotation ) ), interpretationsString );

		size_t startPos = 0;
		size_t endPos = 0;
		do {
		    endPos = interpretationsString.find( '|', startPos + 1 );
		    if( endPos == std::wstring::npos ) endPos = interpretationsString.size();

		    Interpretation interp;
		    interp.parseFromString( interpretationsString.substr( startPos, endPos - startPos  ) );
		    interp.setLevDistance( levDistance );
		    interp.setBaseWordScore( annotation );
		    static_cast< iInterpretationReceiver* >(answers_)->receive( interp );

		    startPos = endPos + 1;
		} while( endPos < interpretationsString.size() );
	    } // receive()

	    void reset() {
		throw exceptions::cslException( "reset() not implemented in AnswerProcessor of csl::StringDictModule" );
	    }


	private:
	    AnnotatedDictModule& myDictModule_;
	    DictSearch::iResultReceiver* answers_;
	}; // class AnswerProcessor


	FBDicString const* dict_;
	bool disposeDict_;
	size_t minWordlengths_[4];
	size_t maxNrOfPatterns_;
	size_t dlev_hypothetic_;
	int priority_;
	Global::CaseMode caseMode_;

	/**
	 * @brief for conventional approximate search on dictitonary keys
	 */
	MSMatch< FW_BW > msMatch_;

    }; // class StringDictModule

} // eon


#endif
