#ifndef CSL_PATTERN_SET
#define CSL_PATTERN_SET CSL_PATTERN_SET

#include<fstream>
#include<vector>
#include<errno.h>

#include "Utils/Utils.h"
#include "../Global.h"
#include "./Pattern.h"
#include "../DictSearch/GlobalHistPatterns.hpp"


namespace csl {

  /**
   * @brief A collection of patterns that should be available in some context.
   *
   * @author Ulrich Reffle, 2008
   */
  class PatternSet {
  protected:
	typedef std::vector< Pattern > PatternList_t;

  public:
	typedef PatternList_t::iterator iterator;
	typedef PatternList_t::const_iterator const_iterator;


	/**
	 * @brief
	 */
	inline PatternSet();


	/**
	 * @brief Returns a reference pointing to the \c i -th position of the
	 * set
	 */
	inline Pattern const& at( size_t i ) const {
	  return patternList_.at( i );
	}

	/**
	 * @brief Returns a reference pointing to the \c i -th position of the
	 *        STRIPPED set.
	 *
	 */
	inline Pattern const& stripped_at( size_t i ) const {
	  return strippedPatternList_.at( i );
	}


	/**
	 * @brief returns an iterator at the first pattern of the list
	 *
	 * Note that this is not the necessarily the first element of the internal container
	 */
	inline iterator begin() {
	  return ++( patternList_.begin() );
	}

	/**
	 * @brief returns a classical past-the-end iterator
	 */
	inline iterator end() {
	  return patternList_.end();
	}


	/**
	 * @brief Reads a set of patterns from file \c patternFile.
	 *
	 * The file is expected to contain one pattern per line, with a blank to
	 * separate left from right side. Example:
	 * @code
	 * t th
	 * ei ey
	 * u v
	 * @endcode
	 */
	inline void loadPatterns( const char* patternFile );

  protected:

	inline const PatternList_t& patternList() const {
	  return patternList_;
	}

  private:
	inline void addPattern(const std::wstring& pattern);
	PatternList_t patternList_;

	/**
	 * @brief This list holds all patterns which are also in patternList_, but WITHOUT the ^ and $ markers.
	 */
	PatternList_t strippedPatternList_;

	static bool either_or( bool a, bool b ) {
	  return ( ( a && !b ) || ( !a && b ) );
	}

  }; // class PatternSet


  PatternSet::PatternSet() {
	patternList_.push_back( Pattern() );
	strippedPatternList_.push_back( Pattern() );
  }

  void PatternSet::loadPatterns( const char* patternFile ) {
	std::wifstream fi;
	fi.open( patternFile );
	if( ! fi.good() ) {
	  std::string message =
		std::string( "PatternSet::Could not open pattern file: " ) +
		patternFile;

	  throw exceptions::badFileHandle( message );
	}

	std::wstring line;
	while( getline( fi, line ).good() ) {
	  if( line.empty() ) continue;
	  if( line.at(0) == '#' ) continue;
	  addPattern(line);
	}
	if( errno == EILSEQ ) { // catch encoding error
	  throw exceptions::badInput( "csl::PatternSet: Encoding error in input sequence." );
	}
	for (const auto& pattern: GlobalHistPatterns::instance().getHistPatterns()) {
	  addPattern(pattern);
	}
#ifndef PROFILER_NO_LOG	
	std::wcerr << "csl::PatternSet: Loaded "
			   << GlobalHistPatterns::instance().getHistPatterns().size()
			   << " global patterns." << std::endl;
	std::wcerr << "csl::PatternSet: Loaded "
			   << patternList_.size() - 1 << " patterns."
			   << std::endl;
#endif // PROFILER_NO_LOG				   
  } // PatternSet::loadPatterns

  void PatternSet::addPattern(const std::wstring& line) {
	size_t delimPos = line.find( Pattern::leftRightDelimiter_ );
	if( delimPos == std::wstring::npos ) {
	  throw exceptions::badInput( std::string( "PatternSet: Invalid pattern - no separator found: " ) + OCRCorrection::Utils::utf8(line));
	}

	std::wstring left = line.substr( 0, delimPos );
	std::wstring right = line.substr( delimPos + 1 );

	// check for word border markers
	bool wordBegin_left, wordEnd_left, wordBegin_right, wordEnd_right;
	size_t pos = left.find( Global::wordBeginMarker );
	if( pos == left.npos ) {
	  wordBegin_left = false;
	}
	else if( pos == 0 ) {
	  wordBegin_left = true;
	}
	else {
	  throw exceptions::badInput(
								 std::string( "PatternSet: Invalid pattern - wordBeginMarker in middle of left side: " )
								 + OCRCorrection::Utils::utf8(line));
	}

	pos = right.find( Global::wordBeginMarker );
	if( pos == right.npos ) {
	  wordBegin_right = false;
	}
	else if( pos == 0 ) {
	  wordBegin_right = true;
	}
	else {
	  throw exceptions::badInput(
								 std::string( "PatternSet: Invalid pattern - wordBeginMarker in middle of right side: " )
								 + OCRCorrection::Utils::utf8(line));
	}

	pos = left.find( Global::wordEndMarker );
	if( pos == left.npos ) {
	  wordEnd_left = false;
	}
	else if( pos == left.size()-1 ) {
	  wordEnd_left = true;
	}
	else {
	  throw exceptions::badInput(
								 std::string( "PatternSet: Invalid pattern - wordEndMarker in middle of left side: " )
								 + OCRCorrection::Utils::utf8(line));
	}

	pos = right.find( Global::wordEndMarker );
	if( pos == right.npos ) {
	  wordEnd_right = false;
	}
	else if( pos == right.size() - 1 ) {
	  wordEnd_right = true;
	}
	else {
	  throw exceptions::badInput( std::string( "PatternSet: Invalid pattern - wordEndMarker in middle of right side: " ) + OCRCorrection::Utils::utf8(line));
	}

	if( either_or( wordBegin_left, wordBegin_right ) ) {
	  throw exceptions::badInput(
								 std::string( "PatternSet: Invalid pattern - wordBeginMarker must be specified on left AND right side: " )
								 + OCRCorrection::Utils::utf8(line));
	}

	if( either_or( wordEnd_left, wordEnd_right ) ) {
	  throw exceptions::badInput(
								 std::string( "PatternSet: Invalid pattern - wordEndMarker must be specified on left AND right side: " )
								 + OCRCorrection::Utils::utf8(line));
	}
	// end: check for markers

	patternList_.push_back( Pattern( left, right ) );


	if( wordBegin_left ) { // then also wordBegin_right must be true
	  left.erase( 0, 1 );
	  right.erase( 0, 1 );
	}
	if( wordEnd_left ) { // then also wordBegin_right must be true
	  left.erase( left.size()-1, 1 );
	  right.erase( right.size()-1, 1 );
	}

	if( left.empty() && right.empty() ) {
	  throw exceptions::badInput(
								 std::string( "PatternSet: Invalid pattern - empty pattern: " )
								 + OCRCorrection::Utils::utf8(line));
	}

	strippedPatternList_.push_back( Pattern( left, right ) );

  } // addPattern
} // eon


#endif
