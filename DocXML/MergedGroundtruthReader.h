
#include<AbbyyXmlParser/AbbyyXmlParser.h>
#include<TXTReader/AlignedTXTReader.h>
#include<DocXML/DocXMLWriter.h>

namespace OCRCorrection {

    /**
     * @brief 
     */
    class MergedGroundtruthReader {
    public:

	void mergeDirectories( char const* abbyyXMLDir, char const* alignedXMLDir, Document* doc );
	

	void mergeDocuments( Document* abbyyDoc, Document& alignedTXTDoc );

	void mergeAreas( Document::iterator abbyyBegin, Document::iterator abbyyEnd, 
			 Document::iterator docXMLBegin, Document::iterator docXMLEnd );
	
    private:

	bool bsbIDCheck( std::wstring const& abbyyFilename, std::wstring const& alignedTXTFilename ) const;
	

	class MergeStatistic {
	public:
	    void registerPair( Token const& abbyyToken, Token const& docToken ) {

		// size_t levDistance = // here we can safely cast to size_t, because we compute std lev distance
		//     static_cast< size_t >( levDistanceComputer_.compLevDistance( abbyyToken.getWOCR(), docToken.getWOCR() ) );
		
		// if( levDistance + 1 > distanceDistribution_.size() ) {
		//     distanceDistribution_.resize( levDistance + 1 );
		// }
		// distanceDistribution_.at( levDistance ) += 1; 
		
	    }
	    
	    void print( std::wostream& os = std::wcout ) const {
		size_t count = 0;
		for( std::vector< size_t >::const_iterator it = distanceDistribution_.begin();
		     it != distanceDistribution_.end();
		     ++it, ++count ) {
		    os << "distance " << count << " : " << *it << std::endl;
		}
	    }

	private:
	    /**
	     * @brief distanceDistribution_.at( n ) stores the number of aligned wOCR-pairs that have distance n.
	     */
	    std::vector< size_t > distanceDistribution_;
	    //LevDistance levDistanceComputer_;
	}; // class MergeStatistic
	
	MergeStatistic mergeStatistic_;


    };

} // eon
