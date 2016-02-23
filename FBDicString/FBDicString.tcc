#ifndef CSL_MINDICSTRING_TCC
#define CSL_MINDICSTRING_TCC CSL_MINDICSTRING_TCC

namespace csl {

    inline FBDicString::FBDicString() :
	annStrings_( 0 ),
	sizeOfAnnStrings_( 0 ),
	annHash_( 0 ),
	keyValueDelimiter_( Global::keyValueDelimiter )
    {
	
    }

    inline FBDicString::FBDicString( char const* dicFile ) :
	annStrings_( 0 ),
	sizeOfAnnStrings_( 0 ),
	annHash_( 0 ),
	keyValueDelimiter_( Global::keyValueDelimiter )
    {
	loadFromFile( dicFile );
    }

    inline void FBDicString::setKeyValueDelimiter( uchar c ) {
	keyValueDelimiter_ = c;
    }

    inline const uchar* FBDicString::lookup( wchar_t* key ) const {
	int offset = 0;
	if( getFWDic().lookup( key, &offset ) ) {
	    return getAnnByOffset( offset );
	}
	else {
	    return 0;
	}
    }

    inline const uchar* FBDicString::getAnnByPerfHashValue_FW( size_t perfHashValue ) const {
	return getAnnByOffset( getFWDic().getAnnotation( perfHashValue ) );
    }

    inline const uchar* FBDicString::getAnnByPerfHashValue_BW( size_t perfHashValue ) const {
	return getAnnByOffset( getBWDic().getAnnotation( perfHashValue ) );
    }

    inline const uchar* FBDicString::getAnnByOffset( size_t offset ) const {
	assert( offset < sizeOfAnnStrings_ );
	return annStrings_ + offset;
    }



    inline void FBDicString::loadFromFile( char const* dicFile ) {
	FILE* fi = fopen( dicFile, "rb" );
	if ( !fi ) {
	    throw exceptions::badFileHandle( "csl::FBDicString: Couldn't open file '" +
					     std::string( dicFile ) +
					     "' for reading." );
	}
	loadFromStream( fi );
	fclose( fi );
    }
    
    inline void FBDicString::loadFromStream( FILE* fi ) {
	size_t freadReturn = 0; 
	freadReturn = fread( &header_, sizeof( Header ), 1, fi );
	if( freadReturn != 1 ) {
	    throw exceptions::badDictFile( "FBDicString: Failure during reading of header.\n" );	    
	}

	if ( ( header_.getMagicNumber() != magicNumber_ ) ) {
	    throw exceptions::badDictFile( "FBDicString: Magic number comparison failed.\n" );
	}
	
	sizeOfAnnStrings_ = header_.getSizeOfAnnStrings();
	FBDic_t::loadFromStream( fi );
	annStrings_ = (uchar*) malloc( sizeOfAnnStrings_ * sizeof( uchar ) );
	freadReturn = fread( annStrings_, sizeof( uchar ), sizeOfAnnStrings_, fi );
	if( freadReturn != sizeOfAnnStrings_ ) {
	    throw exceptions::badDictFile( "FBDicString: Failure during reading of annStrings.\n" );	    
	}

    }

    inline void FBDicString::writeToFile( char const* dicFile ) const {
	FILE* fo = fopen( dicFile, "wb" );
	if ( !fo ) {
	    throw exceptions::badFileHandle( "FBDicString: Couldn't open file '" +
					     std::string( dicFile ) +
					     "' for writing." );
	}
	writeToStream( fo );
	fclose( fo );
    }
    
    inline void FBDicString::writeToStream( FILE* fo ) const {
	fwrite( &header_, sizeof( Header ), 1, fo );
	FBDic_t::writeToStream( fo );
	fwrite( annStrings_, sizeof( uchar ), sizeOfAnnStrings_, fo );
    }


    inline void FBDicString::initConstruction() {
	FBDic_t::initConstruction();
	
    }

    inline void FBDicString::finishConstruction() {
	FBDic_t::finishConstruction();
	sizeOfAnnStrings_ = annHash_->getLengthOfKeyStrings();
	annStrings_ = (uchar*)realloc( annStrings_, sizeOfAnnStrings_ );
	delete( annHash_ );
	header_.set( *this );
    }

    inline void FBDicString::compileDic( const char* lexFile ) {
	initConstruction();
	
	std::ifstream fileHandle( lexFile );
	if( !fileHandle.good() ) {
	    throw exceptions::badFileHandle( "Couldn't open file '" + 
					     std::string( lexFile ) + 
					     "' for reading." );
	}

	
	struct stat f_stat;
	stat( lexFile, &f_stat );
	size_t estimatedNrOfKeys = f_stat.st_size / 100;
	if( estimatedNrOfKeys < 1000 ) estimatedNrOfKeys = 1000; // set a minimum of 1000

	//std::wcerr<<"Estimate about "<< estimatedNrOfKeys << " Keys."<< std::endl;
	
	annHash_ = new Hash< uchar >( estimatedNrOfKeys, annStrings_, sizeOfAnnStrings_ );


	uchar bytesIn[Global::lengthOfLongStr];
	// set the last byte to 0. So we can recognize when an overlong string was read by getline().
	bytesIn[Global::lengthOfLongStr - 1] = 0; 

	wchar_t key[Global::lengthOfLongStr];
	uchar* annotationStr = 0;

	while( fileHandle.getline(( char* ) bytesIn, Global::lengthOfLongStr ) )  {
 	    if ( bytesIn[Global::lengthOfLongStr-1] != 0 ) {
		throw exceptions::badInput( "csl::FBDicString::compileDic: Maximum length of input line violated (set by Global::lengthOfLongStr)" );
	    }
	    
	    /////////////////// PARSE THE INPUT STRING
	    uchar *c;
	    c = ( uchar* )strchr( ( char* )bytesIn, keyValueDelimiter_ );
	    
	    if( c ) {
		*c = 0;
		annotationStr = ( c + 1 );
	    }
	    else throw exceptions::badInput( "csl::FBDicString::compileDic: No string annotation given." );

	    if( mbstowcs( key, (const char*)bytesIn, Global::lengthOfLongStr ) == (size_t)-1 ) {
		throw exceptions::badInput( "csl::FBDicString::compileDic: Invalid utf-8 sequence" );
	    }

	    if( ! ( c  && *key ) ) {
		throw exceptions::badInput( "csl::FBDicString::compileDic: wrong input format" );
	    }

	    size_t offset = annHash_->findOrInsert( annotationStr );
	    
	    FBDic_t::addToken( key, offset );

	}
	fileHandle.close();

	finishConstruction();
    }


    void FBDicString::doAnalysis() const {
	getFWDic().doAnalysis();
	getBWDic().doAnalysis();

	printf( "**********\nFBDicString Analysis\n**********\nannotation strings: %.3f MB\n\n",
		(double)header_.getSizeOfAnnStrings() / 1048576
	    );
	
    }

}

#endif
