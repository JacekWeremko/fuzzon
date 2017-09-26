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
  explicit Mutator(bool white_chars_preservation = false,
                   bool type_preservation = false,
                   std::string output_alphabet =
                       "abcdefghijklmnopqrstuvwxyz"
                       "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                       "1234567890");

  int ChangeByte(uint8_t* data, size_t data_size);

  /*
   * FlipBit mutations
   */
  bool FlipBit(std::vector<char>& base,
               size_t bit_start_idx,
               size_t bits_count) const;

  bool FlipBit(TestCase& base, size_t bit_start_idx, size_t bits_count) const {
    return FlipBit(base.vec(), bit_start_idx, bits_count);
  }

  /*
   * FlipByte mutations
   */
  bool FlipByte(std::vector<char>& base,
                size_t byte_start_idx,
                size_t bytes_count) const;

  bool FlipByte(TestCase& base,
                size_t byte_start_idx,
                size_t bytes_count) const {
    return FlipByte(base.vec(), byte_start_idx, bytes_count);
  }

  /*
   * SimpleArithmetics mutations
   */
  bool SimpleArithmetics(std::vector<char>& base,
                         size_t byte_idx,
                         int value,
                         int value_size = 1) const;

  bool SimpleArithmetics(TestCase& base,
                         size_t byte_idx,
                         int value,
                         int value_size) const {
    return SimpleArithmetics(base.vec(), byte_idx, value, value_size);
  }

  /*
   * KnownIntegers mutations
   */
  bool KnownIntegers(std::vector<char>& base,
                     size_t byte_idx,
                     int value,
                     int value_size = 1) const;

  bool KnownIntegers(TestCase& base,
                     size_t byte_idx,
                     int value,
                     int value_size) const {
    return KnownIntegers(base.vec(), byte_idx, value, value_size);
  }

  /*
   * BlockInsertion mutations
   */
  bool BlockInsertion(std::vector<char>& base,
                      size_t base_start_idx,
                      const std::vector<char>& insertme,
                      size_t insertme_start_idx,
                      size_t block_length) const;
  bool BlockInsertion(TestCase& base,
                      size_t base_start_idx,
                      TestCase& insertme,
                      size_t insertme_start_idx,
                      size_t block_length) const {
    return BlockInsertion(base.vec(), base_start_idx, insertme.vec(),
                          insertme_start_idx, block_length);
  }

  /*
   * BlockDeletion mutations
   */
  bool BlockDeletion(std::vector<char>& base,
                     size_t start_idx,
                     size_t block_length) const;

  bool BlockDeletion(TestCase& base,
                     size_t start_idx,
                     size_t block_length) const {
    return BlockDeletion(base.vec(), start_idx, block_length);
  }

  /*
   * BlockMemset mutations
   */
  bool BlockMemset(std::vector<char>& base,
                   size_t start_idx,
                   size_t block_length,
                   char new_value) const;

  bool BlockMemset(TestCase& base,
                   size_t start_idx,
                   size_t block_length,
                   char new_value) const {
    return BlockMemset(base.vec(), start_idx, block_length, new_value);
  }

  /*
   * BlockOverriding mutations
   */
  bool BlockOverriding(std::vector<char>& base,
                       size_t base_start_idx,
                       const std::vector<char>& new_values,
                       size_t new_values_start_idx,
                       size_t block_length) const;

  bool BlockOverriding(TestCase& base,
                       size_t base_start_idx,
                       TestCase& new_values,
                       size_t new_values_start_idx,
                       size_t block_length) const {
    return BlockOverriding(base.vec(), base_start_idx, new_values.vec(),
                           new_values_start_idx, block_length);
  }

 private:
  bool ChangeAllowed(const char& check_me) const;

  const bool white_chars_preservation_;
  const bool type_preservation_;
  std::string output_alphabet_;
  std::string input_alphabet_;
};

}  // namespace fuzzon

#endif  // SRC_FUZZON_MUTATOR_H_
