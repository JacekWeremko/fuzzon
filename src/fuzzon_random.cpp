/*
 * Copyright [2017] Jacek Weremko
 */

#include "./fuzzon_random.h"

#include <boost/algorithm/string/find.hpp>
#include <ctime>
#include <cstdint>
#include <string>
#include <limits>

#include "./utils/logger.h"

namespace fuzzon {

Random::Random() {
  generator_ = boost::random::mt19937(static_cast<std::uint32_t>(std::time(0)));

  char ascii_0_31[32];
  for (char i = 0; i < 31; ++i) {
    ascii_0_31[i] = i;
  }

  alphabets_ = {
      {AlphabetType::SmallLetters, "abcdefghijklmnopqrstuvwxyz"},
      {AlphabetType::CapitalLetters, "ABCDEFGHIJKLMNOPQRSTUVWXYZ"},
      {AlphabetType::Nums, "1234567890"},
      {AlphabetType::NumsSpecials, "!@#$%^&*()"},
      {AlphabetType::OtherSpecials, "`~-_=+[{]}\\|;:'\",<.>/? "},
      {AlphabetType::ASCII_0_31, std::string(ascii_0_31)},
  };
  alphabet_ = std::string(alphabets_[AlphabetType::SmallLetters]);

  //  alphabet_ = std::string(alphabets_[AlphabetType::SmallLetters] +
  //                          alphabets_[AlphabetType::CapitalLetters] +
  //                          alphabets_[AlphabetType::Nums]);

  //  alphabet_ = std::string(alphabets_[AlphabetType::SmallLetters] +
  //                          alphabets_[AlphabetType::CapitalLetters] +
  //                          alphabets_[AlphabetType::Nums] +
  //                          alphabets_[AlphabetType::NumsSpecials] +
  //                          alphabets_[AlphabetType::OtherSpecials]);
}

void Random::AddAlphabet(AlphabetType type) {
  alphabet_ += alphabets_[type];
}

void Random::SetAlphabet(std::string alphabet) {
  alphabet_ = alphabet;
}

int Random::GenerateInt(int min, int max) {
  static const int default_min = std::numeric_limits<int>::min();
  static const int default_max = std::numeric_limits<int>::max();

  BOOST_ASSERT(min <= max);
  min = min == -1 ? default_min : min;
  max = max == -1 ? default_max : max;

  boost::random::uniform_int_distribution<> min_to_max(min, max);
  return min_to_max(generator_);
}

std::string Random::GenerateString(int length) {
  std::stringstream random;
  for (auto i = 0; i < length; i++) {
    random << GenerateChar();
  }
  return random.str();
}

std::string Random::GenerateString(int min, int max) {
  static const int min_length = 1;
  static const int max_length = 100;

  BOOST_ASSERT(min <= max);
  min = min == -1 ? min_length : min;
  max = max == -1 ? max_length : max;

  const auto string_length = GenerateInt(min, max);
  std::stringstream random;
  for (auto i = 0; i < string_length; i++) {
    random << GenerateChar();
  }
  return random.str();
}

std::string Random::GenerateString(int min,
                                   int max,
                                   const std::string& alphabet) {
  static const int min_length = 1;
  static const int max_length = 100;

  BOOST_ASSERT(min <= max);
  min = min == -1 ? min_length : min;
  max = max == -1 ? max_length : max;

  const auto string_length = GenerateInt(min, max);
  std::stringstream random;
  if (alphabet == "") {
    for (auto i = 0; i < string_length; i++) {
      random << GenerateChar();  // use default
    }
  } else {
    for (auto i = 0; i < string_length; i++) {
      random << GenerateChar(alphabet);
    }
  }

  return random.str();
}

char Random::GenerateChar() {
  return GenerateChar(alphabet_);
}

char Random::GenerateChar(const std::string& alphabet) {
  return alphabet[GenerateInt(0, alphabet.size() - 1)];
}

char Random::GenerateChar(char same_type_as_me) {
  auto alphabet_type = FindCharType(same_type_as_me);
  if (alphabet_type != AlphabetType::Unknown) {
    const auto& alphabet = alphabets_[alphabet_type];
    return GenerateChar(alphabet);
  }
  LOG_CRITICAL("Alphabet not found! Generating random");
  return GenerateChar();
}

Random::AlphabetType Random::FindCharType(char what_is_my_type) {
  for (const auto& alphabet : alphabets_) {
    if (alphabet.second.find(what_is_my_type) != std::string::npos) {
      return alphabet.first;
    }
  }
  return AlphabetType::Unknown;
}

} /* namespace fuzzon */
