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
               Coverage::TrackMode track_mode,
               int test_timeout,
               int max_testcases)
    : output_dir_(output_dir),
      test_timeout_(std::chrono::milliseconds(test_timeout)),
      max_testcases_(max_testcases),
      corpus_(output_dir),
      execution_monitor_(sut_path,
                         env_flags,
                         sut_runtime_timeout,
                         mode,
                         track_mode) {
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
      if (shall_finish()) {
        break;
      }
      LOG_DEBUG("Seed : " + file_path.string());

      boost::filesystem::ifstream file(file_path);
      std::string content((std::istreambuf_iterator<char>(file)),
                          std::istreambuf_iterator<char>());
      auto new_test_case = TestCase(content, TestCase::CorpusSeed);
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
  auto new_test_case = TestCase(test_me, TestCase::Predefined);
  auto execution_data = execution_monitor_.ExecuteBlocking(new_test_case);
  corpus_.AddIfInteresting(execution_data);
  return;
}

void Fuzzon::Generation(std::string input_format, int test_cases_to_generate) {
  LOG_INFO("input_format : " + input_format);
  LOG_INFO(corpus_.GetShortStats().str() + " <- generation start");
  Generator generation_engine(input_format);
  while (!shall_finish() && test_cases_to_generate) {
    test_cases_to_generate--;

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
  while (!shall_finish() && !all_posibilities_checked) {
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
        auto mutate_me = TestCase(*favorite, TestCase::MutationDeterministic);
        if (test_cases_mutator.FlipBit(mutate_me, bite_idx, bits_to_flip)) {
          auto execution_data = execution_monitor_.ExecuteBlocking(mutate_me);
          corpus_.AddIfInteresting(execution_data);
          //          favorite->increased_mutation_counter();
        }
      }
    }

    // walking: exhaust byte flips
    for (auto exp = 0; exp <= 2; ++exp) {
      const auto bytes_to_flip = std::pow(2, exp);
      for (auto byte_idx = 0; byte_idx < favorite->length_byte(); ++byte_idx) {
        auto mutate_me = TestCase(*favorite, TestCase::MutationDeterministic);
        if (test_cases_mutator.FlipByte(mutate_me, byte_idx, bytes_to_flip)) {
          auto execution_data = execution_monitor_.ExecuteBlocking(mutate_me);
          corpus_.AddIfInteresting(execution_data);
          //          favorite->increased_mutation_counter();
        }
      }
    }

    // walking: simple arithmetic
    for (auto exp = 0; exp <= 2; ++exp) {
      const auto word_size = std::pow(2, exp);
      for (auto value = -35; value <= 35; value += (2 * 35)) {
        for (auto byte_idx = 0; byte_idx < favorite->length_byte();
             ++byte_idx) {
          auto mutate_me = TestCase(*favorite, TestCase::MutationDeterministic);
          if (test_cases_mutator.SimpleArithmetics(mutate_me, byte_idx, value,
                                                   word_size)) {
            auto execution_data = execution_monitor_.ExecuteBlocking(mutate_me);
            corpus_.AddIfInteresting(execution_data);
            //          favorite->increased_mutation_counter();
          }
        }
      }
    }

    // std::vector<int> interesting_limits = { 1, 7,8,9, 15,16,17, 31,32,33,
    // 63,64,65, 127,128,129 };
    std::vector<int> interesting_limits = {-1, 16, 32, 64, 127};
    for (auto exp = 0; exp <= 2; ++exp) {
      const auto word_size = std::pow(2, exp);
      for (const auto& value : interesting_limits) {
        for (auto byte_idx = 0; byte_idx < favorite->length_byte();
             ++byte_idx) {
          auto mutate_me = TestCase(*favorite, TestCase::MutationDeterministic);
          if (test_cases_mutator.KnownIntegers(mutate_me, byte_idx, value,
                                               word_size)) {
            auto execution_data = execution_monitor_.ExecuteBlocking(mutate_me);
            corpus_.AddIfInteresting(execution_data);
            //          favorite->increased_mutation_counter();
          }
        }
      }
    }

    // something more?
  }
  LOG_INFO(corpus_.GetShortStats().str() + " <- mutation deterministic finish");
  return;
}

void Fuzzon::MutationJSONBased(int test_cases_to_mutate,
                               bool white_chars_preservation) {
  LOG_INFO(corpus_.GetShortStats().str() + " <- mutation json based start");
  Mutator engine(white_chars_preservation);

  LOG_INFO(corpus_.GetShortStats().str() + " <- mutation json based finish");
  return;
}

void Fuzzon::MutationNonDeterministic(int test_cases_to_mutate,
                                      bool white_chars_preservation) {
  LOG_INFO(corpus_.GetShortStats().str() +
           " <- mutation non-deterministic start");
  Mutator engine(white_chars_preservation);

  while (!shall_finish() && test_cases_to_mutate >= 0) {
    auto favorite = corpus_.SelectFavorite();
    if (favorite == nullptr) {
      break;
    }

    auto f_i = static_cast<double>(favorite->path_execution_coutner_);
    auto s_i =
        std::min(static_cast<double>(favorite->input.mutation_counter()), 30.0);
    static const double M = 64;
    int energy = static_cast<int>(std::min((std::pow(2.0f, s_i) / f_i), M));

    // just to not
    energy = std::min(energy, test_cases_to_mutate);

    if (energy == 0) {
      energy++;
    }
    // doesn't matter before of after a loop
    // just put it out there to prevent too quick expansion
    favorite->input.increased_mutation_counter();

    for (auto ei = 0; ei < energy; ++ei) {
      test_cases_to_mutate--;
      auto mutate_me =
          TestCase(favorite->input, TestCase::MutationNonDeterministic);

      auto mutation_idx = Random::Get()->GenerateInt(0, 8);
      auto mutations_count = 1;
      if (mutation_idx == 8) {
        mutations_count += Random::Get()->GenerateInt(1, 3);
      }

      for (auto i = 0; i < mutations_count; ++i) {
        switch (Random::Get()->GenerateInt(0, 7)) {
          case 0: {
            auto start_idx =
                Random::Get()->GenerateInt(1, mutate_me.size() * 8);
            auto count = Random::Get()->GenerateInt(1, 4);
            auto success = engine.FlipBit(mutate_me, start_idx, count);
            break;
          }
          case 1: {
            auto start_idx = Random::Get()->GenerateInt(1, mutate_me.size());
            auto count = Random::Get()->GenerateInt(1, 4);
            auto success = engine.FlipByte(mutate_me, start_idx, count);
            break;
          }
          case 2: {
            auto byte_idx = Random::Get()->GenerateInt(1, mutate_me.size());
            auto value = Random::Get()->GenerateInt();
            auto count = Random::Get()->GenerateInt(1, 4);
            auto success =
                engine.SimpleArithmetics(mutate_me, byte_idx, value, count);
            break;
          }
          case 3: {
            auto byte_idx = Random::Get()->GenerateInt(1, mutate_me.size());
            auto value = Random::Get()->GenerateInt();
            auto count = Random::Get()->GenerateInt(1, 4);
            auto success =
                engine.KnownIntegers(mutate_me, byte_idx, value, count);
            break;
          }
          case 4: {
            auto base_idx = Random::Get()->GenerateInt(1, mutate_me.size());
            auto insertme = corpus_.SelectRandom();
            if (insertme == nullptr) {
              break;
            }
            auto insertme_idx = Random::Get()->GenerateInt(1, insertme->size());
            auto block_length = Random::Get()->GenerateInt(
                0, static_cast<int>(insertme->size() - insertme_idx));
            auto success = engine.BlockInsertion(mutate_me, base_idx, *insertme,
                                                 insertme_idx, block_length);
            break;
          }
          case 5: {
            auto start_idx = Random::Get()->GenerateInt(1, mutate_me.size());
            auto block_length = Random::Get()->GenerateInt(
                0, static_cast<int>(mutate_me.size() - start_idx));
            auto success =
                engine.BlockDeletion(mutate_me, start_idx, block_length);
            break;
          }
          case 6: {
            auto start_idx = Random::Get()->GenerateInt(1, mutate_me.size());
            auto block_length = Random::Get()->GenerateInt(
                0, static_cast<int>(mutate_me.size() - start_idx));
            auto new_value = 0;
            auto success = engine.BlockMemset(mutate_me, start_idx,
                                              block_length, new_value);
            break;
          }
          case 7: {
            auto base_idx = Random::Get()->GenerateInt(1, mutate_me.size());
            auto insertme = corpus_.SelectRandom();
            if (insertme == nullptr) {
              break;
            }
            auto insertme_idx = Random::Get()->GenerateInt(1, insertme->size());
            auto block_length = Random::Get()->GenerateInt(
                1, static_cast<int>(insertme->size()));
            auto success = engine.BlockOverriding(
                mutate_me, base_idx, *insertme, insertme_idx, block_length);
            break;
          }
          default:
            break;
        }
      }

      auto execution_data = execution_monitor_.ExecuteBlocking(mutate_me);
      auto is_interesting = corpus_.AddIfInteresting(execution_data);
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
