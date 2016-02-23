#include<iostream>

#include "./ResultSet.h"
using namespace csl;

int main() {
    std::cout<<"ResultSet test"<<std::endl;
    ResultSet s;

    s.receive( L"uli", 42, 25 );
    s.receive( L"felix", 43, 5 );
    s.receive( L"maxi", 44, 1 );
    s.receive( L"ina", 45, 1 );
    s.receive( L"julia", 46, 1 );
    s.receive( L"julia", 46, 1 );
    s.receive( L"julia", 46, 1 );
    s.receive( L"julia", 46, 1 );
    s.receive( L"julia", 46, 1 );
    s.receive( L"julia", 46, 1 );
    s.receive( L"julia", 46, 1 );
    s.receive( L"julia", 46, 1 );
    s.receive( L"julia", 46, 1 );
    s.receive( L"julia", 46, 1 );
    s.receive( L"julia", 46, 1 );
    s.receive( L"julia", 46, 1 );
    s.receive( L"julia", 46, 1 );
    s.receive( L"julia", 46, 1 );
    s.receive( L"julia", 46, 1 );
    s.receive( L"julia", 46, 1 );
    s.receive( L"julia", 46, 1 );
    s.receive( L"julia", 46, 1 );
    s.receive( L"julia", 46, 1 );
    s.receive( L"julia", 46, 1 );
    s.receive( L"julia", 46, 1 );
    s.receive( L"julia", 46, 1 );
    s.receive( L"julia", 46, 1 );
    s.receive( L"julia", 46, 1 );
    s.receive( L"julia", 46, 1 );
    s.receive( L"julia", 46, 1 );
    s.receive( L"julia", 46, 1 );

    for( size_t i = 0;i < s.getSize();++i ) {
	printf( "%ls,%d,%d\n", s[i].getStr(), s[i].getLevDistance(), s[i].getAnn() );
    }

    
}
