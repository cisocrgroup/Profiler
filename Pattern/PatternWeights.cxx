#include "./PatternWeights.h"
#include <cerrno>
#include <sstream>
#include<algorithm>

namespace csl {

    PatternWeights::PatternWeights() : smartMerge_( false ) {
    }

    void PatternWeights::clear() {
	patternWeights_.clear();
	defaultWeights_.clear();
    }

    void PatternWeights::clearExplicitWeights() {
	patternWeights_.clear();
    }


    double PatternWeights::getWeight( const csl::Pattern& pattern ) const {
	// try and find it in the list of explicitly defined weights
	std::map< csl::Pattern, double >::const_iterator it = patternWeights_.find( pattern );
	if( it != patternWeights_.end() )
	    return it->second;

	// Smart merge filter
	if( smartMerge_ ) {
	    if( pattern.getLeft().size() > 0 && ( pattern.getRight().find( pattern.getLeft() ) != std::wstring::npos )  ) return UNDEF;
	    if( pattern.getRight().size() > 0 && ( pattern.getLeft().find( pattern.getRight() ) != std::wstring::npos )  ) return UNDEF;
	}

	// otherwise, return default value for the respective pattern type (which might be UNDEF as well)
	return getDefault( PatternType( pattern.getLeft().size(), pattern.getRight().size() ) );
    }

    void PatternWeights::setWeight( const csl::Pattern& pattern, double weight ) {
	if( weight == UNDEF ) {
	    patternWeights_.erase( pattern );
	}
	else patternWeights_[ pattern ] = weight;
    }

    double PatternWeights::getDefault( PatternType const& patternType ) const {
	// look for a default setting suitable for the pattern's type (that is, for its ratio lefthand-size/righthand-size)
	std::map< PatternType, double >::const_iterator defaultIt = defaultWeights_.find( patternType );
	if( ( defaultIt != defaultWeights_.end() ) ) {
	    return defaultIt->second;
	}

	return UNDEF;
    }

    void PatternWeights::setDefault( PatternType const& patternType, double weight ) {
	if( weight == UNDEF ) {
	    defaultWeights_.erase( patternType );
	}
	else defaultWeights_[ patternType ] = weight;
    }


    void PatternWeights::setSmartMerge( bool t ) {
	smartMerge_ = t;
    }

    void PatternWeights::loadFromFile( const char* patternFile ) {
	std::wifstream fi;
	//fi.imbue( CSLLocale::Instance() );
	fi.open( patternFile );
	if( ! fi ) {
	    std::string message =
		std::string( "PatternWeights::Could not open pattern file: " ) +
		patternFile;

	    throw exceptions::badFileHandle( message );
	}

	std::wstring line;

	size_t patternCount = 0;
	while( getline( fi, line ).good() ) {
	    size_t delimPos = line.find( Pattern::leftRightDelimiter_ );
	    size_t weightDelimPos = line.find( L'#' );
	    if( ( delimPos == std::wstring::npos  ) || ( weightDelimPos == std::wstring::npos  ) ) {
		throw exceptions::badInput( "PatternWeights: Invalid line in pattern file" );
	    }


	    // std::wcout << "left side is " << line.substr( 0, delimPos ) << std::endl;
	    // std::wcout << "right side is " << line.substr( delimPos + 1, weightDelimPos - delimPos -1 ) << std::endl;
	    // std::wcout << "weight is " << line.substr( weightDelimPos+1 ).c_str() << std::endl;

	    double weight = 0;
	    try {
                std::wstringstream wstrstr(line.substr(weightDelimPos + 1));
                wstrstr >> weight;
                //weight = csl::CSLLocale::string2number< double >( line.substr( weightDelimPos+1 ) );
	    } catch( std::exception& exc ) {
		throw exceptions::badInput( "csl::PatternWeights: Could not parse double number." );
	    }

	    patternWeights_[ Pattern( line.substr( 0, delimPos ),
				      line.substr( delimPos + 1, weightDelimPos - delimPos - 1 ) ) ]
		= weight;


	    ++patternCount;
	}
	if( errno == EILSEQ ) { // catch encoding error
	    throw exceptions::badInput( "csl::PatternWeights: Encoding error in input sequence." );
	}

	//std::wcerr << "csl::PatternWeights: Loaded " << patternCount << L" patterns."<< std::endl;

	// std::wofstream fo;
	// fo.imbue( CSLLocale::Instance() );
	// fo.open( "/tmp/patterns.xml" );

	// writeToXML( fo );
	// fo.close();
    } // loadFromFile

    void PatternWeights::writeToFile( const char* patternFile ) const {
	std::wofstream fo;
//	fo.imbue( CSLLocale::Instance() );
	fo.open( patternFile );
	if( ! fo ) {
	    throw exceptions::badFileHandle( "csl::PatternWeights: Could not open pattern file for writing" );
	}

	std::vector< std::pair< csl::Pattern, double > > histPatternCountSorted;
	sortToVector( &histPatternCountSorted );

	for( std::vector< std::pair< csl::Pattern, double > >::const_iterator it = histPatternCountSorted.begin();
	     it != histPatternCountSorted.end();
	     ++it ) {
	    fo         << it->first.getLeft() << Pattern::leftRightDelimiter_ << it->first.getRight() << "#" << it->second << std::endl;
	    //std::wcout << it->first.getLeft() << Pattern::leftRightDelimiter_ << it->first.getRight() << "#" << it->second << std::endl;
	}
	fo.close();
    }

    void PatternWeights::writeToXML( std::wostream& os ) const {
	time_t t = time(NULL);

	os << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl
	   << "<patternWeights>" << std::endl
	   << "<head>" << std::endl
	   << "<created_at>" << asctime(localtime(&t)) << "</created_at>" << std::endl
	   << "</head>" << std::endl;

	for( csl::PatternWeights::const_PatternIterator it = patternsBegin(); it != patternsEnd(); ++it ) {
	    os << "<pattern left=\"" << it->first.getLeft() << "\" right=\"" << it->first.getRight()
	       << "\" prob=\"" << it->second << "\"/>" << std::endl;
	}

	os << "</patternWeights>" << std::endl;
    }

    void PatternWeights::writeToXML( char const* xmlFile ) const {
	std::wofstream fo;
	fo.imbue( std::locale( "" ) );
	fo.open( xmlFile );
	if( ! fo ) {
	    throw exceptions::badFileHandle( "csl::PatternWeights: Could not open pattern file for writing" );
	}
	writeToXML( fo );
	fo.close();
    }


    void PatternWeights::sortToVector( std::vector< std::pair< csl::Pattern, double > >* vec ) const {
	if( ! vec->empty() ) throw exceptions::cslException( "csl::PatternWeights::sortToVector: output vector not empty." );
        for( std::map< csl::Pattern, double >::const_iterator it = patternWeights_.begin(); it != patternWeights_.end(); ++it ) {
	    vec->push_back( *it );
	}
	std::sort( vec->begin(), vec->end(), sortBySecond< std::pair< csl::Pattern, double > > );
    }

    void PatternWeights::print( std::wostream& str ) const{
	str << "*** PatternWEights::DebugPrint ***" << std::endl;
	std::vector< std::pair< csl::Pattern, double > > histPatternCountSorted;
	sortToVector( &histPatternCountSorted );

	for( std::vector< std::pair< csl::Pattern, double > >::const_iterator it = histPatternCountSorted.begin();
	     it != histPatternCountSorted.end();
	     ++it ) {
	    str << it->first.getLeft() << Pattern::leftRightDelimiter_ << it->first.getRight() << "#" << it->second << std::endl;
//	    if( it - histPatternCountSorted.begin() > 30 )break;
	}

	str << "Default settings:" << std::endl;
	for(std::map< PatternType, double >::const_iterator defaultIt = defaultWeights_.begin(); defaultIt != defaultWeights_.end(); defaultIt++){
	    str << "<" <<  defaultIt->first.first << ',' << defaultIt->first.second << "> : " << defaultIt->second << std::endl;
	}
    }
}
