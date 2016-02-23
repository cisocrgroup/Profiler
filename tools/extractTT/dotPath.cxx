#include<iostream>
#include "Getopt/Getopt.h"
#include "Alphabet/Alphabet.h"
#include "TransTable/TransTable.h"

using namespace csl;

typedef TransTable< TT_PERFHASH, uint16_t, uint32_t > TransTable_t;


int main( int argc, const char** argv ) {
    std::locale::global( std::locale( "" ) );

    Getopt options( argc, argv );

    if( options.getArgumentCount() != 1 ) {
	std::wcerr<<std::endl
		  <<"Use like: dotPath <binDic>"<<std::endl
		  <<"dotPath reads one word from stdin and prints dot code for all states which are on the path of that input word in the automaton."<<std::endl
		  <<std::endl<<std::endl;

	exit(1);
    }
    try {
	TransTable_t t;
	t.loadFromFile( options.getArgument( 0 ).c_str() );

	TransTable_t::State st = t.getRootState();

	std::wstring word;
	std::getline( std::wcin, word );
	std::wstring::const_iterator c = word.begin();

	std::wcout<<"Digraph dotPath {  // DOTCODE"<<std::endl;
	while( st.isValid() && c != word.end() ) {
	    for( wchar_t const* suso = st.getSusoString(); *suso; ++suso ) {
		std::wstring color = ( *suso == *c )? L"color=red,fontcolor=red," : L"";
		std::wcout << st.getStateID() << "->" << st.getTransTarget( *suso ).getStateID() << "["<< color<< "label=\""<< *suso <<"\"]; // DOTCODE" << std::endl;
	    }

	    st.walk( *c );
	    ++c;
	}

	std::wcout<<"} // DOTCODE"<<std::endl;
	return 0;


    } catch ( exceptions::cslException ex ) {
	std::wcerr << "dotPath failed: " << ex.what() << std::endl;
	return 1;
    }
}
