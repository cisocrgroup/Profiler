#include<iostream>

#include "./Hash.h"

using namespace csl;

void smokeTest() {
    wchar_t* strings = NULL;
    size_t lengthOfStrings = 0;
    Hash h( 5, strings, lengthOfStrings );

    h.findOrInsert( L"Uli"  );
    h.findOrInsert( L"Anna" );
    h.findOrInsert( L"Uli" );

    std::cout<<"Uli: "<<h.find( L"Uli" )<<std::endl;
    std::cout<<"Ulli: "<<h.find( L"Ulli" )<<std::endl;

}


int main() {
    std::cout<<"Tests for class Hash"<<std::endl;

    smokeTest();

}


