/*
 * Copyright [2017] Jacek Weremko
 */

#ifndef SRC_FUZZON_RANDOM_H_
#define SRC_FUZZON_RANDOM_H_

#include <boost/random.hpp>
#include <map>
#include <string>

namespace fuzzon {

class Random {
 public:
  enum AlphabetType {
    SmallLetters = 0,
    CapitalLetters,
    Nums,
    NumsSpecials,
    OtherSpecials,
    ASCII_0_31,
    Unknown,
  };

  Random(Random const&) = delete;
  void operator=(Random const&) = delete;

  static Random* Get() {
    static Random generator;
    return &generator;
  }

  void AddAlphabet(AlphabetType type);
  void SetAlphabet(std::string alphabet);

  std::string GenerateString(int length = -1);
  std::string GenerateString(int min = -1, int max = -1);
  std::string GenerateString(int min = -1,
                             int max = -1,
                             const std::string& alphabet = "");

  int GenerateInt(int min = -1, int max = -1);
  char GenerateChar();
  char GenerateChar(char same_type_as_me);
  char GenerateChar(const std::string& alphabet);

  AlphabetType FindCharType(char what_is_my_type);

 private:
  Random();

  boost::random::mt19937 generator_;
  std::string alphabet_;

  std::map<AlphabetType, std::string> alphabets_;
};

} /* namespace fuzzon */

#endif  // SRC_FUZZON_RANDOM_H_
