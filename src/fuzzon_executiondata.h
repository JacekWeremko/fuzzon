/*
 * Copyright [2017] Jacek Weremko
 */

#ifndef SRC_FUZZON_EXECUTIONDATA_H_
#define SRC_FUZZON_EXECUTIONDATA_H_

#include "./fuzzon_testcase.h"

#include <boost/process.hpp>
#include <sstream>
#include <string>
#include <algorithm>
#include <system_error>
#include <chrono>
#include <memory>
#include <utility>
#include <map>

#include "./fuzzon_coverage.h"
#include "./utils/time.hpp"

namespace fuzzon {

struct ExecutionData {
  ExecutionData(TestCase tc,
                std::error_code erc,
                int exc,
                bool timeout,
                std::chrono::microseconds execution_time,
                std::shared_ptr<std::stringstream> std_out,
                std::shared_ptr<std::stringstream> std_err,
                const Coverage* cov)
      : input(tc),
        error_code(erc),
        exit_code(exc),
        timeout(timeout),
        execution_time(execution_time),
        std_out(std::move(std_out)),
        std_err(std::move(std_err)),
        path(*cov),
        path_execution_coutner_(1) {
    path.Compress();
    path.ComputeHash();
  }

  friend std::ostream& operator<<(std::ostream& os,
                                  const ExecutionData& print_me) {
    os << "{" << std::endl;
    os << "\"genesis\" : \"" << print_me.input.my_genesis() << "\","
       << std::endl;
    os << "\"input\" : \"" << print_me.input << "\"," << std::endl;
    os << "\"error_code\" : \"" << print_me.error_code << "\"," << std::endl;
    os << "\"exit_code\" : " << print_me.exit_code << "," << std::endl;
    os << "\"timeout\" : " << print_me.timeout << "," << std::endl;
    os << "\"execution_time\" : \"" << time_format(print_me.execution_time)
       << "\"," << std::endl;
    os << "\"std_out\" : \"" << print_me.std_out->str() << "\"," << std::endl;
    os << "\"std_err\" : \"" << print_me.std_err->str() << "\"," << std::endl;
    os << "\"path\" : " << print_me.path << std::endl;
    os << "}";
    return os;
  }

  std::string string() const {
    std::stringstream ss;
    ss << *this;
    return ss.str();
  }

  bool crashed() const { return std_err->tellp() != 0; }

  TestCase input;
  std::error_code error_code;
  int exit_code;
  bool timeout;
  std::chrono::microseconds execution_time;

  std::shared_ptr<std::stringstream> std_out;
  std::shared_ptr<std::stringstream> std_err;
  Coverage path;

  // TODO: shouln't be part of this class
  size_t path_execution_coutner_;
};
using ExecutionDataSP = std::shared_ptr<ExecutionData>;

struct CampaignSummary {
  CampaignSummary()
      : test_cases(0),
        none_zero_error_code(0),
        none_zero_return_code(0),
        timeout(0),
        crash(0),
        total_cov(Coverage::Raw) {}
  int test_cases;
  int none_zero_error_code;
  int none_zero_return_code;
  int timeout;
  int crash;

  std::map<TestCase::Genesis, int> test_cases_genesis;
  Coverage total_cov;
};

} /* namespace fuzzon */

#endif  // SRC_FUZZON_EXECUTIONDATA_H_
