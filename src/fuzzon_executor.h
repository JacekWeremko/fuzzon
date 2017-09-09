/*
 * Copyright [2017] Jacek Weremko
 */

#ifndef SRC_FUZZON_EXECUTOR_H_
#define SRC_FUZZON_EXECUTOR_H_

#include "./fuzzon_testcase.h"

#include <string>
#include <memory>
#include <vector>

#include "./fuzzon_executiondata.h"

namespace fuzzon {

class Executor {
 public:
  explicit Executor(std::string sut_path, const std::vector<std::string>& env_flags, int execution_timeout_ms);

  ExecutionData ExecuteBlocking(TestCase& input);

 private:
  std::string sut_path_;
  boost::process::environment sut_env_;
  const std::chrono::milliseconds execution_timeout_;
};

} /* namespace fuzzon */

#endif  // SRC_FUZZON_EXECUTOR_H_
