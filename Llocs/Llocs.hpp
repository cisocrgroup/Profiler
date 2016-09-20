#ifndef ocrc_Llocs_hpp__
#define ocrc_Llocs_hpp__

#include <list>

namespace OCRCorrection {
     class Llocs {
     public:
          using Sptr = std::shared_ptr<Llocs>;
          struct Triple {
               wchar_t c;
               double offset, conf;
          };
          using Content = std::list<Triple>; // use list to keep iterators valid
          using const_iterator = Content::const_iterator;
          using iterator = Content::iterator;

          Llocs(const char *path);

          const char *path() const {return path_;}
          double val() const {return val_;}
          void setVal(double val) {val_ = val;}
          double conf() const;
          double error() const;
          const std::wstring& pred() const;
          const std::wstring& gt() const;
          bool hasGt() const;

          void copy(const char *odir, size_t n, bool autocorr = false) const;
          std::string imagename(const char *what = ".bin.png") const;

          size_t size() const {return content_.size();}
          const Content& content() const {return content_;}
          Content& content() {return content_;}
          const_iterator begin() const {return content_.begin();}
          const_iterator end() const {return content_.end();}
          iterator begin() {return content_.begin();}
          iterator end() {return content_.end();}
          void invalidate() {pred_.clear(); gt_.clear();}
     private:
          static Content read(const char *path);
          static void mkdir(const char *odir);
          static std::string filename(size_t n);
          static void copyimg(const std::string& str,
                              const std::string& path);
          static void write(const std::wstring& str,
                            const std::string& path);

          double calculateConf() const;
          double calculateError() const;
          std::wstring readPred() const;
          std::wstring readGt() const;

          const char *path_;
          Content content_;
          mutable std::wstring pred_, gt_;
          mutable double conf_, error_;
          double val_;
     };
     std::wostream& operator<<(std::wostream& os, const Llocs& llocs);
}

#endif // ocrc_Llocs_hpp__
