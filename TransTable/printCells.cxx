#include<iostream>
#include "TransTable.cxx"
#include "../Alphabet/Alphabet.cxx"

int main(int argc, char** argv) {
    csl::Alphabet alph(argv[1]);
    csl::TransTable t(alph.size(), argv[2]);

    t.printCells();
}
