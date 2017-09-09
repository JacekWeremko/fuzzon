/*
 * Copyright [2017] Jacek Weremko
 */

#ifndef SRC_FUZZON_CORPUS_H_
#define SRC_FUZZON_CORPUS_H_

#include "./fuzzon_executiondata.h"

#include <boost/filesystem.hpp>
#include <list>
#include <sstream>
#include <string>
#include <memory>
#include <vector>

#include "./fuzzon_testcase.h"

namespace fuzzon {

class Corpus {
 public:
  explicit Corpus(std::string output_path);

  bool AddIfInteresting(ExecutionData& add_me_to_corpus);

  const TestCase* SelectFavorite();
  const TestCase* SelectNotYetExhaustMutated();

  std::stringstream GetShortStats();
  std::stringstream GetFullStats();

  std::vector<boost::filesystem::path> GatherCoprusFiles() const;

  void Dump();

 private:
  const std::string DIR_NAME_CORPUS = "corpus";
  const std::string DIR_NAME_RESULTS = "results";
  const std::string DIR_NAME_CRASH = "crash";
  const std::chrono::system_clock::time_point start_;

  boost::filesystem::path output_path_;
  std::list<ExecutionData> data_;
  Coverage total_;

  bool IsInteresting(const ExecutionData& am_i);
  void AddExecutionData(ExecutionData& add_me_to_corpus);
};

} /* namespace fuzzon */

#endif /* SRC_FUZZON_CORPUS_H_ */
