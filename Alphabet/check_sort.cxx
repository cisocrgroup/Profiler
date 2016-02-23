#include<iostream>
#include"./Alphabet.h"
typedef unsigned char uchar;

int main(int argc, char** argv) {

    if(argc<3) {
	std::cout<<"Use like: ./check_sort <alph> <text_dic>"<<std::endl;
	exit(1);
    }

    Alphabet alph(argv[1]);
    FILE* handle = fopen(argv[2],"r");
    if(!handle) {
	std::cerr<<"Couldn't read "<<argv[2]<<std::endl;
	exit(1);
    }

    char* line = new char[Global::lexline_length];
    char* last_line = new char[Global::lexline_length];

    char* temp;

    int line_count = 0;
    while(fgets(line,Global::lexline_length,handle)) { 
	line[strlen((char*)line)-1] = 0; // delete newline
      ++line_count;
//      std::cerr<<"line: '"<<line<<"'"<<std::endl;
      

 	if(alph.strcmp((uchar*)line,(uchar*)last_line) < 0) {
	    std::cout<<":"<<last_line<<":"<<line;
	    std::cout<<"Sort error in line "<<line_count<<std::endl;
	    return 1;
	}

	// swap string buffers
	temp = last_line;
	last_line = line;
	line = temp;
    }

    return 0;
}
