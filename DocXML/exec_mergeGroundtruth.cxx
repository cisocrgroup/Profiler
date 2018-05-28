#include <DocXML/DocXMLReader.h>
#include <DocXML/DocXMLWriter.h>
#include <DocXML/MergedGroundtruthReader.h>
#include <Document/Document.h>
#include <Getopt/Getopt.h>
int
main(int argc, char const** argv)
{

  std::locale::global(std::locale(""));

  try {

    csl::Getopt options;
    options.getOptionsAsSpecified(argc, argv);

    if (options.getArgumentCount() != 2) {
      std::wcerr << "Use like: mergeGroundtruth masterDoc addInfoDoc"
                 << std::endl;
      return 1;
    }

    OCRCorrection::MergedGroundtruthReader mgr;

    OCRCorrection::DocXMLReader reader;

    OCRCorrection::Document master;
    reader.parse(argv[1], &master);

    OCRCorrection::Document addInfo;
    reader.parse(argv[2], &addInfo);

    mgr.mergeDocuments(&master, addInfo);

    OCRCorrection::DocXMLWriter writer;

    std::wostream& os = std::wcout;
    if (!os.good()) {
      throw OCRCorrection::OCRCException("Could not write to output stream.");
    }
    writer.writeXML(master, os);

    return EXIT_SUCCESS;

  } catch (OCRCorrection::OCRCException& exc) {
    std::wcerr << "mergeGroundtruth: Caught exception: " << exc.what()
               << std::endl;
    return EXIT_FAILURE;
  } catch (std::exception& exc) {
    std::wcerr << "mergeGroundtruth: Caught std::exception. " << std::endl;
    return EXIT_FAILURE;
  }
}
