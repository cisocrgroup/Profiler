#include "Alphabet.h"
#include<iostream>


int main(int argc,char** argv) {
    if(argc<2) {
	std::cout<<"use like: Alphabet_test <alph>"<<std::endl;
	exit(1);
    }

    //    Alphabet alph(argv[1]);
    csl::Alphabet alph(argv[1]);

    int i;
    std::cout<<"alph -> char(ascii)"<<std::endl;
    for(i=1;i<=alph.size();++i) {
	printf("%d -> %c (%d)\n",i,alph.decode(i),alph.decode(i));
    }

    std::cout<<std::endl<<"char(ascii) -> alph"<<std::endl;
    for(i=1;i<256;++i) {
	if(alph.code(i,0))
	    printf("%c(%d) -> %d \n",i,i,alph.code(i));
    }


    // test the strcmp-function
    //    std::cout<<alph.strcmp((uchar*)"uli",(uchar*)"ula")<<std::endl;

}
