#include <iostream>
#include <fstream>
#include <iomanip>
#include <regex>
#include <sys/stat.h>
#include <system_error>
#include "Llocs.hpp"

const static std::regex llocsre{"\\.llocs$"};

////////////////////////////////////////////////////////////////////////////////
OCRCorrection::Llocs::Content
OCRCorrection::Llocs::read(const char *path)
{
     static const std::wregex re{L"(.)?\\s+(\\S+)\\s*(\\S+)?"};

     Content content;
     std::wifstream is(path);
     if (not is.good())
          throw std::runtime_error(
               std::string("Could not open Llocs file: ") + path);

     std::wsmatch m;
     Triple t;
     std::wstring line;
     while (std::getline(is, line)) {
          // must match
          if (not std::regex_match(line, m, re))
               throw std::runtime_error(
                    std::string(path) + " Invalid llocs line");

          // char
          if (m[1].length()) {
               t.c = *m[1].first;
          } else {
               t.c = L' ';
          }
          // confidence
          if (m[3].length()) {
               t.conf = std::stod(m[3]);
          } else {
               t.conf = 0;
          }
          // offset
          t.offset = std::stod(m[2]);
          content.push_back(t);
     }
     return content;
}

////////////////////////////////////////////////////////////////////////////////
OCRCorrection::Llocs::Llocs(const char *path)
     : path_(path)
     , content_(read(path))
     , pred_()
     , gt_()
     , conf_(NAN)
     , error_(NAN)
     , val_(0)
{
}

////////////////////////////////////////////////////////////////////////////////
double
OCRCorrection::Llocs::error() const
{
     if (std::isnan(error_))
          error_ = calculateError();
     return double(pred().size()) - error_;
}

////////////////////////////////////////////////////////////////////////////////
double
OCRCorrection::Llocs::calculateError() const
{
     using std::begin;
     using std::end;
     auto accum = [](double acc, const Triple& t) {return acc + t.conf;};
     auto sum = std::accumulate(begin(content_), end(content_), 0.0, accum);
     //return std::round(sum);
     return sum;
}

////////////////////////////////////////////////////////////////////////////////
double
OCRCorrection::Llocs::conf() const
{
     if (std::isnan(conf_))
          conf_ = calculateConf();
     return conf_;
}

////////////////////////////////////////////////////////////////////////////////
double
OCRCorrection::Llocs::calculateConf() const
{
     const auto acc = [](double a, const Triple& t) {
          return a + t.conf;
     };
     using std::begin;
     using std::end;
     const double sum = std::accumulate(begin(content_), end(content_),
                                        0.0, acc);
     return sum / double(content_.size());
}

////////////////////////////////////////////////////////////////////////////////
const std::wstring&
OCRCorrection::Llocs::pred() const
{
     if (pred_.empty() and not content_.empty())
          pred_ = readPred();
     return pred_;
}

////////////////////////////////////////////////////////////////////////////////
std::wstring
OCRCorrection::Llocs::readPred() const
{
     std::wstring res(content_.size(), 0);
     using std::begin;
     using std::end;
     std::transform(begin(content_), end(content_),
                    begin(res), [](const Triple& t) {return t.c;});
     return res;
}

////////////////////////////////////////////////////////////////////////////////
const std::wstring&
OCRCorrection::Llocs::gt() const
{
     if (gt_.empty() and not content_.empty())
          gt_ = readGt();
     return gt_;
}

////////////////////////////////////////////////////////////////////////////////
bool
OCRCorrection::Llocs::hasGt() const
{
     return gt() != L"\0\0\0";
}

////////////////////////////////////////////////////////////////////////////////
std::wstring
OCRCorrection::Llocs::readGt() const
{
     const auto gtfile = std::regex_replace(path_, llocsre, ".gt.txt");
     std::wifstream is(gtfile);
     if (not is.good()) {
          std::wcerr << "[warning] could not open: "
                     << gtfile.data() << "\n";
          return L"\0\0\0";
     }

     std::wstring gt;
     std::getline(is, gt);
     is.close();
     return gt;
}

////////////////////////////////////////////////////////////////////////////////
void
OCRCorrection::Llocs::copy(const char *odir, size_t n, bool autocorrect) const
{
     mkdir(odir);
     const auto fn = filename(n);
     const auto imagefile = imagename();
     const auto oimage = std::string(odir) + "/" + fn + ".bin.png";
     const auto ogt = std::string(odir) + "/" + fn + ".gt.txt";
     const auto oconf = std::string(odir) + "/" + fn + ".conf.txt";

     const auto dewimage = imagename(".dew.png");
     struct stat st;
     if (stat(dewimage.data(), &st) != -1) {
          const auto odewimage = std::string(odir) + "/" + fn + ".dew.png";
          copyimg(dewimage, odewimage);
     }
     copyimg(imagefile, oimage);
     if (autocorrect)
          write(gt(), ogt);
     else
          write(pred(), ogt);
     write(std::to_wstring(conf()), oconf);
}

////////////////////////////////////////////////////////////////////////////////
void
OCRCorrection::Llocs::write(const std::wstring& str, const std::string& path)
{
     std::wofstream os(path);
     if (not os.good())
          throw std::system_error(errno, std::system_category(), path);
     os << str << std::endl;
     os.close();
}

////////////////////////////////////////////////////////////////////////////////
void
OCRCorrection::Llocs::copyimg(const std::string& src, const std::string& dest)
{
     std::ifstream is(src, std::ios::binary);
     if (not is.good())
          throw std::system_error(errno, std::system_category(), src);
     std::ofstream os(dest, std::ios::binary);
     if (not os.good())
          throw std::system_error(errno, std::system_category(), dest);
     os << is.rdbuf();
     is.close();
     os.close();
}

////////////////////////////////////////////////////////////////////////////////
std::string
OCRCorrection::Llocs::filename(size_t n)
{
     std::ostringstream os;
     os << "01" << std::setfill('0') << std::setw(4) << std::hex << n;
     return os.str();
}

////////////////////////////////////////////////////////////////////////////////
std::string
OCRCorrection::Llocs::imagename(const char *what) const
{
     return std::regex_replace(path_, llocsre, what);

}

////////////////////////////////////////////////////////////////////////////////
void
OCRCorrection::Llocs::mkdir(const char *odir)
{
     struct stat st;
     int status = 0;
     if (stat(odir, &st) != 0) {
          if (::mkdir(odir, 0775) != 0 and errno != EEXIST)
               throw std::system_error(errno, std::system_category(), odir);
     } else if (not S_ISDIR(st.st_mode)) {
          throw std::system_error(ENOTDIR, std::system_category(), odir);
     }
}

////////////////////////////////////////////////////////////////////////////////
std::wostream&
OCRCorrection::operator<<(std::wostream& os, const Llocs& llocs)
{
     return os << "[" << llocs.path() << "]\n"
               << "pred: " << llocs.pred() << "\n"
               << "  gt: " << llocs.gt() << "\n"
               << "conf: [" << llocs.conf() << "]\n"
               << " val: [" << llocs.val() << "]\n";
}
