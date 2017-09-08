/*
 * Copyright [2017] Jacek Weremko
 */

#ifndef SRC_FUZZON_H_
#define SRC_FUZZON_H_

#include "./fuzzon_executor.h"

#include <boost/filesystem.hpp>
#include <string>

#include "./fuzzon_corpus.h"

namespace fuzzon {

class Fuzzon {
 public:
  Fuzzon(std::string output_dir, std::string sut_path, int sut_runtime_timeout);

  void Generation(std::string input_format, int test_cases_to_generate);
  void MutationDeterministic(bool white_chars_preservation);
  void MutationNonDeterministic(int test_cases_to_mutate,
                                bool white_chars_preservation);

 private:
  std::string output_dir_;
  Corpus corpus_;
  Executor execution_monitor_;
};

}  // namespace fuzzon

#endif  // SRC_FUZZON_H_
