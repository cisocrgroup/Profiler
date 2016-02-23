#include<algorithm>
#include<iostream>
#include<fstream>
#include<string>
#include<cstring>
#include<vector>


class Entry {
public:
    Entry( const std::wstring& line ) {
	line_ = line;
	size_t pos = line.find( '#' );
	key_ = line.substr( 0, pos );
    }

    bool operator<( const Entry& other ) const {
	return this->key_ < other.key_;
    }
    
    std::wstring line_;
    std::wstring key_;
};

int main( int argc, char** argv ) {

    std::locale::global( std::locale( "" ) );

    if( argc != 2 || ! strcmp( argv[1], "-h" ) ) {
	std::wcerr<<"sortLex - sort .lex dictionary source files to be compiled into a FSA."<<std::endl;
	std::wcerr<<"Use like: sortLex unsorted.txt > sorted.txt"<<std::endl;
	return 1; 
    }
    std::wcerr<<"An easier and faster way to do this is the unix sort tool: $> LC_ALL=C sort -t'#' -k1,1 foo.unsorted > foo.lex"<<std::endl;

    std::wifstream fi( argv[1] );
    fi.imbue( std::locale( "" ) ); // imbue the stream with the environment's standard locale
    if( !fi.good() ) {
	std::wcout << "Couldn't open file for reading." << std::endl;
	return 1;
    }
    
    std::wstring line;
    std::vector< Entry > lines;
    while( std::getline( fi, line ).good() ) {
	lines.push_back( Entry( line ) );
    }

    std::sort( lines.begin(), lines.end() );

    for( std::vector< Entry >::const_iterator it = lines.begin(); it != lines.end(); ++it ) {
	std::wcout<<it->line_<<std::endl;
    }

    return 0;
}
