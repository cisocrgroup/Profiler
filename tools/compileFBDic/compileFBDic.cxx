#include <FBDic/FBDic.h>
#include <iostream>

#include <Getopt/Getopt.h>

/**
 * @todo
 */
void printHelp() {
  std::wcerr << "Use like: compileFBDic <inFile.lex> [<outFile.fbdic>]"
             << std::endl
             << "If no <binDic> is provided, output is written to inFile.fbdic."
             << std::endl;
}

int main(int argc, char const **argv) {
  std::locale::global(std::locale(""));

  csl::Getopt options;
  options.specifyOption("help", csl::Getopt::VOID);
  options.getOptionsAsSpecified(argc, argv);

  if (options.hasOption("help")) {
    printHelp();
    return EXIT_SUCCESS;
  }
  if (options.getArgumentCount() < 1) {
    return EXIT_FAILURE;
  }

  std::string inFile = options.getArgument(0);
  std::string outFile;

  if (options.getArgumentCount() == 2) {
    outFile = options.getArgument(1);
  } else {
    outFile = inFile;
    if (outFile.substr(outFile.size() - 4) == ".lex") {
      outFile.replace(outFile.size() - 4, 4, ".fbdic");
    } else {
      std::wcerr << "Your input filename does not end with '.lex'. In this "
                    "case please provide an output filename as second argument."
                 << std::endl;
      return EXIT_FAILURE;
    }
  }

  csl::FBDic<int> t;

  try {
    t.compileDic(inFile.c_str());
    t.writeToFile(outFile.c_str());
  } catch (csl::exceptions::cslException &ex) {
    std::wcout << "compileFBDic failed: " << ex.what() << std::endl;
    return EXIT_FAILURE;
  }

  //    t.toDot();
  //    t.printCells();
  //    t.printDic();
}
