#ifndef OCRCorrection_NoThousandGrouping_h__
#define OCRCorrection_NoThousandGrouping_h__
namespace OCRCorrection {
  struct NoThousandGrouping: public std::numpunct<wchar_t> {
  protected:
    wchar_t do_thousands_sep() const /*override*/ {return L'\0';}
  };
}

#endif // OCRCorrection_NoThousandGrouping_h__
