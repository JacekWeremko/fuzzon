/*
 * Copyright [2017] Jacek Weremko
 */

#include "./fuzzon_mutator.h"

#include <limits>
#include <string>
#include <algorithm>
#include <vector>

#include "./fuzzon_random.h"
#include "./utils/logger.h"

namespace fuzzon {

Mutator::Mutator(bool white_chars_preservation,
                 bool type_preservation,
                 std::string output_alphabet)
    : white_chars_preservation_(white_chars_preservation),
      type_preservation_(type_preservation),
      output_alphabet_(output_alphabet) {}

bool Mutator::ChangeAllowed(const char& check_me) const {
  if (white_chars_preservation_) {
    return !std::isspace(check_me);
  }
  return true;
}

bool Mutator::FlipBit(std::vector<char>& base,
                      size_t bit_start_idx,
                      size_t bits_count) const {
  SAFE_CHECK(base.size() > (bit_start_idx) / 8, false);
  SAFE_CHECK(base.size() > (bit_start_idx + bits_count - 1) / 8, false);

  for (auto i = 0; i < bits_count; ++i) {
    const auto& byte_idx = (bit_start_idx + i) / 8;
    auto& selected_byte = base[byte_idx];
    if (!ChangeAllowed(selected_byte)) {
      return false;
    }
    selected_byte = selected_byte ^ (1 << ((bit_start_idx + i) % 8));
  }
  return true;
}

bool Mutator::FlipByte(std::vector<char>& base,
                       size_t byte_start_idx,
                       size_t bytes_count) const {
  SAFE_CHECK(base.size() > byte_start_idx, false);
  SAFE_CHECK(base.size() > byte_start_idx + bytes_count - 1, false);

  for (size_t i = 0; i < bytes_count; ++i) {
    const auto& byte_index = byte_start_idx + i;
    auto& selected_byte = base[byte_index];
    if (!ChangeAllowed(selected_byte)) {
      return false;
    }
    selected_byte = ~selected_byte;
  }
  return true;
}

bool Mutator::SimpleArithmetics(std::vector<char>& base,
                                size_t byte_idx,
                                char value) const {
  SAFE_CHECK(base.size() > byte_idx, false);

  auto& selected_byte = base[byte_idx];
  if (!ChangeAllowed(selected_byte)) {
    return false;
  }
  selected_byte += value;
  return true;
}

bool Mutator::KnownIntegers(std::vector<char>& base,
                            size_t byte_idx,
                            char value) const {
  SAFE_CHECK(base.size() > byte_idx, false);

  auto& selected_byte = base[byte_idx];
  if (!ChangeAllowed(selected_byte)) {
    return false;
  }
  selected_byte = value;
  return true;
}

bool Mutator::BlockInsertion(std::vector<char>& base,
                             size_t base_start_idx,
                             const std::vector<char>& insertme,
                             size_t insertme_start_idx,
                             size_t block_length) const {
  SAFE_CHECK(base.size() > base_start_idx, false);
  SAFE_CHECK(insertme.size() > insertme_start_idx, false);
  SAFE_CHECK(insertme.size() > insertme_start_idx + block_length - 1, false);

  base.insert(base.begin() + base_start_idx,
              insertme.begin() + insertme_start_idx,
              insertme.begin() + insertme_start_idx + block_length);
  return true;
}

bool Mutator::BlockDeletion(std::vector<char>& base,
                            size_t start_idx,
                            size_t block_length) const {
  SAFE_CHECK(base.size() > start_idx, false);
  SAFE_CHECK(base.size() > start_idx + block_length, false);

  base.erase(base.begin() + start_idx, base.begin() + start_idx + block_length);
  return true;
}

bool Mutator::BlockMemset(std::vector<char>& base,
                          size_t start_idx,
                          size_t block_length,
                          char new_value) const {
  SAFE_CHECK(base.size() > start_idx, false);
  SAFE_CHECK(base.size() > start_idx + block_length - 1, false);

  std::for_each(base.begin() + start_idx,
                base.begin() + start_idx + block_length,
                [this, new_value](char& current) {
                  if (ChangeAllowed(current)) {
                    current = new_value;
                  }
                });
  return true;
}

bool Mutator::BlockOverriding(std::vector<char>& base,
                              size_t base_start_idx,
                              const std::vector<char>& new_values,
                              size_t new_values_start_idx,
                              size_t block_length) const {
  SAFE_CHECK(base.size() > base_start_idx, false);
  SAFE_CHECK(base.size() > base_start_idx + block_length - 1, false);
  SAFE_CHECK(new_values.size() > new_values_start_idx, false);
  SAFE_CHECK(new_values.size() > new_values_start_idx + block_length - 1,
             false);

  std::copy(base.begin() + base_start_idx,
            base.begin() + base_start_idx + block_length,
            const_cast<std::vector<char>&>(new_values).begin() +
                new_values_start_idx);
  return true;
}

int Mutator::ChangeByte(uint8_t* data, size_t data_size) {
  const size_t mutation_guard = 1000;
  char selected_byte = '\0';
  int byte_to_flip = 0;
  size_t mutation_idx = 0;
  for (; mutation_idx < mutation_guard; mutation_idx++) {
    byte_to_flip = Random::Get()->GenerateInt(0, data_size - 1);
    selected_byte = data[byte_to_flip];
    if (input_alphabet_.find(selected_byte) != std::string::npos) {
      break;
    }
  }
  if (mutation_idx == mutation_guard) {
    LOG_DEBUG("mutation_idx ==mutation_guard");
  }

  for (size_t mutation_idx = 0; mutation_idx < mutation_guard; mutation_idx++) {
    const auto new_char = type_preservation_ == true
                              ? Random::Get()->GenerateChar(selected_byte)
                              : Random::Get()->GenerateChar();
    if (new_char == data[byte_to_flip]) {
      continue;
    }
    data[byte_to_flip] = new_char;
    break;
  }
  return byte_to_flip;
}

} /* namespace fuzzon */
