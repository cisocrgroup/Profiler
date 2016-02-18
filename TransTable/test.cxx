#include<iostream>
#include "TransTable.h"

int main() {
    typedef csl::TransTable< csl::TT_PERFHASH, uint16_t > TransTable_t;

    TransTable_t t;
    TransTable_t::TempState_t tempState;

    TransTable_t::StateId_t st;

    t.initConstruction();

    tempState.setFinal();
    tempState.addTransition( L'a', 2, 0 );
    tempState.addTransition( L'b', 3, 0 );
    tempState.addTransition( L'c', 4, 0 );

    st = t.storeTempState( tempState );


    tempState.reset();
    tempState.addTransition( L'x', st, 0 );
    st = t.storeTempState( tempState );    

    tempState.reset();
    tempState.addTransition( L'y', st, 0 );
    st = t.storeTempState( tempState );    

    t.finishConstruction();

    std::wcerr<<"fertig"<<std::endl;

    t.toDot();




}
