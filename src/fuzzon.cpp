/*
 * Copyright [2017] Jacek Weremko
 */

#include "./fuzzon.h"

#include <string>
#include <vector>

#include "./fuzzon_generator.h"
#include "./fuzzon_mutator.h"
#include "./fuzzon_random.h"
#include "./utils/logger.h"

namespace fuzzon {

#define DIR_NAME_CRASH "crash"
#define DIR_NAME_CORPUS "corpus"
#define DIR_NAME_PATHS "paths"
#define DIR_NAME_TMP "tmp"

Fuzzon::Fuzzon(std::string output_dir,
               std::string sut_path,
               int sut_runtime_timeout)
    : output_dir_(output_dir),
      corpus_(output_dir),
      execution_monitor_(sut_path, sut_runtime_timeout) {
  Logger::Get()->info("Base directory is " + output_dir_);
}

void Fuzzon::TestInput(std::string test_me) {
  auto new_test_case = TestCase(test_me);
  auto execution_data = execution_monitor_.ExecuteBlocking(new_test_case);
  corpus_.AddIfInteresting(execution_data);
  return;
}

void Fuzzon::Generation(std::string input_format, int test_cases_to_generate) {
  Logger::Get()->info("Generation start!");
  Logger::Get()->info("input_format : " + input_format);
  Generator test_cases_generator(input_format);
  while (test_cases_to_generate--) {
    auto new_test_case = test_cases_generator.generateNext();
    auto execution_data = execution_monitor_.ExecuteBlocking(new_test_case);
    corpus_.AddIfInteresting(execution_data);
  }
  Logger::Get()->info("Generation finished!");
  return;
}

void Fuzzon::MutationDeterministic(bool white_chars_preservation) {
  Logger::Get()->info("MutationDeterministic start!");
  bool all_posibilities_checked = false;
  Mutator test_cases_mutator(white_chars_preservation);
  while (!all_posibilities_checked) {
    auto favorite = corpus_.SelectNotYetExhaustMutated();
    if (favorite == nullptr) {
      all_posibilities_checked = true;
      break;
    }

    // walking: exhaust bit flips
    for (auto exp = 0; exp <= 2; ++exp) {
      const auto bits_to_flip = std::pow(2, exp);
      for (auto bite_idx = 0; bite_idx < favorite->length_bit(); ++bite_idx) {
        auto mutate_me = *favorite;
        if (test_cases_mutator.FlipBit(mutate_me, bite_idx, bits_to_flip)) {
          auto execution_data = execution_monitor_.ExecuteBlocking(mutate_me);
          corpus_.AddIfInteresting(execution_data);
        }
      }
    }

    // walking: exhaust byte flips
    for (auto exp = 0; exp <= 2; ++exp) {
      const auto bytes_to_flip = std::pow(2, exp);
      for (auto byte_idx = 0; byte_idx < favorite->length_byte(); ++byte_idx) {
        auto mutate_me = *favorite;
        if (test_cases_mutator.FlipByte(mutate_me, byte_idx, bytes_to_flip)) {
          auto execution_data = execution_monitor_.ExecuteBlocking(mutate_me);
          corpus_.AddIfInteresting(execution_data);
        }
      }
    }

    // walking: simple arithmetic
    for (auto value = -35; value <= 35; value += (2 * 35)) {
      for (auto byte_idx = 0; byte_idx < favorite->length_byte(); ++byte_idx) {
        auto mutate_me = *favorite;
        if (test_cases_mutator.SimpleArithmetics(mutate_me, byte_idx, value)) {
          auto execution_data = execution_monitor_.ExecuteBlocking(mutate_me);
          corpus_.AddIfInteresting(execution_data);
        }
      }
    }

    // std::vector<int> interesting_limits = { 1, 7,8,9, 15,16,17, 31,32,33,
    // 63,64,65, 127,128,129 };
    std::vector<int> interesting_limits = {-1, 16, 32, 64, 127};
    for (const auto& value : interesting_limits) {
      for (auto byte_idx = 0; byte_idx < favorite->length_byte(); ++byte_idx) {
        auto mutate_me = *favorite;
        if (test_cases_mutator.KnownIntegers(mutate_me, byte_idx, value)) {
          auto execution_data = execution_monitor_.ExecuteBlocking(mutate_me);
          corpus_.AddIfInteresting(execution_data);
        }
      }
    }

    // something more?
  }
  Logger::Get()->info("MutationDeterministic finished!");
  return;
}

void Fuzzon::MutationNonDeterministic(int test_cases_to_mutate,
                                      bool white_chars_preservation) {
  Logger::Get()->info("MutationNonDeterministic start!");
  Mutator test_cases_mutator(white_chars_preservation);
  bool stop_testing = false;

  while (!stop_testing) {
    auto favorite = corpus_.SelectFavorite();
    auto mutated = test_cases_mutator.Mutate(*favorite);
    auto execution_data = execution_monitor_.ExecuteBlocking(mutated);
    corpus_.AddIfInteresting(execution_data);

    if (test_cases_to_mutate > 0) {
      test_cases_to_mutate--;
    } else if (test_cases_to_mutate == 0) {
      stop_testing = true;
    }
  }
  Logger::Get()->info("MutationNonDeterministic finished!");
  return;
}

}  // namespace fuzzon
