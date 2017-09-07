/*
 * Copyright [2017] Jacek Weremko
 */

#ifndef SRC_FUZZON_EXECUTIONDATA_H_
#define SRC_FUZZON_EXECUTIONDATA_H_

#include "./fuzzon_testcase.h"

#include <sstream>
#include <string>
#include <algorithm>
#include <system_error>
#include <chrono>

#include "./fuzzon_coverage.h"

namespace fuzzon {

struct ExecutionData {
  ExecutionData(TestCase tc,
                std::error_code erc,
                int exc,
                bool gracefully_finished,
                std::chrono::microseconds execution_time,
                std::stringstream& std_out,
                std::stringstream& std_err,
                const Coverage* cov)
      : input(tc),
        error_code(erc),
        exit_code(exc),
        gracefully_finished(gracefully_finished),
        execution_time(execution_time),
        std_out(std_out.str()),
        std_err(std_err.str()), /* TODO: performance */
        path(cov->mode_),
        mutatation_exhausted(false),
        similar_path_coutner_(0),
        mutation_counter_(0) {
    // coverage = *cov;
    std::copy(std::begin(cov->pc_flow_), std::end(cov->pc_flow_),
              std::begin(path.pc_flow_));
    path.Compress();
  }

  TestCase input;
  std::error_code error_code;
  int exit_code;
  bool gracefully_finished;
  std::chrono::microseconds execution_time;

  std::string std_out;
  std::string std_err;
  Coverage path;

  // TODO: shouln't be part of this class
  bool mutatation_exhausted;
  size_t similar_path_coutner_;
  size_t mutation_counter_;
};

} /* namespace fuzzon */

#endif  // SRC_FUZZON_EXECUTIONDATA_H_
