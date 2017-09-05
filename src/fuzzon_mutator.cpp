/*
 * Copyright [2017] Jacek Weremko
 */

#include "./fuzzon_mutator.h"

#include <limits>
#include <string>

#include "./fuzzon_random.h"
#include "./utils/logger.h"

namespace fuzzon {

Mutator::Mutator(std::string input_alphabet)
    : type_preservation_(true), input_alphabet_(input_alphabet) {}

TestCase Mutator::Mutate(const TestCase& mutate_me) {
  auto new_test_case = TestCase(mutate_me);
  // TODO: add strategy selection
  //  auto result = ChangeByte(new_test_case.data(),
  //  new_test_case.length_byte());
  return new_test_case;
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
    Logger::Get()->debug("mutation_idx ==mutation_guard");
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
