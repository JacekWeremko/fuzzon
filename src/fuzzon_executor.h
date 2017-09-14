/*
 * Copyright [2017] Jacek Weremko
 */

#ifndef SRC_FUZZON_EXECUTOR_H_
#define SRC_FUZZON_EXECUTOR_H_

#include "./fuzzon_testcase.h"

#include <boost/atomic.hpp>

#include <string>
#include <memory>
#include <vector>
#include <algorithm>

#include "./fuzzon_executiondata.h"

namespace fuzzon {

class Executor {
 public:
  enum Mode {
    PROCESS_ONLY_STDIN_FILE,
    PROCESS_ONLY_STDIN_ASYCN_STREAM,
    // ONLY_STDIN_ASYCN_SIGLE_STREAM,  // bugged
    PROCESS_ALL_STD_ASYNC_STREAMS,
    // ALL_STD_ASYNC_STREAMS_IOWT,
    THREAD
  };

  explicit Executor(std::string sut_path,
                    const std::vector<std::string>& env_flags,
                    int execution_timeout_ms,
                    Executor::Mode mode);

  ExecutionData ExecuteBlocking(TestCase& input) {
    return ExecuteProcessAsyncStdAllStremsPoll(input);
  }

  ExecutionData Test(TestCase& input);
  ExecutionData ExecuteProcessLinuxNativ(TestCase& input);
  ExecutionData ExecuteProcessStdInFile(TestCase& input);
  ExecutionData ExecuteProcessAsyncStdInStrems(TestCase& input);
  ExecutionData ExecuteProcessAsyncStdInStrems(TestCase& input, bool one_ios);
  ExecutionData ExecuteProcessAsyncStdInStrems(TestCase& input, int fallback);
  ExecutionData ExecuteProcessAsyncStdAllStrems(TestCase& input);
  ExecutionData ExecuteProcessAsyncStdAllStremsPoll(TestCase& input);

#ifdef EXTERN_FUZZZON_ENTRY_POINT
  ExecutionData ExecuteBlockingThread(TestCase& input);
  ExecutionData ExecuteBlockingThreadFork(TestCase& input);
#endif

 private:
  const std::string sut_path_;
  boost::process::environment sut_env_;
  const std::chrono::milliseconds execution_timeout_;
  const Executor::Mode mode_;
};

} /* namespace fuzzon */

#endif  // SRC_FUZZON_EXECUTOR_H_
