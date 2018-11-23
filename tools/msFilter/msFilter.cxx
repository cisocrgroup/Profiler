#include "Getopt/Getopt.h"
#include "Global.h"
#include "MSMatch/MSMatch.h"
#include "ResultSet/ResultSet.h"
using namespace csl;

/**
 * @todo The wstring handling in this program is hopelessly deprecated!!!
 * Update!
 */

int
main(int argc, char const** argv)
{
  setlocale(LC_CTYPE,
            "de_DE.UTF-8"); /*Setzt das Default Encoding für das Programm */

  Getopt opt(argc, argv);

  if (opt.getArgumentCount() != 2) {
    std::cerr << "Use like: msFilter [--machineReadable=1] <dlev> <fb_dic>"
              << std::endl;
    exit(1);
  }

  bool machineReadable = false;
  if (opt.hasOption("machineReadable")) {
    machineReadable = true;
  }

  try {
    MSMatch<FW_BW> matcher(atoi(opt.getArgument(0).c_str()),
                           opt.getArgument(1).c_str());
    //	MSMatch< STANDARD > matcher( atoi( argv[1] ), argv[2] );

    ResultSet list;

    uchar bytesIn[Global::lengthOfLongStr];
    wchar_t query[Global::lengthOfLongStr];

    // set the last byte to 0. So we can recognize when an overlong string was
    // read by getline().
    bytesIn[Global::lengthOfLongStr - 1] = 0;

    while (std::cin.getline((char*)bytesIn, Global::lengthOfLongStr)) {
      if (bytesIn[Global::lengthOfLongStr - 1] != 0) {
        throw exceptions::badInput(
          "csl::msFilter: Maximum length of input line violated (set by "
          "Global::lengthOfLongStr)");
      }
      mbstowcs(query, (char*)bytesIn, Global::lengthOfLongStr);

      // printf( "Query: %ls\n", query );

      list.reset(); // forget candidates that might be stored from earlier use

      try {
        matcher.query(query, list);
      } catch (exceptions::bufferOverflow& exc) {
        fprintf(stderr, "%s: %lu\n", exc.what(), (unsigned long)list.getSize());
      }

      // 	    std::cout<<list.getSize()<<" hits."<<std::endl;
      //    	    list.sortUnique();
      //   	    std::cout<<list.getSize()<<" hits."<<std::endl;

      // print all hits

      if (machineReadable) { // print all hits in one line
        size_t i = 0;
        for (i = 0; i < list.getSize(); ++i) {
          wprintf(L"%ls,%d,%d",
                  list[i].getStr(),
                  list[i].getLevDistance(),
                  list[i].getAnn());
          if (i + 1 != list.getSize())
            std::wcout << "|";
        }
        wprintf(L"\n");
      } else {
        size_t i = 0;
        for (i = 0; i < list.getSize(); ++i) {
          wprintf(L"%ls,%d,%d\n",
                  list[i].getStr(),
                  list[i].getLevDistance(),
                  list[i].getAnn());
        }
      }
    }
  } catch (exceptions::cslException& exc) {
    std::cerr << "msFilter caught exception: " << exc.what() << std::endl;
  }
}
