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
  Logger::Get(output_dir_);
  Logger::Get()->info("Base directory is " + output_dir_);
}

void Fuzzon::GenerationPhase(std::string input_format,
                             int test_cases_to_generate) {
  Logger::Get()->info("input_format : " + input_format);
  Generator test_cases_generator(input_format);
  while (test_cases_to_generate--) {
    auto new_test_case = test_cases_generator.generateNext();
    auto execution_data = execution_monitor_.ExecuteBlocking(new_test_case);
    corpus_.AddIfInteresting(execution_data);
  }
  return;
}

void Fuzzon::MutationPhaseDeterministic() {
  bool all_posibilities_checked = false;
  Mutator test_cases_mutator;
  while (!all_posibilities_checked) {
    auto favorite_ = corpus_.SelectNotMutated();
    if (favorite_ == NULL) {
      all_posibilities_checked = true;
      break;
    }
    auto not_mutated = *favorite_;

    // walking: exhaust bit flips
    for (auto bits_to_flip_exp = 0; bits_to_flip_exp <= 2; ++bits_to_flip_exp) {
      const auto bits_to_flip = std::pow(2, bits_to_flip_exp);
      // for(auto bite_index = 0; bite_index <= (not_mutated.length_bit() - 1 -
      // (bits_to_flip - 1)); ++bite_index)
      for (auto bite_index = 0; bite_index < not_mutated.length_bit();
           ++bite_index) {
        auto mutated = not_mutated;
        // test_cases_mutator.FlipBit(mutated.data(),
        // mutated.length_byte(), bite_index, bits_to_flip);

        auto execution_data = execution_monitor_.ExecuteBlocking(mutated);
        corpus_.AddIfInteresting(execution_data);
      }
    }

    // walking: exhaust byte flips
    for (auto bytes_to_flip_exp = 0; bytes_to_flip_exp <= 2;
         ++bytes_to_flip_exp) {
      const auto bytes_to_flip = std::pow(2, bytes_to_flip_exp);
      // for(auto byte_index = 0; byte_index < (not_mutated.length_byte() - 1 -
      // (bytes_to_flip - 1)); ++byte_index)
      for (auto byte_index = 0; byte_index < not_mutated.length_byte();
           ++byte_index) {
        auto mutated = not_mutated;
        //        test_cases_mutator.FlipByte(mutated.data(),
        //        mutated.length_byte(),
        //                                    byte_index, bytes_to_flip);

        auto execution_data = execution_monitor_.ExecuteBlocking(mutated);
        corpus_.AddIfInteresting(execution_data);
      }
    }

    // walking: simple arithmetic
    for (auto value = -35; value <= 35; value += (2 * 35)) {
      for (auto byte_index = 0; byte_index < not_mutated.length_byte() - 1;
           ++byte_index) {
        auto mutated = not_mutated;
        //        test_cases_mutator.SimpleArithmetics(
        //            mutated.data(), mutated.length_byte(), byte_index, value);

        auto execution_data = execution_monitor_.ExecuteBlocking(mutated);
        corpus_.AddIfInteresting(execution_data);
      }
    }

    {
      // std::vector<int> interesting_limits = { 1, 7,8,9, 15,16,17, 31,32,33,
      // 63,64,65, 127,128,129 };
      std::vector<int> interesting_limits = {-1, 16, 32, 64, 127};
      for (const auto& value : interesting_limits) {
        for (auto byte_index = 0; byte_index < not_mutated.length_byte() - 1;
             ++byte_index) {
          auto mutated = not_mutated;
          //          test_cases_mutator.KnownIntegers(
          //              mutated.data(), mutated.length_byte(), byte_index,
          //              value);

          auto execution_data = execution_monitor_.ExecuteBlocking(mutated);
          corpus_.AddIfInteresting(execution_data);
        }
      }
    }
  }
  return;
}

void Fuzzon::MutationPhaseNonDeterministic(int test_cases_to_mutate) {
  Mutator test_cases_mutator;
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
  return;
}

}  // namespace fuzzon
