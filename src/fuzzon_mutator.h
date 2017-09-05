/*
 * Copyright [2017] Jacek Weremko
 */

#ifndef SRC_FUZZON_MUTATOR_H_
#define SRC_FUZZON_MUTATOR_H_

#include "./fuzzon_testcase.h"

#include <algorithm>
#include <limits>
#include <string>
#include <vector>

#define SAFE_CHECK_ENABLE 1

#ifdef SAFE_CHECK_ENABLE
#define SAFE_CHECK(condition, return_value) \
  if ((condition) == false) {               \
    return (return_value);                  \
  }
#else
#define SAFE_CHECK(condition, return_value)
#endif

namespace fuzzon {

class Mutator {
 public:
  explicit Mutator(std::string input_alphabet =
                       "abcdefghijklmnopqrstuvwxyz"
                       "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                       "1234567890");

  TestCase Mutate(const TestCase& mutate_me);

  int ChangeByte(uint8_t* data, size_t data_size);

  bool FlipBit(std::vector<char>& base,
               size_t bit_start_idx,
               size_t bits_count) const {
    SAFE_CHECK(base.size() > (bit_start_idx) / 8, false);
    SAFE_CHECK(base.size() > (bit_start_idx + bits_count - 1) / 8, false);

    for (auto i = 0; i < bits_count; ++i) {
      const auto& byte_idx = (bit_start_idx + i) / 8;
      auto& selected_byte = base[byte_idx];
      selected_byte = selected_byte ^ (1 << ((bit_start_idx + i) % 8));
    }
    return true;
  }

  bool FlipByte(std::vector<char>& base,
                size_t byte_start_idx,
                size_t bytes_count) const {
    SAFE_CHECK(base.size() > byte_start_idx, false);
    SAFE_CHECK(base.size() > byte_start_idx + bytes_count - 1, false);

    for (size_t i = 0; i < bytes_count; ++i) {
      const auto& byte_index = byte_start_idx + i;
      auto& selected_byte = base[byte_index];
      selected_byte = ~selected_byte;
    }
    return true;
  }

  bool SimpleArithmetics(std::vector<char>& base,
                         size_t byte_idx,
                         char value) const {
    SAFE_CHECK(base.size() > byte_idx, false);

    auto& selected_byte = base[byte_idx];
    selected_byte += value;
    return true;
  }

  bool KnownIntegers(std::vector<char>& base,
                     size_t byte_idx,
                     char value) const {
    SAFE_CHECK(base.size() > byte_idx, false);

    auto& selected_byte = base[byte_idx];
    selected_byte = value;
    return true;
  }

  bool BlockInsertion(std::vector<char>& base,
                      size_t base_start_idx,
                      const std::vector<char>& insert_me,
                      size_t insert_me_start_idx,
                      size_t block_length) {
    SAFE_CHECK(base.size() > base_start_idx, false);
    SAFE_CHECK(insert_me.size() > insert_me_start_idx, false);
    SAFE_CHECK(insert_me.size() > insert_me_start_idx + block_length - 1,
               false);

    base.insert(base.begin() + base_start_idx,
                insert_me.begin() + insert_me_start_idx,
                insert_me.begin() + insert_me_start_idx + block_length);
    return true;
  }

  bool BlockDeletion(std::vector<char>& base,
                     size_t start_idx,
                     size_t block_length) const {
    SAFE_CHECK(base.size() > start_idx, false);
    SAFE_CHECK(base.size() > start_idx + block_length, false);

    base.erase(base.begin() + start_idx,
               base.begin() + start_idx + block_length);
    return true;
  }

  bool BlockMemset(std::vector<char>& base,
                   size_t start_idx,
                   size_t block_length,
                   char new_value) const {
    SAFE_CHECK(base.size() > start_idx, false);
    SAFE_CHECK(base.size() > start_idx + block_length - 1, false);

    std::for_each(base.begin() + start_idx,
                  base.begin() + start_idx + block_length,
                  [new_value](char& current) { current = new_value; });
    return true;
  }

  bool BlockOverriding(std::vector<char>& base,
                       size_t base_start_idx,
                       std::vector<char>& new_values,
                       size_t new_values_start_idx,
                       size_t block_length) const {
    SAFE_CHECK(base.size() > base_start_idx, false);
    SAFE_CHECK(base.size() > base_start_idx + block_length - 1, false);
    SAFE_CHECK(new_values.size() > new_values_start_idx, false);
    SAFE_CHECK(new_values.size() > new_values_start_idx + block_length - 1,
               false);

    std::copy(base.begin() + base_start_idx,
              base.begin() + base_start_idx + block_length,
              new_values.begin() + new_values_start_idx);
    return true;
  }

 private:
  bool type_preservation_;
  std::string input_alphabet_;
};

}  // namespace fuzzon

#endif  // SRC_FUZZON_MUTATOR_H_
