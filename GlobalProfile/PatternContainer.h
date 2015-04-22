#ifndef OCRC_PROFILER_PATTERNCONTAINER_H
#define OCRC_PROFILER_PATTERNCONTAINER_H OCRC_PROFILER_PATTERNCONTAINER_H

#include<map>
#include<csl/Pattern/Pattern.h>
#include<csl/Pattern/PatternProbabilities.h>

#include<Exceptions.h>

namespace OCRCorrection {
    
    /**
     * @brief This class extends csl::PatternProbabilities in order to additionally store
     *        an absolute frequency for each pattern (apart from the probability value
     *        whichis stored, as before, in the regular csl::PatternProbabilities structure)
     *
     * This is makeshift design - more appropriate would be to have sth like 'iPatternSet'
     * defined in csl, so that we could here provide the functionality of PatternSet and
     * still choose our own way to store the data appropriately. The way it is now we are bound
     * to inherit the std::map< csl::pattern, double > from csl::PatternSet and have to set up an
     * additional container for the absolute frequencies. 
     *
     * @author Ulrich Reffle<uli@cis.uni-muenchen.de>
     * @year 2009
     */
    class PatternContainer : public csl::PatternProbabilities {
    public:
	
	/**
	 * overrides method in csl::PatternProbabilities
	 */
	virtual void clear() {
	    csl::PatternProbabilities::clear();
	    absoluteFreqs_.clear();
	}

	
	/**
	 * overrides method in csl::PatternProbabilities
	 */
	virtual void clearExplicitWeights() {
	    csl::PatternProbabilities::clearExplicitWeights();
	    absoluteFreqs_.clear();
	}


	/**
	 * @brief set a new weight/ probability and absolute frequency for a pattern
	 */
	void setWeight( const csl::Pattern& pattern, double relFreq, double absFreq ) {
	    csl::PatternProbabilities::setWeight( pattern, relFreq );
	    absoluteFreqs_[pattern] = absFreq;
	}



	/**
	 * @brief returns a const reference to the structure holding the absolute freqs
	 *
	 */
	std::map< csl::Pattern, double > const& getAbsoluteFreqs() const {
	    return absoluteFreqs_;
	}	

	/**
	 * @brief writes xml output to the specified file
	 */
	void writeToXML( char const* xmlFile ) const {
	    std::wofstream fo;
	    fo.imbue( csl::CSLLocale::Instance() );
	    fo.open( xmlFile );
	    if( ! fo ) {
		throw OCRCException( "OCRC::PatternContainer::writeToXML: Could not open pattern file for writing" );
	    }
	    writeToXML( fo );
	    fo.close();
	}

	/**
	 * @brief writes xml output to the specified stream
	 *
	 * Note that the Profiler currently prints the container from scratch on its own.
	 */
	void writeToXML( std::wostream& os = std::wcout ) const {
	    time_t t = time(NULL);
	    
	    os << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl
	       << "<patternProbabilities>" << std::endl
	       << "<head>" << std::endl
	       << "<created_at>" << asctime(localtime(&t)) << "</created_at>" << std::endl
	       << "</head>" << std::endl;
	    
	    writeToXML_patternsOnly( os );

	    os << "</patternProbabilities>" << std::endl;
	}
	
	/**
	 * @brief Writes only the <pattern ... /> containers. This can be used if the patterns
	 *        are to be included in some other xml format.
	 *
	 * Note that the Profiler currently prints the container from scratch on its own.
	 */
	void writeToXML_patternsOnly( std::wostream& os = std::wcout ) const {
	    std::vector< std::pair< csl::Pattern, double > > patternsSorted;
	    sortToVector( &patternsSorted );

	    for( std::vector< std::pair< csl::Pattern, double > >::const_iterator it = patternsSorted.begin(); 
		 it != patternsSorted.end();
		 ++it ) {
		os << "<pattern left=\"" << it->first.getLeft() << "\" right=\"" << it->first.getRight() << "\""
		   << " pat_string=\"" << it->first.getLeft() << "_" << it->first.getRight() << "\""
		   << " relFreq=\"" << getWeight( it->first )  << "\""
		   << " absFreq=\"" << ( absoluteFreqs_.find( it->first ) )->second << "\""
		   << "/>" 
		   << std::endl;
	    }
	}

	/**
	 * @brief Prints a text representation of the pattern set. Mostly for debug.
	 * This overrides t emethod in csl::PatternProbabilities
	 */
	void print( std::wostream& os ) const {
	    time_t t = time(NULL);
	    
	    std::vector< std::pair< csl::Pattern, double > > patternsSorted;
	    sortToVector( &patternsSorted );

	    for( std::vector< std::pair< csl::Pattern, double > >::const_iterator it = patternsSorted.begin(); 
		 it != patternsSorted.end();
		 ++it ) {
		os << it->first.getLeft() << "_" << it->first.getRight() 
		   << "#" << getWeight( it->first )
		   << "#" << ( absoluteFreqs_.find( it->first ) )->second
		   << std::endl;
	    }
	}

	/**
	 * @brief When we print the structure, we usually want the data sorted according to absolute frequencies.
	 *        This function fills and sorts a given vector in that sense.
	 *
	 * @param[out] outVector A vector of std::pair< csl::Pattern, double >, typically (but not necessarily) empty in advance.
	 *             sortToVector() will write all patterns and their absolute freq uency to that vector, sorted by absolute freq.
	 */
	void sortToVector( std::vector< std::pair< csl::Pattern, double > >* outVector ) const {
	    if( ! outVector->empty() ) throw OCRCException( "OCRC::PatternContainer::sortToVector: output vector not empty." );
	    for( std::map< csl::Pattern, double >::const_iterator it = absoluteFreqs_.begin(); it != absoluteFreqs_.end(); ++it ) {
		outVector->push_back( *it );
	    }
	    std::sort( outVector->begin(), outVector->end(), sortByAbsolute );
	}

	/**
	 * @brief sorts the std::pairs according to their absolute freqs
	 */
	static bool sortByAbsolute( std::pair< csl::Pattern, double > const& a, std::pair< csl::Pattern, double > const& b ) {
	    return ( a.second > b.second ); // reverse ordering
	}
	
    private:
	/**
	 * @brief An additiomal data member storing the absolute frequencies.
	 *
	 * The general class description at the top of this page says some words on this
	 * sub-optimal design.
	 */
	std::map< csl::Pattern, double > absoluteFreqs_;
    };


} // eon

#endif
