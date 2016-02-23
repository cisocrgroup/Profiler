#ifndef CSL_FBDIC_TCC
#define CSL_FBDIC_TCC CSL_FBDIC_TCC


namespace csl {

    template< class AnnType_t >
    FBDic< AnnType_t >::FBDic( const char* dicFile ) :
	fwDic_(),
	bwDic_() {

	if( dicFile ) {
	    loadFromFile( dicFile );
	}
    }

    template< class AnnType_t >
    inline const MinDic< AnnType_t >& FBDic< AnnType_t >::getFWDic() const {
	return fwDic_;
    }

    template< class AnnType_t >
    inline const MinDic< AnnType_t >& FBDic< AnnType_t >::getBWDic() const {
	return bwDic_;
    }


    template< class AnnType_t >
    void FBDic< AnnType_t >::initConstruction() {
	nrOfTokens_ = 0;
	fwDic_.initConstruction();

    }

    template< class AnnType_t >
    void FBDic< AnnType_t >::addToken( std::wstring const& entry, AnnType_t annotation ) {
	fwDic_.addToken( entry.c_str(), annotation );
	++nrOfTokens_;

	Global::reverse( entry, &reversedKey_ );
	entries_.push_back( DictEntry( std::wstring( reversedKey_ ), annotation ) );
    }

    template< class AnnType_t >
    void FBDic< AnnType_t >::finishConstruction() {
	fwDic_.finishConstruction();
	std::sort( entries_.begin(), entries_.end() );

	bwDic_.initConstruction();

	// why didn't the iterator work???
	for( size_t i = 0; i <  entries_.size(); ++i ) {
	    bwDic_.addToken( entries_.at( i ).getKey().c_str(), entries_.at( i ).getAnnotation() );
	}

	bwDic_.finishConstruction();

	header_.set( *this );
    }

    template< class AnnType_t >
    void FBDic< AnnType_t >::compileDic( const char* txtFile ) {

	initConstruction();

	std::wifstream fileHandle( txtFile );
	if( !fileHandle.good() ) {
	    throw exceptions::badFileHandle( "Couldn't open file '" +
					     std::string( txtFile ) +
					     "' for reading." );
	}

	std::wstring line;

	size_t lineCount = 0;
	while( std::getline( fileHandle, line ).good() )  {

	    if ( line.length() > Global::lengthOfLongStr ) {
		throw exceptions::badInput( "csl::MinDic::compileDic: Maximum length of input line violated (set by Global::lengthOfLongStr)" );
	    }
	    AnnType_t annotation;
	    fwDic_.parseAnnotation( &line, &annotation );
	    addToken( line, annotation );
	}

	finishConstruction();
    }


    template< class AnnType_t >
    inline void FBDic< AnnType_t >::loadFromFile( const char* binFile ) {
	FILE* fi = fopen( binFile, "rb" );

	if( ! fi ) {
	    throw exceptions::badFileHandle( std::string( "FBDic: Couldn't open file '" ) +
					     std::string( binFile ) +
					     std::string( "' for reading." ) );
	}

	loadFromStream( fi );
	fclose( fi );
    }

    template< class AnnType_t >
    inline void FBDic< AnnType_t >::loadFromStream( FILE* fi ) {
	// read the FBDic-Header
	if( fread( &header_, sizeof( Header ), 1, fi ) != 1 ) {
	  throw exceptions::badDictFile( "csl::FBDic::loadFromStream: Read error while reading header" );
	}

	if( header_.getMagicNumber() != magicNumber_ ) {
	    throw exceptions::badDictFile( "FBDic: Magic number comparison failed.\n" );
	}

	// read the FWDic
	fwDic_.loadFromStream( fi );

	// read the BWDic
	bwDic_.loadFromStream( fi );
    }


    template< class AnnType_t >
    inline void FBDic< AnnType_t >::writeToFile( char const* binFile ) const {
	FILE* fo = fopen( binFile, "wb" );
	if( ! fo ) {
	    throw exceptions::badFileHandle( "FBDic: Couldn't open file '" +
					     std::string( binFile ) +
					     "' for writing." );
	}
	writeToStream( fo );
	fclose( fo );
    }

    template< class AnnType_t >
    inline void FBDic< AnnType_t >::writeToStream( FILE* fo ) const {


	// write the header
	fwrite( &header_, sizeof( Header ), 1, fo );

	// write the FWDic
	fwDic_.writeToStream( fo );

	// write the BWDic
	bwDic_.writeToStream( fo );
    }

} // eon

#endif
