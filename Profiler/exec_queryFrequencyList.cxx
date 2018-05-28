#include "./FrequencyList.h"
#include <DictSearch/DictSearch.h>

#include <Getopt/Getopt.h>

/**
 * This is to explore the behaviour of the FrequencyList.
 * Query words to STDIN.
 * Output is the FrequencyList return value of all possible interpretations of
 * the query word.
 */
int
main(int argc, char const** argv)
{

  std::locale::global(std::locale(""));

  csl::Getopt options(argc, argv);

  if (options.getArgumentCount() != 2) {
    std::wcerr << "Use like: queryFrequencyList <frequencies.binfrq> "
                  "<patternweights.txt>"
               << std::endl;
    exit(1);
  }

  std::string modernDict =
    "/mounts/data/proj/impact/lexika/staticlex_de/staticlex_de.fbdic";
  std::string patternFile =
    "/mounts/data/proj/impact/software/Vaam/patterns.lexgui.txt";
  std::string histFreqFile = options.getArgument(0);
  std::string weightFile = options.getArgument(1);

  try {

    csl::DictSearch dictSearch;

    OCRCorrection::FrequencyList freqlist(histFreqFile.c_str(),
                                          weightFile.c_str());

    dictSearch.initHypothetic(patternFile.c_str());

    csl::DictSearch::DictModule& modernMod =
      dictSearch.addDictModule(L"modern", modernDict);
    // modernMod.setDLevWordlengths();
    // modernMod.setDLev( 2 );
    modernMod.setPriority(100);
    modernMod.setMaxNrOfPatterns(2);

    // modernMod.setDLevHypothetic( 1 );

    std::wstring query;
    csl::DictSearch::CandidateSet cands;
    while (std::getline(std::wcin, query).good()) {
      cands.reset();
      dictSearch.query(query, &cands);
      std::sort(cands.begin(), cands.end());
      for (csl::DictSearch::CandidateSet::const_iterator cand = cands.begin();
           cand != cands.end();
           ++cand) {
        if (cand - cands.begin() > 10)
          break;
        std::wcout << cand->toString() << std::endl
                   << L"   interpretationCount="
                   << freqlist.getInterpretationCount(*cand) << std::endl
                   << L"   baseWordFreq="
                   << freqlist.getBaseWordFrequency(cand->getBaseWord())
                   << std::endl
                   << L"   instructionProb="
                   << freqlist.getInstructionProb(cand->getInstruction())
                   << std::endl
                   << L"   actually returned freq value="
                   << freqlist.getInterpretationFrequency(*cand) << std::endl
                   << std::endl;
      }
    }
    if (errno == EILSEQ) {
      throw std::runtime_error("OCRC::queryFrequencyList: Input encodig error");
    }
    return 0;

  } catch (std::exception& exc) {
    std::wcout << "queryFrequencyList: Caught exception: " << exc.what()
               << std::endl;
    throw exc;
  }
}
