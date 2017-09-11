/*
 * Copyright [2017] Jacek Weremko
 */

#include "./fuzzon_executor.h"

#include <boost/thread.hpp>
#include <boost/process.hpp>
#include <boost/process/extend.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream_buffer.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/chrono.hpp>
#include <boost/chrono/chrono.hpp>
#include <boost/chrono/duration.hpp>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <memory>
#include <utility>
#include <vector>

#include "./fuzzon_executiontracker.h"
#include "./utils/logger.h"

namespace bp = boost::process;
namespace ex = bp::extend;
namespace bs = boost::system;
namespace ba = boost::asio;

namespace fuzzon {

Executor::Executor(std::string sut_path, const std::vector<std::string>& env_flags, int execution_timeout_ms_)
    : sut_path_(sut_path), execution_timeout_(std::chrono::milliseconds(execution_timeout_ms_)) {
  ExecutionTracker::Get(ExecutionTracker::Monitor);
  LOG_DEBUG(std::string("SUT path: ") + sut_path_);

  for (auto env_flag : env_flags) {
    auto equal_sign_pos = env_flag.find('=');
    if (equal_sign_pos == std::string::npos) {
      sut_env_[env_flag] = "";
    } else {
      const auto flag_key = env_flag.substr(0, equal_sign_pos);
      const auto flag_value = env_flag.substr(equal_sign_pos + 1, env_flag.size());
      sut_env_[flag_key] = flag_value;
    }
  }

  // bug (?) in llvm : would report 1bilion of memory leaks on crash
  sut_env_["ASAN_OPTIONS"] += ":detect_leaks=false";
  // hack : instead of linking  with asan <- this is not quite possible since
  // some symbols not easly avaliable
  sut_env_["LD_PRELOAD"] = "/usr/lib/llvm-4.0/lib/clang/4.0.0/lib/linux/libclang_rt.asan-x86_64.so";
}

struct process_hanlders : ex::async_handler {
  template <typename Sequence>
  std::function<void(int, const std::error_code&)> on_exit_handler(Sequence& exec) {
    auto handler = exec.handler;
    return [handler](int exit_code, const std::error_code& ec) {
      std::cout << "on_exit_handler: " << exit_code << " ec:" << ec << std::endl;
    };
  }

  template <typename Sequence>
  void on_setup(bp::extend::posix_executor<Sequence>& exec) {
    std::cout << "on_setup: " << exec.exe << std::endl;
  }

  template <typename Sequence>
  void on_error(bp::extend::posix_executor<Sequence>& exec, const std::error_code& ec) {
    std::cout << "on_error: " << exec.exe << " ec:" << ec << std::endl;
  }

  template <typename Sequence>
  void on_success(bp::extend::posix_executor<Sequence>& exec) {
    std::cout << "on_success: " << exec.exe << std::endl;
  }

  template <typename Sequence>
  void on_success(bp::extend::posix_executor<Sequence>& exec) const {
    std::cout << "on_success: " << exec.exe << std::endl;
  }

  template <typename Sequence>
  void on_fork_error(bp::extend::posix_executor<Sequence>& exec, const std::error_code& ec) {
    std::cout << "on_fork_error: " << exec.exe << " ec:" << ec << std::endl;
  }

  template <typename Sequence>
  void on_exec_error(bp::extend::posix_executor<Sequence>& exec, const std::error_code& ec) {
    std::cout << "on_exec_error: " << exec.exe << " ec:" << ec << std::endl;
  }

  template <typename Sequence>
  void on_exec_error(bp::extend::posix_executor<Sequence>& exec, const std::error_code& ec) const {
    std::cout << "on_exec_error: " << exec.exe << " ec:" << ec << std::endl;
  }
};

ExecutionData Executor::ExecuteBlocking(TestCase& input) {
  using async_handler = std::function<void(const bs::error_code& ec, std::size_t n)>;
  ba::io_service ios;

  auto sut_std_out = std::make_shared<std::stringstream>();
  auto stdout_buffer = std::vector<char>(1024);
  auto stdout_ap_buffer = ba::buffer(stdout_buffer);
  auto stdout_ap = bp::async_pipe(ios);
  async_handler stdout_handler = [&](const bs::error_code& ec, size_t n) {
    std::copy(stdout_buffer.begin(), stdout_buffer.begin() + n, std::ostream_iterator<char>(*sut_std_out.get()));
    //    std::cout << "sut_std_out:" << sut_std_out;
    if (ec == 0) {
      boost::asio::async_read(stdout_ap, stdout_ap_buffer, stdout_handler);
    }
  };

  auto sut_std_err = std::make_shared<std::stringstream>();
  auto stderr_buffer = std::vector<char>(1024);
  auto stderr_ap_buffer = ba::buffer(stderr_buffer);
  auto stderr_ap = bp::async_pipe(ios);
  async_handler stderr_handler = [&](const bs::error_code& ec, size_t n) {
    std::copy(stderr_buffer.begin(), stderr_buffer.begin() + n, std::ostream_iterator<char>(*sut_std_err.get()));
    //    std::cout << "sut_std_err:" << sut_std_err;
    if (ec == 0) {
      ba::async_read(stderr_ap, stderr_ap_buffer, stderr_handler);
    }
  };

  auto stdin_ap_buffer = ba::buffer(input.string());
  auto stdin_ap = bp::async_pipe(ios);
  async_handler stdin_handler = [&](const bs::error_code& ec, size_t n) { stdin_ap.async_close(); };

  ba::async_write(stdin_ap, stdin_ap_buffer, stdin_handler);
  ba::async_read(stdout_ap, stdout_ap_buffer, stdout_handler);
  ba::async_read(stderr_ap, stderr_ap_buffer, stderr_handler);

  boost::process::child sut(sut_path_, input.string(), boost::process::std_out > stdout_ap,
                            boost::process::std_err > stderr_ap, boost::process::std_in < stdin_ap, sut_env_);

  boost::system::error_code boost_ec;
  std::auto_ptr<boost::asio::io_service::work> work(new boost::asio::io_service::work(ios));
  auto worker = boost::thread([&work, &boost_ec]() { auto handlers_count = work->get_io_service().run(boost_ec); });

  std::error_code ec;
  auto start = std::chrono::system_clock::now();
  auto gracefully_finished = sut.wait_for(execution_timeout_, ec);
  auto finish = std::chrono::system_clock::now();
  if (!gracefully_finished) {
    //    work->get_io_service().run_one();
    sut.terminate(ec);
    finish = std::chrono::system_clock::now();
  }
  auto exit_code = sut.exit_code();
  //  auto handlers_count2 = work->get_io_service().run();
  work->get_io_service().stop();
  worker.join();

  return ExecutionData(input, ec, exit_code, gracefully_finished,
                       std::chrono::duration_cast<std::chrono::milliseconds>(finish - start), std::move(sut_std_out),
                       std::move(sut_std_err), ExecutionTracker::Get()->GetCoverage());
}

} /* namespace fuzzon */
