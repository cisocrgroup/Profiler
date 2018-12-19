#ifndef CSL_GETOPT_H
#define CSL_GETOPT_H CSL_GETOPT_H

#include <iostream>
#include <map>
#include <set>
#include <stdexcept>
#include <vector>

#include "Utils/Utils.h"
#include <Global.h>

namespace csl {

/**
 * @brief A class for the parsing of command line arguments.
 */
class Getopt
{
public:
  enum ValueType
  {
    VOID,
    STRING
  };
  enum Obligatority
  {
    OBLIGATORY,
    OPTIONAL
  };

  /**
   * @name Constructors
   */
  //@{

  /**
   * A standard constructor
   */
  Getopt() {}

  /**
   * @brief This constructor also carries out the command line parsing. No
   * detailed specification of flags is possible here.
   *
   * This is mainly for backwards compatibility.
   *
   */
  Getopt(size_t argc, char const** argv)
  {

    progName_ = argv[0];

    std::string openFlag;
    for (size_t i = 1; i < argc; ++i) {
      std::string word = argv[i];

      if (word.length() > 2 && word.at(0) == '-' && word.at(1) == '-') {
        if (!openFlag.empty()) {
          optionValues_[openFlag] = "1";
          openFlag.clear();
        }

        size_t pos = word.find('=');
        if (pos != std::string::npos) {
          optionValues_[word.substr(2, pos - 2)] = word.substr(pos + 1);
        } else {
          openFlag = word.substr(2);
        }
      } else {
        if (openFlag.empty()) {
          addArgument(word);
        } else {
          optionValues_[openFlag] = word;
          openFlag.clear();
        }
      }
    }

    // 	std::cout<<"Options:"<<std::endl;
    // 	for( std::map< std::string, std::string >::iterator it =
    // options_.begin(); it != options_.end(); ++it ) {
    // std::cout<<it->first<<" = "<<it->second<<std::endl;
    // 	}
    // 	std::cout<<"arguments:"<<std::endl;
    // 	for( std::vector< std::string >::iterator it = arguments_.begin(); it !=
    // arguments_.end(); ++it ) { 	    std::cout<<*it<<std::endl;
    // 	}
  }

  //@} end Constructors

  /**
   * @name Configure options
   */
  //@{

  /**
   * @brief specify an option.
   *
   * @param key  the name of the key
   * @param valueType  the valueType, one of VOID or STRING
   * @param obligatority  one of OBLIGATORY or OPTIONAL
   *
   */
  void specifyOption(std::string const& key,
                     ValueType valueType,
                     Obligatority obligatority = OPTIONAL)
  {
    if (optionTypes_.find(key) != optionTypes_.end()) {
      std::string msg = std::string("csl::Getopt::specifyOption: Key ") + key +
                        "was specified before";
      throw Exception("msg");
    }
    optionTypes_[key] = valueType;
    if (obligatority == OBLIGATORY) {
      obligatory_.insert(key);
    }
  }

  /**
   * @brief Specify an option with a defaultValue
   *
   * This obviously makes the option OPTIONAL: If it's not there ,the default
   * valkue is used.
   *
   * @param key
   * @param valueType
   * @param defaultValue
   */
  void specifyOption(std::string const& key,
                     ValueType valueType,
                     std::string const& defaultValue)
  {
    if (optionTypes_.find(key) != optionTypes_.end()) {
      std::string msg = std::string("csl::Getopt::specifyOption: Key ") + key +
                        "was specified before";
      throw Exception("msg");
    }
    optionTypes_[key] = valueType;
    optionValues_[key] = defaultValue;
  }

  void setOption(std::string const& key, std::string const& value)
  {
    optionValues_[key] = value;
  }

  //@} end configure options

  /**
   * @name Trigger Command line Parsing
   */
  //@{

  /**
   * @brief This option permits only options of the form option=value, and
   * regular arguments.
   */
  void getOptions(size_t argc, char const** argv)
  {

    progName_ = argv[0];

    for (size_t i = 1; i < argc; ++i) {
      std::string word = argv[i];

      if (word.length() > 2 && word.at(0) == '-' &&
          word.at(1) == '-') { // looks like a flag
        size_t pos = word.find('=');
        if (pos != std::string::npos) {
          optionValues_[word.substr(2, pos - 2)] = word.substr(pos + 1);
        } else {
          throw Exception(
            "csl::Getopt::getOptions: invalid argument structure");
        }
      } else { // is no flag
        addArgument(word);
      }
    } // for all items in argv

  } // getOptions

  void getOptionsAsSpecified(size_t argc, char const** argv)
  {

    progName_ = argv[0];

    std::string keyWaitsForValue;

    for (size_t i = 1; i < argc; ++i) {
      std::string word = argv[i];

      if (word.length() > 2 && word.at(0) == '-' &&
          word.at(1) == '-') { // looks like a flag
        if (!keyWaitsForValue.empty()) {
          std::string msg = "csl::Getopt::getOptionsAsSpecified: expected "
                            "value after non-void option ";
          msg += keyWaitsForValue;
          throw Exception(msg);
        }

        std::string key;
        size_t pos = word.find('=');
        if (pos != std::string::npos) { // '=' inside the string
          key = word.substr(2, pos - 2);
          std::string value = word.substr(pos + 1);

          if (optionTypes_.find(key) == optionTypes_.end()) {
            std::string msg =
              "csl::Getopt::getOptionsAsSpecified: unknown option ";
            msg += key;
            throw Exception(msg);
          } else if (optionTypes_[key] == VOID) {
            std::string msg = "csl::Getopt::getOptionsAsSpecified: no value "
                              "expected for void option ";
            msg += key;
            throw Exception(msg);
          }
          optionValues_[key] = value;
        } else { // no '=' inside the string
          key = word.substr(2);
          if (optionTypes_.find(key) == optionTypes_.end()) {
            std::string msg =
              "csl::Getopt::getOptionsAsSpecified: unknown option: '";
            msg += key + "'";
            throw Exception(msg);
          } else if (optionTypes_[key] == VOID) {
            optionValues_[key] = "1";
          } else {
            keyWaitsForValue = word.substr(2);
          }
        }
        std::set<std::string>::iterator ob = obligatory_.find(key);
        if (ob != obligatory_.end())
          obligatory_.erase(ob);
      } else { // is no flag
        if (!keyWaitsForValue.empty() &&
            (optionTypes_[keyWaitsForValue] !=
             VOID)) { // insert as value of open key
          optionValues_[keyWaitsForValue] = word;
          keyWaitsForValue.clear();
        } else { // no flag waiting for value, add as argument
          addArgument(word);
        }
      }
    } // for all items in argv

    if (!keyWaitsForValue.empty()) {
      std::string msg = "csl::Getopt::getOptionsAsSpecified: expected value "
                        "for non-void option ";
      msg += keyWaitsForValue;
      throw Exception(msg);
    }

    if (!obligatory_.empty()) {
      std::string msg = std::string("csl::Getopt::getOptionsAsSpecified: "
                                    "obligatory options still missing: ");
      for (std::set<std::string>::const_iterator it = obligatory_.begin();
           it != obligatory_.end();
           ++it) {
        msg += *it + ",";
      }

      throw Exception(msg);
    }

    // 	std::cout<<"Options:"<<std::endl;
    // 	for( std::map< std::string, std::string >::iterator it =
    // optionValues_.begin(); it != optionValues_.end(); ++it ) {
    // 	    std::cout<<it->first<<" = "<<it->second<<std::endl;
    // 	}
    // 	std::cout<<"arguments:"<<std::endl;
    // 	for( std::vector< std::string >::iterator it = arguments_.begin(); it !=
    // arguments_.end(); ++it ) { 	    std::cout<<*it<<std::endl;
    // 	}
  } // getOptions

  //@} end trigger command line parsing

  /**
   * @name Access options and arguments
   */
  //@{

  void addArgument(std::string value) { arguments_.push_back(value); }

  const std::string& getProgName() const { return progName_; }

  /**
   * @brief returns true iff an option with the specified key exists.
   * @param key
   * @return true iff an option with the specified key exists.
   */
  bool hasOption(const std::string& key)
  {
    return (optionValues_.find(key) != optionValues_.end());
  }

  /**
   * @brief returns the string value of the option with the specified key.
   * @param key
   * @return the string value of the option with the specified key.
   * @throws a std::runtime_error if the specified key is not defined.
   */
  const std::string& getOption(const std::string& key)
  {
    if (!hasOption(key))
      throw std::runtime_error("csl::Getopt::getOption: no such key defined");
    return optionValues_[key];
  }

  const std::string& operator[](const std::string& key)
  {
    return getOption(key);
  }

  typedef std::map<std::string, ValueType>::const_iterator OptionIterator;

  OptionIterator optionsBegin() const { return optionTypes_.begin(); }

  OptionIterator optionsEnd() const { return optionTypes_.end(); }

  const std::string& getArgument(size_t idx) const
  {
    if (idx >= getArgumentCount())
      throw std::runtime_error("csl::Getopt::getArgument: out of range.");
    return arguments_.at(idx);
  }

  const size_t getArgumentCount() const { return arguments_.size(); }

  void print(std::wostream& os)
  {
    for (OptionIterator it = optionsBegin(); it != optionsEnd(); ++it) {
      if (it->second == VOID) {
        os << OCRCorrection::Utils::utf8(it->first) << " = "
           << "true" << std::endl;
      } else {
        os << OCRCorrection::Utils::utf8(it->first) << " = "
           << OCRCorrection::Utils::utf8(getOption(it->first)) << std::endl;
      }
    }
  }

  //@}

  class Exception : public exceptions::cslException
  {
  public:
    Exception(std::string const& what)
      : cslException(what)
    {}
  };

private:
  std::string progName_;
  std::map<std::string, ValueType> optionTypes_;
  std::set<std::string> obligatory_;
  std::map<std::string, std::string> optionValues_;
  std::vector<std::string> arguments_;

}; // class Getopt

} // eon
#endif
