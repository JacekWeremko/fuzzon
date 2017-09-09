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

#include "./utils/logger.h"

namespace fs = boost::filesystem;
namespace stdch = std::chrono;

namespace fuzzon {
Corpus::Corpus(std::string output_path)
    : start_(stdch::system_clock::now()), output_path_(fs::path(output_path)), total_(Coverage::Raw) {
  boost::filesystem::create_directories(output_path_);
  boost::filesystem::create_directories(output_path_ / DIR_NAME_RESULTS);
  boost::filesystem::create_directories(output_path_ / DIR_NAME_CORPUS);
  boost::filesystem::create_directories(output_path_ / DIR_NAME_CRASH);
}

bool Corpus::IsInteresting(const ExecutionData& am_i) {
  auto result = true;
  for (auto& current : data_) {
    if (current.path == am_i.path) {
      current.path_execution_coutner_++;
      result = false;
    }
  }

  static Timeout log_timer(stdch::milliseconds(1000));
  if (log_timer()) {
    LOG_INFO(GetShortStats().str());
    log_timer.arm(stdch::system_clock::now());
  }

  return result;
}

// TODO: move semantic
void Corpus::AddExecutionData(ExecutionData& add_me_to_corpus) {
  LOG_DEBUG("Adding new test case to corpus: " + add_me_to_corpus.input.string());
  // TODO: optimize memory footprint

  total_.Merge(add_me_to_corpus.path);
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

  std::vector<ExecutionData*> lowest_mutation_counter;
  // find min s(i) - test cases used as mutation base
  {
    auto lowest_usage_count = std::numeric_limits<size_t>::max();
    for (auto& current : data_copy_) {
      if (current->mutation_counter_ < lowest_usage_count) {
        lowest_mutation_counter.clear();
        lowest_usage_count = current->mutation_counter_;
        lowest_mutation_counter.push_back(current);
      } else if (current->mutation_counter_ == lowest_usage_count) {
        lowest_mutation_counter.push_back(current);
      }
    }
  }

  std::vector<ExecutionData*> lowest_similar_path_coutner;
  // find min f(i) - lowest frequency path
  {
    auto lowest_similar_execution_coutner = std::numeric_limits<size_t>::max();
    for (auto& current : lowest_mutation_counter) {
      if (current->path_execution_coutner_ < lowest_similar_execution_coutner) {
        lowest_similar_path_coutner.clear();
        lowest_similar_execution_coutner = current->path_execution_coutner_;
        lowest_similar_path_coutner.push_back(current);
      } else if (current->path_execution_coutner_ == lowest_similar_execution_coutner) {
        lowest_similar_path_coutner.push_back(current);
      }
    }
  }

  ExecutionData* result = nullptr;
  // find min t(i) time and size
  {
    auto lowest_time_and_size = std::numeric_limits<double>::max();
    for (auto& current : lowest_similar_path_coutner) {
      double current_time_and_size = current->execution_time.count() * current->input.length_byte();
      if (current_time_and_size < lowest_time_and_size) {
        result = current;
      }
    }
  }
  if (result != nullptr) {
    return &result->input;
  }
  return nullptr;
}

const TestCase* Corpus::SelectNotYetExhaustMutated() {
  for (auto& current : data_) {
    if (current.mutatation_exhausted == false) {
      current.mutatation_exhausted = true;
      return &current.input;
    }
  }
  return nullptr;
}

std::stringstream Corpus::GetShortStats() {
  std::stringstream stats;
  stats << time_format(stdch::duration_cast<stdch::milliseconds>(stdch::system_clock::now() - start_))
        << "    Runs:" << std::accumulate(data_.begin(), data_.end(), 0,
                                          [](int execution_counter, ExecutionData& arg) {
                                            return execution_counter + arg.path_execution_coutner_;
                                          })
        << "    cor:" << std::to_string(data_.size()) << "    cov:" << total_.GetVisitedPCCounter() << "/"
        << total_.GetTotalPCCounter() << "  "
        << ((static_cast<double>(total_.GetVisitedPCCounter()) / static_cast<double>(total_.GetTotalPCCounter())) *
            100);
  return stats;
}

std::stringstream Corpus::GetFullStats() {
  std::stringstream stats;
  stats << "Corpus size : " << std::to_string(data_.size()) << std::endl;

  stats << "Total coverage: "
        << (static_cast<double>(total_.GetVisitedPCCounter()) / static_cast<double>(total_.GetTotalPCCounter())) * 100
        << std::endl;
  stats << "Visited pc: " << total_.GetVisitedPCCounter() << std::endl;
  stats << "Total   pc: " << total_.GetTotalPCCounter() << std::endl;

  stats << "Test cases: " << std::accumulate(data_.begin(), data_.end(), 0, [](int execution_counter,
                                                                               ExecutionData& arg) {
    return execution_counter + arg.path_execution_coutner_;
  }) << std::endl;

  if (data_.size() == 0) {
    return stats;
  }

  stats << "Test case max mutation count: "
        << std::max_element(
               data_.begin(), data_.end(),
               [](ExecutionData& arg1, ExecutionData& arg2) { return arg1.mutation_counter_ < arg2.mutation_counter_; })
               ->mutation_counter_
        << std::endl;
  stats << "Test case min mutation count: "
        << std::max_element(
               data_.begin(), data_.end(),
               [](ExecutionData& arg1, ExecutionData& arg2) { return arg1.mutation_counter_ > arg2.mutation_counter_; })
               ->mutation_counter_
        << std::endl;

  stats << "Test case max similar executions count: "
        << std::max_element(data_.begin(), data_.end(),
                            [](ExecutionData& arg1, ExecutionData& arg2) {
                              return arg1.path_execution_coutner_ < arg2.path_execution_coutner_;
                            })
               ->path_execution_coutner_
        << std::endl;
  stats << "Test case min similar executions count: "
        << std::max_element(data_.begin(), data_.end(),
                            [](ExecutionData& arg1, ExecutionData& arg2) {
                              return arg1.path_execution_coutner_ > arg2.path_execution_coutner_;
                            })
               ->path_execution_coutner_
        << std::endl;

  stats << "Test case max execution time: "
        << time_format(std::max_element(data_.begin(), data_.end(),
                                        [](ExecutionData& arg1, ExecutionData& arg2) {
                                          return arg1.execution_time < arg2.execution_time;
                                        })
                           ->execution_time)
        << std::endl;
  stats << "Test case min execution time: "
        << time_format(std::max_element(data_.begin(), data_.end(),
                                        [](ExecutionData& arg1, ExecutionData& arg2) {
                                          return arg1.execution_time > arg2.execution_time;
                                        })
                           ->execution_time)
        << std::endl;

  stats << "Gracefully finished: " << std::count_if(data_.begin(), data_.end(), [](ExecutionData& arg) {
    return arg.gracefull_close == true;
  }) << std::endl;

  stats << "Non zero error code: " << std::count_if(data_.begin(), data_.end(), [](ExecutionData& arg) {
    return arg.error_code.value() != 0;
  }) << std::endl;

  stats << "Non zero return code: " << std::count_if(data_.begin(), data_.end(), [](ExecutionData& arg) {
    return arg.exit_code != 0;
  }) << std::endl;

  stats << "Faulty test cases: " << std::count_if(data_.begin(), data_.end(), [](ExecutionData& arg) {
    return arg.gracefull_close == false && arg.error_code.value() != 0;
  }) << std::endl;

  return stats;
}

void Corpus::Dump() {
  fs::ofstream(output_path_ / "stats.txt") << GetFullStats().str();
  fs::ofstream(output_path_ / DIR_NAME_RESULTS / "total.json") << total_;

  {
    auto index = 1;
    for (auto& elem : data_) {
      fs::ofstream(output_path_ / DIR_NAME_CORPUS / fs::path(std::to_string(index) + ".txt")) << elem.input;

      const auto result_path = output_path_ / DIR_NAME_RESULTS / fs::path(std::to_string(index) + ".json");
      { fs::ofstream(result_path) << elem; }

      if (elem.std_err->tellp()) {
        const auto crash_link = output_path_ / DIR_NAME_CRASH / fs::path(std::to_string(index));
        fs::create_directory_symlink(result_path, crash_link);
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
