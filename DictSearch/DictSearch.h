#ifndef CSL_DICTSEARCH_H
#define CSL_DICTSEARCH_H CSL_DICTSEARCH_H

#include<ostream>
#include<vector>
#include<map>
#include<utility>
#include<Global.h>
#include<MinDic/MinDic.h>
#include<FBDic/FBDic.h>
#include<MSMatch/MSMatch.h>
#include<LevFilter/LevFilter.h>
#include<Vaam/Vaam.h>
#include<Val/Val.h>
#include<INIConfig/INIConfig.h>

// "AnnotatedDictModule.h" is included at the end of this header file!

namespace csl {

    /**
     * @brief csl::DictSearch is a combined interface for approximate dictionary lookup in the context of historical language.
     *
     * A special @link dictSearch_manual DictSearch Manual@endlink page was prepared for this module. You might want
     * to have a look at it before reading this class reference.
     *
     * csl::DictSearch is a combined interface for a number of modules of the package. The scenario is that the users
     * specify a modern as well as a historical dictionary to perform exact or approximate dictionary lookups.
     * A third component is what we call the "hypothetic dictionary", containing all orthographical variants that
     * can possibly be derived from some word of the modern dictionary and the application of some "orthographical
     * variant patterns". These patterns are simple rewrite rules and can be specified by the user.
     *
     * For a query word @c w the users receive as answer a set of words, containing exact and approximate matches for
     * either of the three dictionaries. In the usual application this answer set is understood as set of correction
     * candidates for a (probably garbled) token @c w. The task of ranking these candidates is explicitly out of the scope of
     * csl::DictSearch: To decide for a correction candidate, various other techniques for channel and language modelling have
     * to be taken into account.
     *
     * @author Ulrich Reffle<uli@cis.uni-muenchen.de>, 2008
     */
    class DictSearch {
    public:
	typedef FBDic<> Dict_t;
	typedef MinDic<> VaamDict_t;

	class DictModule; // forward declaration
        class AnnotatedDictModule;

	static const size_t INFINITE = (size_t)-1;

	// forward declaration
	class iDictModule;

	/**
	 * @brief This is a specialisation of csl::Interpretation, tailored to store additional
	 *        information added by DictSearch
	 */
	class Interpretation : public csl::Interpretation {
	public:
	    Interpretation() : dictModule_( 0 ) {}

	    Interpretation( csl::Interpretation const& interpretation, iDictModule const& dm ) :
		csl::Interpretation( interpretation ),
		dictModule_( &dm )
		{}


	    /**
	     *
	     * set up a comparison based on the sum of levenshtein or pattern edits.
	     * Give the levenshtein operations a marginally higher punishment, so that
	     * if the sums are equal, the one with less lev. operations and more pattern
	     * operations will win.
	     */
	    bool operator<( Interpretation const& other ) const {

		// std::wcout << "op< this=" << this << std::endl;
		// std::wcout << "op< other=" << &other << std::endl;
		// std::wcout << "op< this=" << this->toString() << std::endl;
		// std::wcout << "op< other=" << other.toString() << std::endl;

		// float compareSumOfOperations =
		//     ( getInstruction().size() + getLevDistance() * 1.01 ) -
		//     ( other.getInstruction().size() + other.getLevDistance() * 1.01 );

		int compareSumOfOperations =
		    ( getInstruction().size() + getLevDistance()  ) -
		    ( other.getInstruction().size() + other.getLevDistance() );

		if( compareSumOfOperations == 0 ) {
		    compareSumOfOperations = getLevDistance() - other.getLevDistance();
		}
		if( compareSumOfOperations == 0 ) {
		    compareSumOfOperations = other. getDictModule().getPriority() - getDictModule().getPriority();
		}
		if( compareSumOfOperations == 0 ) {
		    compareSumOfOperations = getWord().compare( other.getWord() );
		}

		// std::wcout
		//     << "BLA:" << getInstruction().size() << L" + " << getLevDistance() << L" - "
		//     << other.getInstruction().size() << L" + " << other.getLevDistance() << std::endl
		//     <<"sum=" << compareSumOfOperations << std::endl
		//     ;

		if     ( compareSumOfOperations < 0 ) return true;
		else if( compareSumOfOperations > 0 ) return false;
		else {
		    return false;
		}
	    }


	    /**
	     * @brief specify the dictModule which the interpretation comes from
	     * @see dictModule_
	     */
	    void setDictModule( iDictModule const& dictModule ) { dictModule_ = &dictModule; }

	    /**
	     * @brief get a pointer to the dictModule which the interpretation came from
	     * @see dictModule_
	     */
	    iDictModule const& getDictModule() const { return *dictModule_; }

	    void print( std::wostream& os = std::wcout ) const {
		csl::Interpretation::print( os );
		os << "(" << getDictModule().getName() << ")";
	    }

	    std::wstring toString() const {
		return csl::Interpretation::toString() + L"(" + getDictModule().getName() + L")";
	    }

	private:
	    /**
	     * @brief A pointer to the dictModule which the interpretation came from
	     *
	     */
	    iDictModule const* dictModule_;
	}; // class Inter

	/**
	 * @brief Whoever wants to receive results from DictSearch, has to implement
	 *        these two interfaces.
	 */
	class iResultReceiver : public csl::LevFilter::CandidateReceiver,
				public csl::iInterpretationReceiver {

	public:
	    virtual void setCurrentDictModule( iDictModule const& dm ) = 0;

	    /**
	     *
	     */
	    virtual void handleWordBoundaries( bool b ) = 0;

	}; // class iResultReceiver






	/**
	 * @brief CandidateSet is a simple container to collect candidates/interpretations
	 *        from either a Vaam or a csl::MSMatch object.
	 */
	class CandidateSet : public iResultReceiver
	{
	public:
	    typedef csl::DictSearch::Interpretation Interpretation_t;
	    typedef std::vector< Interpretation_t >::iterator iterator;
	    typedef std::vector< Interpretation_t >::const_iterator const_iterator;

	    /**
	     * @brief This is to fulfill the csl::LevFilter::CandidateReceiver interface
	     */
	    void receive( const wchar_t* str, int levDistance, int annotation ) {
		csl::Interpretation cslInt;
		cslInt.setWord( str );
		cslInt.setBaseWord( str );
		cslInt.setBaseWordScore( annotation );
		cslInt.setLevDistance( levDistance );
		receive( cslInt );
	    }

	    /**
	     * @brief This is to fulfill the csl::InterpretationReceiver interface
	     */
	    void receive( csl::Interpretation const& vaam_interpretation ) {
		myVector_.push_back( csl::DictSearch::Interpretation( vaam_interpretation, *currentDictModule_ ) );
	    }

	    void handleWordBoundaries( bool b ) {
	    }


	    /**
	     * @see currentDictModule_
	     */
	    void setCurrentDictModule( iDictModule const& dm ) { currentDictModule_ = &dm; }



	    /**
	     * @brief alias for clear()
	     * @see clear()
	     */
	    void reset() {
		clear();
	    }

	    /**
	     * @brief method defined as usual for containers, e.g. in the std library
	     */
	    void clear() {
		myVector_.clear();
	    }

	    /**
	     * @brief method defined as usual for containers, e.g. in the std library
	     */
	    iterator begin() {
		return myVector_.begin();
	    }

	    /**
	     * @brief method defined as usual for containers, e.g. in the std library
	     */
	    const_iterator begin() const {
		return myVector_.begin();
	    }

	    /**
	     * @brief method defined as usual for containers, e.g. in the std library
	     */
	    iterator end() {
		return myVector_.end();
	    }

	    /**
	     * @brief method defined as usual for containers, e.g. in the std library
	     */
	    const_iterator end() const {
		return myVector_.end();
	    }

	    /**
	     * @brief method defined as usual for containers, e.g. in the std library
	     */
	    size_t size() const {
		return myVector_.size();
	    }

	    /**
	     * @brief method defined as usual for containers, e.g. in the std library
	     */
	    bool empty() const {
		return myVector_.empty();
	    }

	    /**
	     * @brief method defined as usual for containers, e.g. in the std library
	     */
	    Interpretation_t const& at( size_t i ) const {
		return myVector_.at( i );
	    }

	    /**
	     * Remove all interpretations from the candidate set that match the
	     * given predicate p.
	     */
	    template<class P>
	    void discard_if(P p) {
		    auto e = std::remove_if(myVector_.begin(), myVector_.end(), p);
		    myVector_.erase(e, myVector_.end());
	    }


	private:
	    /**
	     * @brief All candidates that are added via one of the receive()-methods will get this
	     *        dictModule_ as source.
	     *
	     * This is somewhat messy. Certainly it is not thread-safe!!!
	     */
	    iDictModule const* currentDictModule_;
	    std::vector< csl::DictSearch::Interpretation > myVector_;

	}; // class CandidateSet






	/**
	 * @brief All classes that are supposed to contribute to the answer set of a DictSearch query
	 * have to implement this interface.
	 * The method DictSearch::addExternalDictModule() allows to add such objects to the process.
	 *
	 * One implementation of this interface is DictSearch's sub-class DictModule
	 */
	class iDictModule {
	public:

	    iDictModule( size_t cascadeRank = 0 ) :
		priority_( 1 ),
		cascadeRank_( cascadeRank ) {
	    }

	    /**
	     * @brief A virtual destructor for a virtual class
	     */
	    virtual ~iDictModule() {
	    }

	    /**
	     * @brief Allows to query the iDctModule with some string. The iDictModule is expected
	     * to add all its answers/ interpretations to the CandidateSet that is passed.
	     *
	     * @return true iff at least one answer was found
	     */
	    virtual bool query( std::wstring const& query, iResultReceiver* answers ) = 0;



	    /**
	     * @brief returns a string to identify the DictModule
	     */
	    virtual std::wstring const& getName() const = 0;


	    size_t getCascadeRank() const {
		return cascadeRank_;
	    }

	    /**
	     * @brief Set a value in the range [0;100]. This value decides which candidate to rank higher if the other
	     *        criteria (nr of hist/ocr patterns) are equal.
	     */
	    void setPriority( int p ) {
		if( ( p < 0 ) || ( p > 100 ) ) throw csl::exceptions::LogicalError( "csl::DictSearch::DictModule::setPriority: value ranges from 0 to 100" );
		priority_ = p;
	    }

	    /**
	     * @brief returns a value between 0 and 100 to indicate the DictModule's priority as compared to others
	     * This is used for a rough sorting order of all answers.
	     */
	    int getPriority() const {
		return priority_;
	    }


	private:
	    int priority_;
	    size_t cascadeRank_;
	}; // class iDictModule



	class AbstractDictModule : public iDictModule {
	public:

	    AbstractDictModule( DictSearch& myDictSearch, std::wstring const& name, size_t cascadeRank ) :
		iDictModule( cascadeRank ),
		name_( name ),
		myDictSearch_( myDictSearch ),
		maxNrOfPatterns_( 0 ),
		dlev_hypothetic_( 0 )
		{

		setDLev( 0 );
	    }

	    /**
	     * @name Setters
	     */
	    //@{


	    /**
	     * @brief Sets a static Levenshtein distance threshold for approximate lookup.
	     *
	     * If you set a value other than 0 here (which is the default value), the lookup algorithm
	     * will, for a query word w, return all words w' of the dictionary where the levenshtein distance
	     * between w and w' does not exceed the given boundary.
	     */
	    void setDLev( size_t dLev ) {
		if( dLev > 3 ) throw exceptions::LogicalError( "csl::DictSearch::ConfigLookup::setDLevStatic: choose a threshold in the range [0..3]" );
		// set minWordLengths to 0 for all levDistances smaller or equal to dLev
		for( size_t i = 1; i <= 3; ++i ) {
		    minWordlengths_[i] = ( i <= dLev )? 0 : INFINITE;
		}
	    }

	    /**
	     * @brief configures the approximate lookup in a way that the lev-distance bound (see setDLev() ) depends on
	     * the word length of the query word.
	     *
	     * @param wordlength_1 minimal word length for a query to be searched for with distance bound 1
	     * @param wordlength_2 minimal word length for a query to be searched for with distance bound 2
	     * @param wordlength_3 minimal word length for a query to be searched for with distance bound 3
	     *
	     * A search with distance bound n naturally also returns candidates with distances smaller than n. For example,
	     * searching with distance 2 will in general return candidates with distance 0,1,2.
	     * That's why it makes no sense to assign, e.g. wordlength_2=8 and wordlength_1=6. Whenever
	     * wordlength_i is smaller than wordlength_i-1 an exception is thrown.
	     */
	    void setDLevWordlengths( size_t wordlength_1, size_t wordlength_2, size_t wordlength_3 ) {
		if( wordlength_2 < wordlength_1 || wordlength_3 < wordlength_2 )
		    throw exceptions::LogicalError( "csl::DictSearch::ConfigLookup::setDLevWordlengths: minimal wordlength for distance i must be greater than for distance i-1" );

		minWordlengths_[1] = wordlength_1;
		minWordlengths_[2] = wordlength_2;
		minWordlengths_[3] = wordlength_3;
	    }

	    /**
	     * @brief Sets sensible standard values for word-length dependent distance bounds for approximate search
	     *
	     * - lev. distance 1 for wordslengths 1-6
	     * - lev. distance 2 for words of length 7 and longer
	     *
	     * @see setDLevWordlengths( size_t wordlength_1, size_t wordlength_2, size_t wordlength_3 ), setDLev
	     */
	    void setDLevWordlengths() {
		setDLevWordlengths( 1, 7, INFINITE );
	    }

	    /**
	     * @brief sets a new value for the upper bound of applied variant patterns
	     */
	    void setMinNrOfPatterns( size_t min ) {
		minNrOfPatterns_ = min;
	    }

	    /**
	     * @brief sets a new value for the upper bound of applied variant patterns
	     */
	    void setMaxNrOfPatterns( size_t max ) {
		maxNrOfPatterns_ = max;
	    }

	    /**
	     * @brief sets a new value for the upper bound of applied variant patterns
	     */
	    void setDLevHypothetic( size_t dlev ) {
		dlev_hypothetic_ = dlev;
	    }



	    //@}

	    /**
	     * @name Getters
	     */
	    //@}

	    DictSearch& getMyDictSearch() {
		return myDictSearch_;
	    }

	    /**
	     * @brief get the lev-distance threshold given a word length
	     */
	    size_t getDLevByWordlength( size_t wordLength ) const {
		for( int i = 3; i >= 1; --i ) {
		    if( ( minWordlengths_[i] != INFINITE ) && ( minWordlengths_[i] <= wordLength ) ) return i;
		}
		return 0;
	    }


	    Global::CaseMode getCaseMode() const {
		return caseMode_;
	    }

	    /**
	     * @brief returns the current setting for the upper bound of applied variant patterns
	     */
	    size_t getMaxNrOfPatterns() const {
		return maxNrOfPatterns_;
	    }


	    size_t getDLevHypothetic() const {
		return dlev_hypothetic_;
	    }


	    /**
	     * @brief returns the DictModule's  name
	     */
	    std::wstring const& getName() const {
		return name_;
	    }


	private:
	    std::wstring name_;
	    DictSearch& myDictSearch_;
	    size_t minWordlengths_[4];
	    size_t minNrOfPatterns_;
	    size_t maxNrOfPatterns_;
	    size_t dlev_hypothetic_;

	    Global::CaseMode caseMode_;
	}; // class AbstractDictModule



	/**
	 * @brief An object of this type handles one dictionary that is to be used with DictSearch, and allows
	 *        search with approximate search and approximate/hypothetic search.
	 *
	 */
	class DictModule : public AbstractDictModule {
	public:
	    /**
	     * @brief A standard constructor using a filepath pointing to a dictionary.
	     *
	     * In this case, DictModule will load the dictionary and destroy it when the DictModule dies.
	     */
	    DictModule( DictSearch& myDictSearch, std::wstring const& name, std::string const& dicFile, size_t cascadeRank = 0 ) :
		AbstractDictModule( myDictSearch, name, cascadeRank ),
		dict_( 0 ),
		disposeDict_( false ), // will perhaps be changed at setDict()
		caseMode_( Global::asIs )
		{

		    setDict( dicFile.c_str() );
		    setDLev( 0 );
		}

	    /**
	     * @brief A standard constructor using a pointer to a dictionary-object in memory.
	     *
	     * Of course, in this case, DictModule will leave the dictionary-object alive when DictModule dies.
	     */
	    DictModule( DictSearch& myDictSearch, std::wstring const& name, Dict_t const& dicRef, size_t cascadeRank = 0 ) :
		AbstractDictModule( myDictSearch, name, cascadeRank ),
		dict_( 0 ),
		disposeDict_( false ),  // will perhaps be changed at setDict()
		caseMode_( Global::asIs ) {
		setDict( dicRef );
		setDLev( 0 );
	    }

	    ~DictModule() {
		if( dict_ && disposeDict_ ) delete( dict_ );
	    }

	    /**
	     * copy constructor is blocked!!!
	     */
	    DictModule( DictModule const& other );


	    /**
	     * @brief returns a pointer to the connected dictionary. (May be 0)
	     */
	    Dict_t const* getDict() const {
		return dict_;
	    }

	    /**
	     * @brief connects the configuration to a certain dictionary.
	     *        This dictionary will NOT be deleted in the destructor.
	     *
	     * @param dict a const reference to an existing dictionary.
	     */
	    void setDict( Dict_t const& dict ) {
		if( dict_ && disposeDict_ ) delete dict_;
		dict_ = &dict;
		disposeDict_ = false;
	    }

	    /**
	     * @brief Loads a dictionary from the hard disk and connects it to the configuration.
	     *        This dictionary will NOT be deleted in the destructor.
	     *
	     * @param dictFile a path to a file storing a dictionary of type Dict_t
	     */
	    void setDict( char const* dictFile ) {
		setDict( *( new Dict_t( dictFile ) ) );
		disposeDict_ = true;
	    }


	    /**
	     * @brief Specify how to handle upper/lower case queries. See Global::CaseMode to find
	     *        out which options are available.

	     * Note that all options belong to the namespace csl::Global.
	     *
	     */
	    void setCaseMode( Global::CaseMode const& cm ) {
		caseMode_ = cm;
	    }


	    Global::CaseMode getCaseMode() const {
		return caseMode_;
	    }




	    bool query( std::wstring const& query, iResultReceiver* answers ) {
		bool foundAnswers = false;
		// std::wcerr << "query: " << query << "\n";
		if( getDict() ) {
		    getMyDictSearch().msMatch_.setFBDic( *( getDict() ) );
		    getMyDictSearch().msMatch_.setDistance( getDLevByWordlength( query.length() ) );
		    getMyDictSearch().msMatch_.setCaseMode( getCaseMode() );

		    foundAnswers = getMyDictSearch().msMatch_.query( query.c_str(), *answers );

			// std::wcerr << "maxnrofpatterns: " << getMaxNrOfPatterns() << "\n";
		    if( getMaxNrOfPatterns() > 0 ) {
			if( ! getMyDictSearch().hasHypothetic() ) {
			    throw exceptions::cslException( "csl::DictSearch::DictModule::query: DictSearch has no Vaam ready" );
			}
			// std::wcerr << "getDLevHypothetic: " << getDLevHypothetic() << "\n";
			if( getDLevHypothetic() == 0 ) { // use val
			    getMyDictSearch().val_->setBaseDic( dict_->getFWDic() );
			    getMyDictSearch().val_->setMinNrOfPatterns( 1 ); // get only strictly hypothetic matches
			    getMyDictSearch().val_->setMaxNrOfPatterns( getMaxNrOfPatterns() );
			    getMyDictSearch().val_->setCaseMode( getCaseMode() );

			    foundAnswers =
				getMyDictSearch().val_->query( query, answers )
				|| foundAnswers;
			}
			else { // use vaam
			    getMyDictSearch().vaam_->setBaseDic( dict_->getFWDic() );
			    getMyDictSearch().vaam_->setMinNrOfPatterns( 1 ); // get only strictly hypothetic matches
			    getMyDictSearch().vaam_->setMaxNrOfPatterns( getMaxNrOfPatterns() );
			    getMyDictSearch().vaam_->setDistance( getDLevHypothetic() );
			    getMyDictSearch().vaam_->setCaseMode( getCaseMode() );

			    foundAnswers =
				getMyDictSearch().vaam_->query( query, answers )
				|| foundAnswers;
			}
		    }
		}
		return foundAnswers;
	    }

	private:
	    Dict_t const* dict_;
	    bool disposeDict_;
	    Global::CaseMode caseMode_;
	}; // class DictModule


	/**
	 * @name Constructor/ Destructor
	 */
	//@{
	DictSearch();
	~DictSearch();
	//@}

	/**
	 * @name Configuration
	 */
	//@{


	/**
	 * @brief Load a configuration from a configFile
	 *
	 *
	 */
	void readConfiguration( char const* configFile );


	/**
	 * @brief Load a configuration from an existing INIConfig object
	 */
	void readConfiguration( INIConfig const& iniConf );


	/**
	 * @brief initialise the hypothetic dictionary.
	 * @param patternFile A file containing a list of rewrite patterns. Consult csl::PatternSet for details
	 *
	 * At the moment you can NOT change the pattern set afterwards.
	 * Please notify the developers if you need such a feature.
	 */
	void initHypothetic( char const* patternFile );


	/**
	 * Add a new managed DictionaryModule
	 */
	iDictModule& addDictModule(iDictModule& module);

	/**
	 * Search for a dictionary module by name
	 */
	iDictModule* findDictModule(const std::wstring& name) const;

	/**
	 * Get the maximum cascade rank of dictionary modules
	 */
	size_t getMaxCascadeRank() const;

	/**
	 * Get the maximum cascade rank of dictionary modules
	 */
	size_t getMinCascadeRank() const;

	DictModule& addDictModule( std::wstring const& name, std::string const& dicFile, size_t cascadeRank = 0 );

        DictModule& addDictModule( std::wstring const& name, Dict_t const& dicRef, size_t cascadeRank = 0 );

        /**
         * @brief Adds an AnnotatedDictModule to DictSearch. This module will be deleted by DictSearch, so
         *        be sure to create it on the heap and not to delete it yourself.
         * @param newDM
         * @return
         */
        AnnotatedDictModule& addAnnotatedDictModule( AnnotatedDictModule* newDM );

	/**
	 * @brief
	 */
	void addExternalDictModule( iDictModule& extModule );

	/**
	 * @brief A simple check if any DictModules are activated. This is useful to test
	 *        in client programs and perhaps to give a warning,
	 */
	bool hasDictModules() const;

	bool hasHypothetic() const;

	//@} // END Configuration methods

	/**
	 * @name Lookup
	 */
	//@{

	/**
	 *
	 * @param[in] query
	 * @param[out] answers
	 *
	 * @return true iff at least one answer was found
	 */
	bool query( std::wstring const& query, iResultReceiver* answers );

	//@} // END Lookup methods

    private:
	Vaam< VaamDict_t >* vaam_;
	Val* val_;

	/**
	 * @brief holds the conventional DictModules as addedd by addDictModule()
	 *
	 * These iDictModules are destroyed when the DictSearch object is destroyed.
	 */
	std::vector< iDictModule* > internalDictModules_;

	/**
	 * @brief holds the external DictModules as addedd by addExternalDictModule()
	 *
	 * Whoever added these modules must take care of their deletion.
	 */
	std::vector< iDictModule* > externalDictModules_;

	/**
	 * @brief Holds pointers to all conventional and external
	 */
	std::multimap< size_t, iDictModule* > allDictModules_;


	VaamDict_t dummyDic;
	MSMatch< FW_BW > msMatch_;

    }; // class DictSearch


} // namespace csl

#include "./AnnotatedDictModule.h"


namespace std {
    inline wostream& operator<<( wostream& os, csl::DictSearch::Interpretation const& obj ) {
	obj.print( os );
	return os;
    }
}


#endif
