#ifndef OCRCorrection_NoThousandGrouping_h__
#define OCRCorrection_NoThousandGrouping_h__
namespace OCRCorrection {
  struct NoThousandGrouping: public std::numpunct<wchar_t> {
  protected:
    wchar_t do_thousands_sep() const /*override*/ {return L'\0';}
  };

  struct NoThousandGroupingWithDecimalDot: public NoThousandGrouping {
  protected:
    wchar_t do_decimal_point() const /*override*/ {return L'.';}
  };
}

#endif // OCRCorrection_NoThousandGrouping_h__
