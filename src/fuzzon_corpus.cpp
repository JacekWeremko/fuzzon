/*
 * Copyright [2017] Jacek Weremko
 */

#include "./fuzzon_corpus.h"

#include <boost/assert.hpp>
#include <boost/filesystem.hpp>

#include <string>
#include <vector>
#include <set>
#include <limits>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <memory>

#include "./utils/logger.h"

#define DIR_NAME_CORPUS "corpus"

namespace fuzzon {

Corpus::Corpus(std::string output_path)
    : output_path_(
          (boost::filesystem::path(output_path) / DIR_NAME_CORPUS).string()) {
  boost::filesystem::create_directories(output_path_);
}

bool Corpus::IsInteresting(const ExecutionData& am_i) {
  static int counter = 0;
  const int statistics_print_interval = 1000;
  if (counter % statistics_print_interval == 0) {
    Logger::Get()->info("Progress: \r\n" + GetStatistics().str());
  }
  counter++;

  // TODO: calculate coverage (path) hash in order to improve performance
  for (auto& current : data_) {
    // if (current->second.coverage_.pc_flow_hash ==
    // am_i.coverage_.pc_flow_hash)
    if (current.coverage == am_i.coverage) {
      current.coverage_coutner_++;
      return false;
    }
  }
  return true;
}

void Corpus::AddExecutionData(ExecutionData& add_me_to_corpus) {
  Logger::Get()->info("Adding new test case to corpus: " +
                      add_me_to_corpus.input.string());

  // TODO: optimize memory footprint
  //  auto new_element = std::make_shared<ExecutionData>(add_me_to_corpus);
  //  data_.push_back(new_element);
  data_.push_back(add_me_to_corpus);
}

bool Corpus::AddIfInteresting(ExecutionData& add_me_to_corpus) {
  if (IsInteresting(add_me_to_corpus)) {
    AddExecutionData(add_me_to_corpus);
    return true;
  }
  return false;
}

const TestCase* Corpus::SelectFavorite() {
  BOOST_ASSERT(data_.size() > 0);

  // FIXME: dat should implement score_, container should sort automatically
  std::vector<ExecutionData*> data_copy_;
  for (auto& current : data_) {
    data_copy_.push_back(&current);
  }

  std::vector<ExecutionData*> lowest_usage_data;
  // find min s(i) - test cases used as mutation base
  {
    auto lowest_usage_count = std::numeric_limits<size_t>::max();
    for (auto& current : data_copy_) {
      if (current->mutation_counter_ < lowest_usage_count) {
        lowest_usage_data.clear();
        lowest_usage_count = current->mutation_counter_;
        lowest_usage_data.push_back(current);
      } else if (current->mutation_counter_ == lowest_usage_count) {
        lowest_usage_data.push_back(current);
      }
    }
  }

  std::vector<ExecutionData*> lowest_similar_execution_data;
  // find min f(i) - lowest frequency path
  {
    auto lowest_similar_execution_coutner = std::numeric_limits<size_t>::max();
    for (auto& current : lowest_usage_data) {
      if (current->coverage_coutner_ < lowest_similar_execution_coutner) {
        lowest_similar_execution_data.clear();
        lowest_similar_execution_coutner = current->coverage_coutner_;
        lowest_similar_execution_data.push_back(current);
      } else if (current->coverage_coutner_ ==
                 lowest_similar_execution_coutner) {
        lowest_similar_execution_data.push_back(current);
      }
    }
  }

  ExecutionData* result = nullptr;
  // find min t(i) time and size
  {
    auto lowest_time_and_size = std::numeric_limits<double>::max();
    for (auto& current : lowest_similar_execution_data) {
      double current_time_and_size =
          current->execution_time.count() * current->input.length_byte();
      if (current_time_and_size < lowest_time_and_size) {
        result = current;
      }
    }
  }
  return &result->input;
}

const TestCase* Corpus::SelectNotMutated() {
  for (auto& current : data_) {
    if (current.mutatation_exhausted == false) {
      current.mutatation_exhausted = true;
      return &current.input;
    }
  }
  return nullptr;
}

std::stringstream Corpus::GetStatistics() {
  std::stringstream stats;
  stats << "Corpus size : " << std::to_string(data_.size()) << std::endl;

  stats << "Tested cases: "
        << std::accumulate(data_.begin(), data_.end(), 0,
                           [](int execution_counter, ExecutionData& arg) {
                             return execution_counter + arg.coverage_coutner_;
                           })
        << std::endl;

  if (data_.size() == 0) {
    return stats;
  }

  stats << "Test case max mutation count: "
        << std::max_element(data_.begin(), data_.end(),
                            [](ExecutionData& arg1, ExecutionData& arg2) {
                              return arg1.mutation_counter_ <
                                     arg2.mutation_counter_;
                            })
               ->mutation_counter_
        << std::endl;
  stats << "Test case min mutation count: "
        << std::max_element(data_.begin(), data_.end(),
                            [](ExecutionData& arg1, ExecutionData& arg2) {
                              return arg1.mutation_counter_ >
                                     arg2.mutation_counter_;
                            })
               ->mutation_counter_
        << std::endl;

  stats << "Test case max similar executions count: "
        << std::max_element(data_.begin(), data_.end(),
                            [](ExecutionData& arg1, ExecutionData& arg2) {
                              return arg1.coverage_coutner_ <
                                     arg2.coverage_coutner_;
                            })
               ->coverage_coutner_
        << std::endl;
  stats << "Test case min similar executions count: "
        << std::max_element(data_.begin(), data_.end(),
                            [](ExecutionData& arg1, ExecutionData& arg2) {
                              return arg1.coverage_coutner_ >
                                     arg2.coverage_coutner_;
                            })
               ->coverage_coutner_
        << std::endl;

  stats << "Test case max execution time[ms]: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(
               std::max_element(data_.begin(), data_.end(),
                                [](ExecutionData& arg1, ExecutionData& arg2) {
                                  return arg1.execution_time <
                                         arg2.execution_time;
                                })
                   ->execution_time)
               .count()
        << std::endl;
  stats << "Test case min execution time[ms]: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(
               std::max_element(data_.begin(), data_.end(),
                                [](ExecutionData& arg1, ExecutionData& arg2) {
                                  return arg1.execution_time >
                                         arg2.execution_time;
                                })
                   ->execution_time)
               .count()
        << std::endl;

  stats << "Gracefully finished: "
        << std::count_if(data_.begin(), data_.end(),
                         [](ExecutionData& arg) {
                           return arg.gracefully_finished == true;
                         })
        << std::endl;

  stats << "Non zero error code: "
        << std::count_if(
               data_.begin(), data_.end(),
               [](ExecutionData& arg) { return arg.error_code.value() != 0; })
        << std::endl;

  stats << "Non zero return code: "
        << std::count_if(data_.begin(), data_.end(),
                         [](ExecutionData& arg) { return arg.exit_code != 0; })
        << std::endl;

  stats << "Faulty test cases: "
        << std::count_if(data_.begin(), data_.end(),
                         [](ExecutionData& arg) {
                           return arg.gracefully_finished == false &&
                                  arg.error_code.value() != 0;
                         })
        << std::endl;

  return stats;
}

} /* namespace fuzzon */
