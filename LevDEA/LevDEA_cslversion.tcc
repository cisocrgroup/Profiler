#ifndef LEVDEA_CXX
#define LEVDEA_CXX LEVDEA_CXX

#include "./LevDEA_cslversion.h"

namespace csl {


#include "lev0data.cxx"
#include "lev1data.cxx"
#include "lev2data.cxx"
#include "lev3data.cxx"
    
    LevDEA::LevDEA( int init_k ) : k_( 0 ),
				   charvecs_( Global::maxNrOfChars, 0 )
    {
	*pattern_ = 0; // set pattern to empty word
	tabsLoaded_ = 0;
	setDistance( init_k );
    }
    
    LevDEA::~LevDEA() {
	delete( tab );
	delete( fin );
    }


    void LevDEA::setDistance( int initK ) {
	k_ = initK;

	/*
	  In case of k==1, z2k1 would be 111  (==7) ( 2k+1 set bits )
	  z2k2 would be 1111 (==15)  ( 2k+2 set bits )
	*/
	z2k1 = 1ll;
	z2k1 <<=  2 * k_ + 1;
	z2k1--; // a sequence of 2k+1 1-values
	z2k2 = 1ll;
	z2k2 <<= 2 * k_ + 2;
	z2k2--; // a sequence of 2k+2 1-values

	if( ( tabsLoaded_ & ( 1 << k_ ) ) == 0 ) { // have the tables for k already been loaded???

	    const int* arrPos;
	    if( initK == 0 ) arrPos = lev0data;
	    else if( initK == 1 ) arrPos = lev1data;
	    else if( initK == 2 ) arrPos = lev2data;
	    else if( initK == 3 ) arrPos = lev3data;
	    else throw exceptions::invalidLevDistance( "LevDEA: This distance is not supported. Supported distances are 0, 1, 2, 3" );

	    k_= *arrPos; ++arrPos;;
	    coresetss[k_] = *arrPos; ++arrPos;
	    coresets = coresetss[k_];

	    tabs[k_] = new table_cell[z2k2*coresets];
	    fins[k_] = ( int* )malloc( ( 2 * k_ + 1 ) * coresets * sizeof( int ) );
	    tab = tabs[k_];
	    fin = fins[k_];

	    size_t row, col;

	    for( row = 0; row < z2k2; row++ ) {
		for( col = 0; col < coresets; ++col ) {
		    table( row, col ).target =  *arrPos; ++arrPos;
		    table( row, col ).move_pattern =  *arrPos; ++arrPos;
		}
	    }

	    for( row = 0; row < 2*k_ + 1; row++ ) {
		for( col = 0; col < coresets; ++col ) {
		    fin[coresets*row+col] =  *arrPos; ++arrPos;
		}
	    }
	    tabsLoaded_ = tabsLoaded_ | ( 1 << k_ );
	}
	else {
	    // tables were loaded some time before. simply assign the pointers.
	    tab = tabs[k_];
	    fin = fins[k_];
	    coresets = coresetss[k_];
	}
    }

    void LevDEA::loadPattern( const wchar_t* p ) {
	cleanCharvecs(); // do this while the old pattern is still loaded!
	patLength_ = wcslen( p );
	if( patLength_ > Global::lengthOfWord ) {
	    throw exceptions::badInput( "csl::LevDEA::loadPattern: Maximum Pattern length (as specified by Global::lengthOfWord) violated." );
	}
	wcscpy( pattern_, p );
	calcCharvec();
    }

    void LevDEA::cleanCharvecs() {
	for( const wchar_t* pat = pattern_; *pat; ++pat ) {
	    charvecs_.at( *pat ) = 0;
	}
    }
    
    void LevDEA::calcCharvec() {
	bits64 c;
	const wchar_t* pat;
	for ( c = z10, pat= pattern_; *pat; ++pat, c >>= 1 ) {
	    charvecs_[*pat] |= c;
	}
    }
    
    bits32 LevDEA::calc_k_charvec( wchar_t c, size_t i ) const {
	bits64 r;
	// after the next line, the bits i,i+1,i+2 of chv are the lowest bits of r. All other bits of r are 0
	r = ( charvecs_[c] >> ( 64 - ( 2 * k_ + 1 + i ) ) ) & z2k1;
	if ( patLength_ - i < 2 * k_ + 1 ) // the last few chars of the word
	    r = ( ( r >> ( 2 * k_ + 1 - ( patLength_ - i ) ) ) | ( zff << ( ( patLength_ - i ) + 1 ) ) ) & z2k2;
	return ( (bits32) r );
    }

    void LevDEA::printTable() const {
	size_t row, col;
	for ( row = 0; row < z2k2; row++ ) {
	    printf( "%d\t", row );
	    for ( col = 0; col < coresets; ++col ) {
		printf( "(%d,%d)\t", table( row, col ).target, table( row, col ).move_pattern );
	    }
	    printf( "\n" );
	}
    }

    void LevDEA::printCharvec() const {
	std::cout << "-------------" << std::endl;
	for( const wchar_t* c = pattern_; *c; ++c ) {
	    printf( "%lc\n", *c );
	    printBits( charvecs_[*c] );
	}
	std::cout << "-------------" << std::endl;
    }
    
    void LevDEA::printBits( const bits64& n ) const {
	int i;
	for( i = 63;i >= 0;--i ) {
	    if( ( i % 10 ) == 0 ) printf( "%d", i / 10 );
	    else if( ( i % 10 ) == 5 ) printf( "|" );
	    else printf( " " );
	}
	printf( "\n" );
	for( i = 63;i >= 0;--i ) {
	    printf( "%i", ( int )( 1 & ( n >> i ) ) );
	}
	printf( "\n" );

    }

} // eon
#endif
