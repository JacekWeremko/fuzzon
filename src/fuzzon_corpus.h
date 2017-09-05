/*
 * Copyright [2017] Jacek Weremko
 */

#ifndef SRC_FUZZON_CORPUS_H_
#define SRC_FUZZON_CORPUS_H_

#include "./fuzzon_executiondata.h"

#include <vector>
#include <sstream>
#include <string>

#include "./fuzzon_testcase.h"

namespace fuzzon {

class Corpus {
  using ExecutionDataT = std::vector<ExecutionData>;

 public:
  explicit Corpus(std::string output_path);

  bool IsInteresting(const ExecutionData& am_i);
  void AddExecutionData(ExecutionData& add_me_to_corpus);

  bool AddIfInteresting(ExecutionData& add_me_to_corpus);

  using iterator = ExecutionDataT::iterator;

  TestCase* SelectFavorite();
  TestCase* SelectNotMutated();

  std::stringstream GetStatistics();

 private:
  std::string output_path_;
  ExecutionDataT execution_history_;
};

} /* namespace fuzzon */

#endif /* SRC_FUZZON_CORPUS_H_ */
