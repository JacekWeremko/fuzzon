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
        gracefull_close(gracefully_finished),
        execution_time(execution_time),
        std_out(std_out.str()),
        std_err(std_err.str()), /* TODO: performance */
        path(*cov),
        mutatation_exhausted(false),
        similar_path_coutner_(0),
        mutation_counter_(0) {
    path.Compress();
    path.ComputeHash();
  }

  friend std::ostream& operator<<(std::ostream& os, const ExecutionData& print_me) {
    os << "{" << std::endl;
    os << "\"input\" : " << print_me.input << "," << std::endl;
    os << "\"error_code\" : \"" << print_me.error_code << "\"," << std::endl;
    os << "\"exit_code\" : " << print_me.exit_code << "," << std::endl;
    os << "\"gracefull_close\" : " << print_me.gracefull_close << "," << std::endl;
    os << "\"execution_time\" : " << print_me.execution_time.count() << "," << std::endl;
    os << "\"std_out\" : \"" << print_me.std_out << "\"," << std::endl;
    os << "\"std_err\" : \"" << print_me.std_err << "\"," << std::endl;
    os << "\"path\" : " << print_me.path << std::endl;
    os << "}";
    return os;
  }

  TestCase input;
  std::error_code error_code;
  int exit_code;
  bool gracefull_close;
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
