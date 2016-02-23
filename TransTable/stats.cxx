#include<iostream>

#include "../Alphabet/Alphabet.h"
#include "./TransTable.h"
using namespace csl;

Alphabet* alph;
TransTable* t;

static const int maxCounts = 30;

enum{STATES, ENTRIES};
char cats[50][50] = {"States","Entries"};

int count[maxCounts];


void getStats(int pos, int depth)  {
    int c;
    static int newPos;

    ++count[STATES];
    
    for(c=1;c<= alph->size();++c) {
//	    std::cout<<"depth="<<depth<<", char="<<(uchar)alph_.decode(c)<<std::endl;
	if((newPos = t->walk(pos,c))) {
//	    w[depth] = alph_.decode(c);
	    
	    if(t->isFinal(newPos)) {
//		w[depth+1] = 0;
//		printf("%s",(char*)w);
		++count[ENTRIES];

// 		if(hasAnnotations(newPos)) {
// 		    std::cout<<Global::keyValueDelimiter;
// 		    AnnIterator it(*this, newPos);
// 		    while(it.isValid()) {
// 			printf("%d, ",*it);
// 			++it;
// 		    }
// 		}
//		printf("\n");
		    
//		if((++count_ % 100000) == 0) fprintf(stderr, "%d\n", count_);
	    } // if isFinal

	    getStats(newPos, depth+1);
	} // if couldWalk
    } // for
}

int main(int argc, char** argv) {
    alph = new Alphabet(argv[1]);
    t = new TransTable(alph->size(), argv[2]);

    memset(count,0,maxCounts * sizeof(int));

    getStats(t->getRoot(),0);
    
    for(int i=0; i<2; ++i) {
	printf("%30s\t %d\n",cats[i],count[i]);
    }

} // end of method 


    
    
