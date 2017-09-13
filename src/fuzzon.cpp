/*
 * Copyright [2017] Jacek Weremko
 */

#include "./fuzzon.h"

#include <boost/filesystem.hpp>

#include <string>
#include <vector>
#include <chrono>
#include <utility>
#include <algorithm>

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
               const std::vector<std::string>& env_flags,
               int sut_runtime_timeout,
               Executor::Mode mode,
               int test_timeout)
    : output_dir_(output_dir),
      test_timeout_(std::chrono::milliseconds(test_timeout)),
      corpus_(output_dir),
      execution_monitor_(sut_path, env_flags, sut_runtime_timeout, mode) {
  LOG_INFO("Base directory is " + output_dir_);
}

void Fuzzon::ScanCorpus(std::string corpus_base) {
  LOG_INFO("corpus_base : " + corpus_base);
  LOG_INFO(corpus_.GetShortStats().str() + " <- corpus seeds testing start");
  if (boost::filesystem::exists(corpus_base)) {
    std::vector<boost::filesystem::path> corpus_seeds;
    boost::filesystem::path corpus_seeds_dir(corpus_base);

    for (const auto& file :
         boost::filesystem::directory_iterator(corpus_seeds_dir)) {
      corpus_seeds.push_back(file.path());
    }

    for (const auto& file_path : corpus_seeds) {
      if (test_timeout_()) {
        break;
      }
      LOG_DEBUG("Seed : " + file_path.string());

      boost::filesystem::ifstream file(file_path);
      std::string content((std::istreambuf_iterator<char>(file)),
                          std::istreambuf_iterator<char>());
      auto new_test_case = TestCase(content);
      auto execution_data = execution_monitor_.ExecuteBlocking(new_test_case);
      corpus_.AddIfInteresting(execution_data);
    }
  } else {
    LOG_WARNING("Provided corpus path does not exist.");
  }
  LOG_INFO(corpus_.GetShortStats().str() + " <- corpus seeds testing finish");
  return;
}

void Fuzzon::TestInput(std::string test_me) {
  auto new_test_case = TestCase(test_me);
  auto execution_data = execution_monitor_.ExecuteBlocking(new_test_case);
  corpus_.AddIfInteresting(execution_data);
  return;
}

void Fuzzon::Generation(std::string input_format, int test_cases_to_generate) {
  LOG_INFO("input_format : " + input_format);
  LOG_INFO(corpus_.GetShortStats().str() + " <- generation start");
  Generator generation_engine(input_format);
  while (!test_timeout_() && test_cases_to_generate--) {
    auto new_test_case = generation_engine.generateNext();
    auto execution_data = execution_monitor_.ExecuteBlocking(new_test_case);
    corpus_.AddIfInteresting(execution_data);
  }
  LOG_INFO(corpus_.GetShortStats().str() + " <- generation finish");
  return;
}

void Fuzzon::MutationDeterministic(int level, bool white_chars_preservation) {
  LOG_INFO(corpus_.GetShortStats().str() + " <- mutation deterministic start");
  bool all_posibilities_checked = false;
  Mutator test_cases_mutator(white_chars_preservation);
  while (!test_timeout_() && !all_posibilities_checked) {
    auto favorite = corpus_.SelectNotYetExhaustMutated();
    if (favorite == nullptr) {
      all_posibilities_checked = true;
      break;
    }

    if (level == 0) {
      continue;
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
  LOG_INFO(corpus_.GetShortStats().str() + " <- mutation deterministic finish");
  return;
}

void Fuzzon::MutationNonDeterministic(int test_cases_to_mutate,
                                      bool white_chars_preservation) {
  LOG_INFO(corpus_.GetShortStats().str() +
           " <- mutation non-deterministic start");
  Mutator mutation_engine(white_chars_preservation);
  bool stop_testing = false;

  while (!test_timeout_() && !stop_testing) {
    auto favorite = corpus_.SelectFavorite();
    if (favorite == nullptr) {
      break;
    }

    auto new_test_case = TestCase(*favorite);
    auto mutations_count = Random::Get()->GenerateInt(1, 5);
    for (auto i = 0; i < mutations_count; ++i) {
      auto mutation_idx = Random::Get()->GenerateInt(0, 7);
      switch (mutation_idx) {
        case 0: {
          auto bit_start_idx =
              Random::Get()->GenerateInt(1, new_test_case.size() * 8);
          auto bits_count = Random::Get()->GenerateInt(1, 4);
          auto success =
              mutation_engine.FlipBit(new_test_case, bit_start_idx, bits_count);
          break;
        }
        case 1: {
          auto byte_start_idx =
              Random::Get()->GenerateInt(1, new_test_case.size());
          auto bytes_count = Random::Get()->GenerateInt(1, 4);
          auto success = mutation_engine.FlipByte(new_test_case, byte_start_idx,
                                                  bytes_count);
          break;
        }
        case 2: {
          auto byte_idx = Random::Get()->GenerateInt(1, new_test_case.size());
          auto value = Random::Get()->GenerateChar();
          auto success =
              mutation_engine.SimpleArithmetics(new_test_case, byte_idx, value);
          break;
        }
        case 3: {
          auto byte_idx = Random::Get()->GenerateInt(1, new_test_case.size());
          auto value = Random::Get()->GenerateChar();
          auto success =
              mutation_engine.KnownIntegers(new_test_case, byte_idx, value);
          break;
        }
        case 4: {
          auto base_idx = Random::Get()->GenerateInt(1, new_test_case.size());
          auto insertme = corpus_.SelectRandom();
          if (insertme == nullptr) {
            break;
          }
          auto insertme_idx = Random::Get()->GenerateInt(1, insertme->size());
          auto block_length = Random::Get()->GenerateInt(
              0, static_cast<int>(insertme->size() - insertme_idx));
          auto success = mutation_engine.BlockInsertion(
              new_test_case, base_idx, *insertme, insertme_idx, block_length);
          break;
        }
        case 5: {
          auto start_idx = Random::Get()->GenerateInt(1, new_test_case.size());
          auto block_length = Random::Get()->GenerateInt(
              0, static_cast<int>(new_test_case.size() - start_idx));
          auto success = mutation_engine.BlockDeletion(new_test_case, start_idx,
                                                       block_length);
          break;
        }
        case 6: {
          auto start_idx = Random::Get()->GenerateInt(1, new_test_case.size());
          auto block_length = Random::Get()->GenerateInt(
              0, static_cast<int>(new_test_case.size() - start_idx));
          auto new_value = 0;
          auto success = mutation_engine.BlockMemset(new_test_case, start_idx,
                                                     block_length, new_value);
          break;
        }
        case 7: {
          auto base_idx = Random::Get()->GenerateInt(1, new_test_case.size());
          auto insertme = corpus_.SelectRandom();
          if (insertme == nullptr) {
            break;
          }
          auto insertme_idx = Random::Get()->GenerateInt(1, insertme->size());
          auto block_length =
              Random::Get()->GenerateInt(1, static_cast<int>(insertme->size()));
          auto success = mutation_engine.BlockOverriding(
              new_test_case, base_idx, *insertme, insertme_idx, block_length);
          break;
        }
        default:
          break;
      }
    }

    auto execution_data = execution_monitor_.ExecuteBlocking(new_test_case);
    corpus_.AddIfInteresting(execution_data);

    if (test_cases_to_mutate > 0) {
      test_cases_to_mutate--;
    } else if (test_cases_to_mutate == 0) {
      stop_testing = true;
    }
  }
  LOG_INFO(corpus_.GetShortStats().str() +
           " <- mutation non-deterministic finish");
  return;
}

void Fuzzon::PrintStats() {
  LOG_INFO(corpus_.GetFullStats().str());
  return;
}

void Fuzzon::Dump() {
  corpus_.Dump();
  return;
}

}  // namespace fuzzon
