/*
 * Copyright [2017] Jacek Weremko
 */

#ifndef SRC_FUZZON_CORPUS_H_
#define SRC_FUZZON_CORPUS_H_

#include "./fuzzon_executiondata.h"

#include <list>
#include <sstream>
#include <string>
#include <memory>

#include "./fuzzon_testcase.h"

namespace fuzzon {

class Corpus {
 public:
  explicit Corpus(std::string output_path);

  bool AddIfInteresting(ExecutionData& add_me_to_corpus);

  const TestCase* SelectFavorite();
  const TestCase* SelectNotMutated();

  std::stringstream GetStatistics();

 private:
  std::string output_path_;
  std::list<ExecutionData> data_;

  bool IsInteresting(const ExecutionData& am_i);
  void AddExecutionData(ExecutionData& add_me_to_corpus);
};

} /* namespace fuzzon */

#endif /* SRC_FUZZON_CORPUS_H_ */
