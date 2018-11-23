
#include <TransTable/TransTable.h>
#include <cstdlib>

using namespace csl;

int
main(int argc, char** argv)
{
  //    setlocale(LC_CTYPE, "de_DE.UTF-8");  /*Setzt das Default Encoding für
  //    das Programm */
  std::locale::global(std::locale("")); // set the environment's default locale

  try {

    if (argc != 2) {
      std::cerr << "Use like: " << argv[0] << " <binDic>" << std::endl;
      exit(1);
    }

    TransTable<TT_PERFHASH, uint16_t, uint32_t> tt;
    tt.loadFromFile(argv[1]);

    std::wstring query;
    while (std::getline(std::wcin, query).good()) {
      // is this really necessary ??
      if (query.length() > Global::lengthOfLongStr) {
        throw exceptions::badInput(
          "csl::compileMD: Maximum length of input line violated (set by "
          "Global::lengthOfLongStr)");
      }

      size_t ann = 0;
      if (tt.getTokenIndex(query.c_str(), &ann)) {
        std::wcout << ann << std::endl;
      } else
        std::wcout << std::endl;
    }

  } catch (exceptions::cslException& ex) {
    std::wcout << "lookupTT: " << ex.what() << std::endl;
    exit(1);
  }
  return 0;
}
