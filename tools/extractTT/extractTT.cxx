#include "Alphabet/Alphabet.h"
#include "Getopt/Getopt.h"
#include "TransTable/TransTable.h"
#include <iostream>

using namespace csl;

int
main(int argc, const char** argv)
{
  setlocale(LC_CTYPE,
            "de_DE.UTF-8"); /*Setzt das Default Encoding für das Programm */

  Getopt options(argc, argv);

  if (options.getArgumentCount() != 1) {
    std::wcerr << std::endl
               << "Use like: extractTT [options] <binDic> " << std::endl
               << "Options:" << std::endl
               << "--stat=1\tto print statistics of the dic" << std::endl
               << "--dot=1\tto print dotcode for the dic" << std::endl
               << std::endl;

    exit(1);
  }
  try {
    TransTable<TT_PERFHASH, uint16_t, uint32_t> t;
    t.loadFromFile(options.getArgument(0).c_str());

    if (options.hasOption("dot")) {
      t.toDot();
    } else if (options.hasOption("stat")) {
      t.doAnalysis();
    }
    // else t.printDic();

    return 0;
  } catch (exceptions::cslException& ex) {
    std::wcerr << "Dictionary extraction failed: " << ex.what() << std::endl;
    return 1;
  }
}
