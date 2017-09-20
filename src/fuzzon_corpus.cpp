/*
 * Copyright [2017] Jacek Weremko
 */

#include "./fuzzon_corpus.h"

#include <boost/assert.hpp>
#include <boost/filesystem.hpp>
#include <utils/time.hpp>

#include <string>
#include <vector>
#include <set>
#include <limits>
#include <algorithm>
#include <numeric>
#include <memory>
#include <utility>

#include "./fuzzon_random.h"
#include "./utils/logger.h"

namespace fs = boost::filesystem;
namespace stdch = std::chrono;

namespace fuzzon {
Corpus::Corpus(std::string output_path)
    : start_(stdch::system_clock::now()), output_path_(fs::path(output_path)) {
  boost::filesystem::create_directories(output_path_);
  boost::filesystem::create_directories(output_path_ / DIR_NAME_RESULTS);
  boost::filesystem::create_directories(output_path_ / DIR_NAME_CORPUS);
  boost::filesystem::create_directories(output_path_ / DIR_NAME_CRASH);
  boost::filesystem::create_directories(output_path_ / DIR_NAME_TIMEOUE);
}

bool Corpus::IsInteresting(const ExecutionData& am_i) {
  auto result = true;
  for (auto& current : data_) {
    if (current->path == am_i.path) {
      current->path_execution_coutner_++;

      /* In order to avoid memory consumption issues related to path explosion
       * paths are compressed...anyways for two compressed path crash or timeout
       * may or may not occur separately
       */
      if ((current->crashed() == am_i.crashed()) &&
          (current->timeout == am_i.timeout)) {
        result = false;
        break;
      }
    }
  }

  LOG_DEBUG(am_i.string());
  summary_.test_cases += 1;
  summary_.none_zero_error_code += am_i.error_code.value() != 0 ? 1 : 0;
  summary_.none_zero_return_code += am_i.exit_code != 0 ? 1 : 0;
  summary_.timeout += am_i.timeout == true ? 1 : 0;
  summary_.crash += am_i.crashed() == true ? 1 : 0;
  summary_.test_cases_genesis[am_i.input.my_genesis()]++;
  return result;
}

void Corpus::AddExecutionData(ExecutionDataSP add_me_to_corpus) {
  LOG_DEBUG("Adding new test case to corpus: " +
            add_me_to_corpus->input.string());

  summary_.total_cov.Merge(add_me_to_corpus->path);
  data_.push_back(add_me_to_corpus);
}

bool Corpus::AddIfInteresting(ExecutionDataSP add_me_to_corpus) {
  bool result = false;
  if (IsInteresting(*add_me_to_corpus.get())) {
    AddExecutionData(add_me_to_corpus);
    result = true;
  }

  static Timeout log_timer(stdch::milliseconds(1000));
  if (log_timer()) {
    LOG_INFO(GetShortStats().str());
    log_timer.arm(stdch::system_clock::now());
  }

  return result;
}

ExecutionData* const Corpus::SelectFavorite() {
  BOOST_ASSERT(data_.size() > 0);

  std::vector<ExecutionDataSP> viable_elements;

  std::copy_if(data_.begin(), data_.end(), std::back_inserter(viable_elements),
               [](const ExecutionDataSP& elem) {
                 return elem->crashed() == false && elem->timeout == false;
               });

  std::vector<ExecutionDataSP> lowest_paths_coutner;
  // find min f(i) - lowest frequency path
  {
    auto lowest_path_execution_coutner = std::numeric_limits<size_t>::max();
    for (auto& current : viable_elements) {
      if (current->path_execution_coutner_ < lowest_path_execution_coutner) {
        lowest_paths_coutner.clear();
        lowest_path_execution_coutner = current->path_execution_coutner_;
        lowest_paths_coutner.push_back(current);
      } else if (current->path_execution_coutner_ ==
                 lowest_path_execution_coutner) {
        lowest_paths_coutner.push_back(current);
      }
    }
  }

  std::vector<ExecutionDataSP> lowest_mutation_counter;
  // find min s(i) - test cases used as mutation base
  {
    auto lowest_mutation_count = std::numeric_limits<size_t>::max();
    for (auto& current : lowest_paths_coutner) {
      if (current->input.mutation_counter() < lowest_mutation_count) {
        lowest_mutation_counter.clear();
        lowest_mutation_count = current->input.mutation_counter();
        lowest_mutation_counter.push_back(current);
      } else if (current->input.mutation_counter() == lowest_mutation_count) {
        lowest_mutation_counter.push_back(current);
      }
    }
  }

  ExecutionDataSP result = nullptr;
  // find min t(i) time and size
  {
    auto lowest_time_and_size = std::numeric_limits<double>::max();
    for (auto& current : lowest_mutation_counter) {
      double current_time_and_size =
          current->execution_time.count() * current->input.length_byte();
      if (current_time_and_size < lowest_time_and_size) {
        result = current;
      }
    }
  }
  if (result != nullptr) {
    return result.get();
  }
  return nullptr;
}

TestCase* const Corpus::SelectRandom() {
  auto i = 0;
  auto idx = Random::Get()->GenerateInt(0, data_.size() - 1);
  for (auto& current : data_) {
    if (i == idx) {
      return &current->input;
    }
    i++;
  }
  return nullptr;
}

TestCase* const Corpus::SelectNotYetExhaustMutated() {
  for (auto& current : data_) {
    if (current->input.not_yet_mutated() == false) {
      return &current->input;
    }
  }
  return nullptr;
}

std::stringstream Corpus::GetShortStats() {
  std::stringstream stats;
  stats << time_format(stdch::duration_cast<stdch::microseconds>(
               stdch::system_clock::now() - start_))
        << "    test_cases:" << summary_.test_cases
        << "    corpus_size:" << std::to_string(data_.size())
        << "    coverage:" << summary_.total_cov.GetVisitedPCCounter() << "/"
        << summary_.total_cov.GetTotalPCCounter() << "  "
        << (static_cast<double>(summary_.total_cov.GetVisitedPCCounter()) /
            static_cast<double>(summary_.total_cov.GetTotalPCCounter())) *
               100
        << "%";
  return stats;
}

std::stringstream Corpus::GetFullStats() {
  std::stringstream stats;
  // yey...cashing those data would be useful...
  stats << std::endl
        << "Campaign Summary : " << std::endl
        << "  Test cases: " << summary_.test_cases << std::endl
        << "  None zero error code: " << summary_.none_zero_error_code
        << std::endl
        << "  None zero return code: " << summary_.none_zero_return_code
        << std::endl
        << "  Timeout: " << summary_.timeout << std::endl
        << "  Crash: " << summary_.crash << std::endl
        << "  Test case genesis: " /*yeah....could be map in corpus..*/
        << std::endl
        << "    Predefined: "
        << summary_.test_cases_genesis[TestCase::Predefined] << std::endl
        << "    CorpusSeed: "
        << summary_.test_cases_genesis[TestCase::CorpusSeed] << std::endl
        << "    Generation: "
        << summary_.test_cases_genesis[TestCase::Generation] << std::endl
        << "    MutationDeterministic: "
        << summary_.test_cases_genesis[TestCase::MutationDeterministic]
        << std::endl
        << "    MutationNonDeterministic: "
        << summary_.test_cases_genesis[TestCase::MutationNonDeterministic]
        << std::endl
        << std::endl;

  stats << "Corpus Summary : " << std::endl
        << "  Test cases : " << std::to_string(data_.size()) << std::endl
        << "  None zero error code: "
        << std::count_if(data_.begin(), data_.end(),
                         [](const ExecutionDataSP& arg) {
                           return arg->error_code.value() != 0;
                         })
        << std::endl

        << "  None zero return code: "
        << std::count_if(
               data_.begin(), data_.end(),
               [](const ExecutionDataSP& arg) { return arg->exit_code != 0; })
        << std::endl

        << "  Timeout: " << std::count_if(data_.begin(), data_.end(),
                                          [](const ExecutionDataSP& arg) {
                                            return arg->timeout == true;
                                          })
        << std::endl

        << "  Crash: " << std::count_if(data_.begin(), data_.end(),
                                        [](const ExecutionDataSP& arg) {
                                          return arg->crashed();
                                        })
        << std::endl
        << "  Total coverage: "
        << (static_cast<double>(summary_.total_cov.GetVisitedPCCounter()) /
            static_cast<double>(summary_.total_cov.GetTotalPCCounter())) *
               100
        << "%" << std::endl
        << "  Visited trace points: "
        << summary_.total_cov.GetVisitedPCCounter() << std::endl
        << "  Total   trace points: " << summary_.total_cov.GetTotalPCCounter()
        << std::endl;

  if (data_.size() == 0) {
    return stats;
  }
  stats << "  Test case min/max mutation count: "
        << std::max_element(
               data_.begin(), data_.end(),
               [](const ExecutionDataSP& arg1, const ExecutionDataSP& arg2) {
                 return arg1->input.mutation_counter() >
                        arg2->input.mutation_counter();
               })
               ->get()
               ->input.mutation_counter()
        << "/"
        << std::max_element(
               data_.begin(), data_.end(),
               [](const ExecutionDataSP& arg1, const ExecutionDataSP& arg2) {
                 return arg1->input.mutation_counter() <
                        arg2->input.mutation_counter();
               })
               ->get()
               ->input.mutation_counter()
        << std::endl

        << "  Test case min/max path executions count: "
        << std::min_element(
               data_.begin(), data_.end(),
               [](const ExecutionDataSP& arg1, const ExecutionDataSP& arg2) {
                 return arg1->path_execution_coutner_ <
                        arg2->path_execution_coutner_;
               })
               ->get()
               ->path_execution_coutner_
        << "/"
        << std::max_element(
               data_.begin(), data_.end(),
               [](const ExecutionDataSP& arg1, const ExecutionDataSP& arg2) {
                 return arg1->path_execution_coutner_ <
                        arg2->path_execution_coutner_;
               })
               ->get()
               ->path_execution_coutner_
        << std::endl

        << "  Test case min/max execution time: "
        << time_format(std::max_element(data_.begin(), data_.end(),
                                        [](const ExecutionDataSP& arg1,
                                           const ExecutionDataSP& arg2) {
                                          return arg1->execution_time >
                                                 arg2->execution_time;
                                        })
                           ->get()
                           ->execution_time)
        << "/" << time_format(std::max_element(data_.begin(), data_.end(),
                                               [](const ExecutionDataSP& arg1,
                                                  const ExecutionDataSP& arg2) {
                                                 return arg1->execution_time <
                                                        arg2->execution_time;
                                               })
                                  ->get()
                                  ->execution_time)
        << std::endl
        << "  Test case genesis: " /*yeah....could be map in corpus..*/
        << std::endl
        << "    Predefined: "
        << std::count_if(data_.begin(), data_.end(),
                         [](const ExecutionDataSP& arg) {
                           return arg->input.my_genesis() ==
                                  TestCase::Genesis::Predefined;
                         })
        << std::endl
        << "    CorpusSeed: "
        << std::count_if(data_.begin(), data_.end(),
                         [](const ExecutionDataSP& arg) {
                           return arg->input.my_genesis() ==
                                  TestCase::Genesis::CorpusSeed;
                         })
        << std::endl
        << "    Generation: "
        << std::count_if(data_.begin(), data_.end(),
                         [](const ExecutionDataSP& arg) {
                           return arg->input.my_genesis() ==
                                  TestCase::Genesis::Generation;
                         })
        << std::endl
        << "    MutationDeterministic: "
        << std::count_if(data_.begin(), data_.end(),
                         [](const ExecutionDataSP& arg) {
                           return arg->input.my_genesis() ==
                                  TestCase::Genesis::MutationDeterministic;
                         })
        << std::endl
        << "    MutationNonDeterministic: "
        << std::count_if(data_.begin(), data_.end(),
                         [](const ExecutionDataSP& arg) {
                           return arg->input.my_genesis() ==
                                  TestCase::Genesis::MutationNonDeterministic;
                         })
        << std::endl;

  return stats;
}

void Corpus::Dump() {
  fs::ofstream(output_path_ / "stats.txt") << GetFullStats().str();
  fs::ofstream(output_path_ / DIR_NAME_RESULTS / "total.json")
      << summary_.total_cov;

  {
    auto index = 1;
    for (auto& elem : data_) {
      fs::ofstream(output_path_ / DIR_NAME_CORPUS /
                   fs::path(std::to_string(index) + ".txt"))
          << elem->input;

      {
        const auto result_path = output_path_ / DIR_NAME_RESULTS /
                                 fs::path(std::to_string(index) + ".json");

        fs::ofstream(result_path) << *elem;
      }

      if (elem->crashed()) {
        const auto crash_link = output_path_ / DIR_NAME_CRASH /
                                fs::path(std::to_string(index) + ".txt");
        fs::ofstream(crash_link) << *elem;
      }

      if (elem->timeout) {
        const auto crash_link = output_path_ / DIR_NAME_TIMEOUE /
                                fs::path(std::to_string(index) + ".txt");
        fs::ofstream(crash_link) << *elem;
      }

      index++;
    }
  }

  return;
}

std::vector<fs::path> Corpus::GatherCoprusFiles() const {
  std::vector<fs::path> list;
  fs::path corpus(output_path_ / DIR_NAME_CORPUS);

  for (const auto& file : fs::directory_iterator(corpus)) {
    list.push_back(file.path());
  }
  return list;
}

} /* namespace fuzzon */
