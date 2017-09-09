/*
 * Copyright [2017] Jacek Weremko
 */

#include "./fuzzon_executor.h"

#include <boost/process.hpp>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <memory>
#include <utility>

#include "./fuzzon_executiontracker.h"
#include "./utils/logger.h"

namespace fuzzon {

Executor::Executor(std::string sut_path, int execution_timeout_ms_)
    : sut_path_(sut_path), execution_timeout_(std::chrono::milliseconds(execution_timeout_ms_)) {
  ExecutionTracker::Get(ExecutionTracker::Monitor);
  LOG_DEBUG(std::string("SUT path: ") + sut_path_);
}

ExecutionData Executor::ExecuteBlocking(TestCase& input) {
  ExecutionTracker::Get()->Reset();

  std::string sut_std_out_value, sut_std_err_value;
  boost::process::ipstream sut_std_out_ips;
  boost::process::ipstream sut_std_err_ips;
  boost::process::opstream sit_std_in_ips;
  boost::process::child sut(sut_path_, input.string(), boost::process::std_out > sut_std_out_ips,
                            boost::process::std_err > sut_std_err_ips, boost::process::std_in < sit_std_in_ips);

  sit_std_in_ips << input.string() << std::endl;
  std::error_code ec;
  auto start = std::chrono::system_clock::now();
  auto gracefully_finished = sut.wait_for(execution_timeout_, ec);
  auto finish = std::chrono::system_clock::now();
  if (!gracefully_finished) {
    sut.terminate(ec);
    finish = std::chrono::system_clock::now();
  }
  auto exit_code = sut.exit_code();

  std::string line;
  auto sut_std_out = std::make_unique<std::stringstream>();
  while (std::getline(sut_std_out_ips, line)) {
    *sut_std_out.get() << line << std::endl;
  }

  auto sut_std_err = std::make_unique<std::stringstream>();
  while (std::getline(sut_std_err_ips, line)) {
    *sut_std_err.get() << line << std::endl;
  }

  return ExecutionData(input, ec, exit_code, gracefully_finished,
                       std::chrono::duration_cast<std::chrono::milliseconds>(finish - start), std::move(sut_std_out),
                       std::move(sut_std_err), ExecutionTracker::Get()->GetCoverage());
}

} /* namespace fuzzon */
