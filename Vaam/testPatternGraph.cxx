
#include<iostream>
#include "../MinDic/MinDic.h"
#include "./PatternGraph.h"


int main(int argc, const char** argv ) {

    std::locale::global( std::locale( "de_DE.utf-8" ) );
    
    Getopt opt( argc, argv );
    
    if( opt.getArgumentCount() < 1 ) {
	std::wcerr<<"so nicht!"<<std::endl;
	exit( 1 );
    }

    csl::PatternGraph patGraph;
    patGraph.loadPatterns( opt.getArgument( 0 ).c_str() );


    std::wstring str( L"antannantonundonno" );


    std::wstring::const_iterator c = str.begin();
    csl::PatternGraph::State state = patGraph.getRoot();

    for( ; c != str.end(); ++c ) {
	state.walk( *c );
	if( state.isFinal() ) {
	    std::wcout<<"HIT"<<std::endl;
	}
    }
}
