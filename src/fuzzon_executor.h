/*
 * Copyright [2017] Jacek Weremko
 */

#ifndef SRC_FUZZON_EXECUTOR_H_
#define SRC_FUZZON_EXECUTOR_H_

#include "./fuzzon_testcase.h"

#include <string>
#include <memory>

#include "./fuzzon_executiondata.h"

namespace fuzzon {

class Executor {
 public:
  explicit Executor(std::string sut_path, int execution_timeout_sec);

  ExecutionData ExecuteBlocking(TestCase& input);

 private:
  std::string sut_path_;
  const int execution_timeout_sec_;
};

} /* namespace fuzzon */

#endif  // SRC_FUZZON_EXECUTOR_H_
