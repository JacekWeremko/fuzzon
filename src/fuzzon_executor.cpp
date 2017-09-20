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
#include <boost/lockfree/queue.hpp>
#include <boost/chrono.hpp>
#include <boost/chrono/chrono.hpp>
#include <boost/chrono/duration.hpp>
#include <boost/signals2.hpp>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <memory>
#include <utility>
#include <vector>
#include <list>

#include "./fuzzon_executiontracker.h"
#include "./utils/logger.h"

namespace bp = boost::process;
namespace ex = bp::extend;
namespace bs = boost::system;
namespace ba = boost::asio;

namespace fuzzon {

Executor::Executor(std::string sut_path,
                   const std::vector<std::string>& env_flags,
                   int execution_timeout_ms,
                   Executor::Mode mode,
                   Coverage::TrackMode track_mode)
    : sut_path_(sut_path),
      execution_timeout_(std::chrono::milliseconds(execution_timeout_ms)),
      mode_(mode) {
  ExecutionTracker::Get(ExecutionTracker::Monitor, track_mode);
  LOG_DEBUG(std::string("SUT path: ") + sut_path_);

  for (auto env_flag : env_flags) {
    auto equal_sign_pos = env_flag.find('=');
    if (equal_sign_pos == std::string::npos) {
      sut_env_[env_flag] = "";
    } else {
      const auto flag_key = env_flag.substr(0, equal_sign_pos);
      const auto flag_value =
          env_flag.substr(equal_sign_pos + 1, env_flag.size());
      sut_env_[flag_key] = flag_value;
    }
  }

  // bug (?) in llvm : would report 1bilion of memory leaks on crash
  sut_env_["ASAN_OPTIONS"] += ":detect_leaks=false";
  // just load libclang_rt.asan lib
  // linking with asan lib is not quite possible since some reqired symbols
  // missing
  sut_env_["LD_PRELOAD"] =
      "/usr/lib/llvm-4.0/lib/clang/4.0.0/lib/linux/libclang_rt.asan-x86_64.so";
}

#if EXTERN_FUZZZON_ENTRY_POINT

extern "C" int FuzzonTest(int argc, char** argv);

ExecutionData Executor::ExecuteBlockingThread(TestCase& input) {
  ba::io_service ios;

  boost::asio::signal_set signals(ios);
  signals.add(SIGHUP);
  signals.add(SIGINT);
  signals.add(SIGQUIT);
  signals.add(SIGILL);
  signals.add(SIGTRAP);
  signals.add(SIGABRT);
  signals.add(SIGIOT);
  signals.add(SIGBUS);
  signals.add(SIGFPE);
  //  signals.add(SIGKILL);
  signals.add(SIGUSR1);
  signals.add(SIGSEGV);
  signals.add(SIGUSR2);
  signals.add(SIGPIPE);
  signals.add(SIGALRM);
  signals.add(SIGTERM);
  signals.add(SIGSTKFLT);
  signals.add(SIGCLD);
  signals.add(SIGCHLD);
  signals.add(SIGCONT);
  //  signals.add(SIGSTOP);
  signals.add(SIGTSTP);
  signals.add(SIGTTIN);
  signals.add(SIGTTOU);
  signals.add(SIGURG);
  signals.add(SIGXCPU);
  signals.add(SIGXFSZ);
  signals.add(SIGVTALRM);
  signals.add(SIGPROF);
  signals.add(SIGWINCH);
  signals.add(SIGPOLL);
  signals.add(SIGIO);
  signals.add(SIGPWR);
  signals.add(SIGSYS);
  signals.add(SIGUNUSED);

  signals.async_wait([&](const boost::system::error_code& error, int signal) {
    std::cout << "signal_set -> " << error << ":" << signal << std::endl;
  });

  std::error_code ec;
  int exit_code = 0;
  bool timeout_not_occured = false;

  std::vector<char*> argv;
  {
    std::istringstream ss(input.string());
    std::string current_arg;
    std::list<std::string> helper_list;
    while (ss >> current_arg) {
      helper_list.push_back(current_arg);
      argv.push_back(const_cast<char*>(helper_list.back().c_str()));
    }
    argv.push_back(0);  // need terminating null pointer
  }

  auto start = std::chrono::system_clock::now();
  try {
    exit_code = FuzzonTest(argv.size(), &argv[0]);
  } catch (std::exception& ex) {
    std::cout << ex.what() << std::endl;
  }
  auto finish = std::chrono::system_clock::now();

  return ExecutionData(
      input, ec, exit_code, !timeout_not_occured,
      std::chrono::duration_cast<std::chrono::microseconds>(finish - start),
      std::make_shared<std::stringstream>(),
      std::make_shared<std::stringstream>(),
      ExecutionTracker::Get()->GetCoverage());
}

ExecutionData Executor::ExecuteBlockingThreadFork(TestCase& input) {
  ba::io_service ios;

  std::error_code ec;
  int exit_code = 0;
  bool timeout_not_occured = false;

  std::vector<char*> argv;
  {
    std::istringstream ss(input.string());
    std::string current_arg;
    std::list<std::string> helper_list;
    while (ss >> current_arg) {
      helper_list.push_back(current_arg);
      argv.push_back(const_cast<char*>(helper_list.back().c_str()));
    }
    argv.push_back(0);  // need terminating null pointer
  }

  auto start = std::chrono::system_clock::now();
  try {
    boost::thread sut([&argv, &ios]() {
      boost::asio::signal_set signals(ios);
      signals.add(SIGHUP);
      signals.add(SIGINT);
      signals.add(SIGQUIT);
      signals.add(SIGILL);
      signals.add(SIGTRAP);
      signals.add(SIGABRT);
      signals.add(SIGIOT);
      signals.add(SIGBUS);
      signals.add(SIGFPE);
      //  signals.add(SIGKILL);
      signals.add(SIGUSR1);
      signals.add(SIGSEGV);
      signals.add(SIGUSR2);
      signals.add(SIGPIPE);
      signals.add(SIGALRM);
      signals.add(SIGTERM);
      signals.add(SIGSTKFLT);
      signals.add(SIGCLD);
      signals.add(SIGCHLD);
      signals.add(SIGCONT);
      //  signals.add(SIGSTOP);
      signals.add(SIGTSTP);
      signals.add(SIGTTIN);
      signals.add(SIGTTOU);
      signals.add(SIGURG);
      signals.add(SIGXCPU);
      signals.add(SIGXFSZ);
      signals.add(SIGVTALRM);
      signals.add(SIGPROF);
      signals.add(SIGWINCH);
      signals.add(SIGPOLL);
      signals.add(SIGIO);
      signals.add(SIGPWR);
      signals.add(SIGSYS);
      signals.add(SIGUNUSED);

      signals.async_wait([&](const boost::system::error_code& error,
                             int signal) {
        std::cout << "signal_set -> " << error << ":" << signal << std::endl;
      });
      auto exit_code = FuzzonTest(argv.size(), &argv[0]);
    });
  } catch (std::exception& ex) {
    std::cout << ex.what() << std::endl;
  }

  auto finish = std::chrono::system_clock::now();

  return ExecutionData(
      input, ec, exit_code, !timeout_not_occured,
      std::chrono::duration_cast<std::chrono::microseconds>(finish - start),
      std::make_shared<std::stringstream>(),
      std::make_shared<std::stringstream>(),
      ExecutionTracker::Get()->GetCoverage());
}

#endif

ExecutionDataSP Executor::ExecuteProcessStdInFile(TestCase& input) {
  ExecutionTracker::Get()->Reset();

  auto input_file_path = boost::filesystem::path("input.txt");
  boost::filesystem::ofstream input_file(input_file_path);
  input_file << input.string();

  auto start = std::chrono::system_clock::now();
  int exit_code = bp::system(sut_path_, sut_env_, input.string(),
                             boost::process::std_out > boost::process::null,
                             boost::process::std_err > boost::process::null,
                             boost::process::std_in = input_file_path);
  auto finish = std::chrono::system_clock::now();

  static int counter = 0;
  if (exit_code != 0) {
    counter++;
    std::cout << "ExecuteProcessStdInFile exit_code != 0 : " << counter
              << std::endl;
  }
  std::error_code ec;

  auto duration = finish - start;
  //      std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
  bool timeout_not_occured = duration > execution_timeout_ ? false : true;

  return std::make_shared<ExecutionData>(
      input, ec, exit_code, !timeout_not_occured,
      std::chrono::duration_cast<std::chrono::microseconds>(finish - start),
      std::make_shared<std::stringstream>(),
      std::make_shared<std::stringstream>(),
      ExecutionTracker::Get()->GetCoverage());
}

ExecutionDataSP Executor::ExecuteProcessAsyncStdAllStremsPoll(TestCase& input) {
  using async_handler = std::function<void(const bs::error_code& ec, size_t n)>;
  ba::io_service ios;

  auto sut_std_out = std::make_shared<std::stringstream>();
  auto stdout_buffer = std::vector<char>(10 * 4096);
  auto stdout_ap_buffer = ba::buffer(stdout_buffer);
  auto stdout_ap = bp::async_pipe(ios);
  async_handler stdout_handler = [&](const bs::error_code& ec, size_t n) {
    std::copy(stdout_buffer.begin(), stdout_buffer.begin() + n,
              std::ostream_iterator<char>(*sut_std_out.get()));
    //    std::cout << "sut_std_out:" << sut_std_out;
    if (ec == 0) {
      boost::asio::async_read(stdout_ap, stdout_ap_buffer, stdout_handler);
    }
  };

  auto sut_std_err = std::make_shared<std::stringstream>();
  auto stderr_buffer = std::vector<char>(10 * 4096);
  auto stderr_ap_buffer = ba::buffer(stderr_buffer);
  auto stderr_ap = bp::async_pipe(ios);
  async_handler stderr_handler = [&](const bs::error_code& ec, size_t n) {
    std::copy(stderr_buffer.begin(), stderr_buffer.begin() + n,
              std::ostream_iterator<char>(*sut_std_err.get()));
    //    std::cout << "sut_std_err:" << sut_std_err.get();
    if (ec == 0) {
      ba::async_read(stderr_ap, stderr_ap_buffer, stderr_handler);
    }
  };

  boost::process::detail::posix::file_descriptor null_source{
      "/dev/null", boost::process::detail::posix::file_descriptor::read};

  auto stdin_ap = bp::async_pipe(ios);
  async_handler stdin_handler = [&](const bs::error_code& ec, size_t n) {
    stdin_ap.async_close();
  };
  //  char nulls_arr[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  //  ba::const_buffer cb(&nulls_arr[0], 0);
  //  boost::process::detail::posix::file_descriptor null_source{
  //      "/dev/null", boost::process::detail::posix::file_descriptor::read};

  auto start = std::chrono::system_clock::now();
  boost::process::child sut(
      sut_path_, input.string(), sut_env_, boost::process::std_out > stdout_ap,
      boost::process::std_err > stderr_ap, boost::process::std_in < stdin_ap);

  ba::async_write(stdin_ap, ba::buffer(input.string()), stdin_handler);
  ba::async_read(stdout_ap, stdout_ap_buffer, stdout_handler);
  ba::async_read(stderr_ap, stderr_ap_buffer, stderr_handler);

  std::error_code ec;
  boost::system::error_code boot_ec;
  while (true) {
    ios.poll(boot_ec);
    if (boot_ec.value()) {
      break;
    }

    if (ios.stopped()) {
      ios.reset();
    }

    auto now = std::chrono::system_clock::now();
    if (((now - start) > execution_timeout_) || !sut.running()) {
      break;
    }
  }

  if (sut.running()) {
    sut.terminate(ec);
  }

  auto finish = std::chrono::system_clock::now();
  auto timeout_occured = ((finish - start) > execution_timeout_);
  auto exit_code = sut.exit_code();

  return std::make_shared<ExecutionData>(
      input, ec, exit_code, timeout_occured,
      std::chrono::duration_cast<std::chrono::microseconds>(finish - start),
      std::move(sut_std_out), std::move(sut_std_err),
      ExecutionTracker::Get()->GetCoverage());
}

ExecutionDataSP Executor::ExecuteProcessSyncStdAllStremsPoll(TestCase& input) {
  bp::pipe pipe_in;
  bp::pipe pipe_out;
  bp::pipe pipe_err;

  auto int_str = input.string();
  pipe_in.write(int_str.c_str(), int_str.size());

  auto start = std::chrono::system_clock::now();
  boost::process::child sut(
      sut_path_, input.string(), sut_env_, boost::process::std_out > pipe_out,
      boost::process::std_err > pipe_err, boost::process::std_in < pipe_in);

  while (!pipe_in.is_open()) {
    boost::this_thread::yield();
  }

  std::error_code ec;
  boost::system::error_code boot_ec;
  while (true) {
    auto now = std::chrono::system_clock::now();
    if ((now - start > execution_timeout_) || !sut.running()) {
      break;
    }
    boost::this_thread::yield();
  }

  if (sut.running()) {
    sut.terminate(ec);
    std::cout << "terminate";
  }

  std::string output;
  {
    char output_char[256] = {0};
    while (true) {
      auto read_len = ::read(pipe_out.native_source(), &output_char[0], 256);
      if (read_len <= 0) {
        break;
      }
      output += std::string(&output_char[0], read_len);
    }
  }

  std::string error;
  {
    char outerr_char[256] = {0};
    while (true) {
      auto read_len = ::read(pipe_err.native_source(), &outerr_char[0], 256);
      if (read_len <= 0) {
        break;
      }
      error += std::string(&outerr_char[0], read_len);
    }
  }

  if (output != "") {
    //    std::cout << "output: " << output << std::endl;
    std::cout << "o";
  }

  if (error != "") {
    std::cout << "e";
    //    std::cout << "error: " << error << std::endl;
  }

  auto finish = std::chrono::system_clock::now();
  auto timeout_occured = ((finish - start) > execution_timeout_);
  auto exit_code = sut.exit_code();

  return std::make_shared<ExecutionData>(
      input, ec, exit_code, timeout_occured,
      std::chrono::duration_cast<std::chrono::microseconds>(finish - start),
      std::make_shared<std::stringstream>(),
      std::make_shared<std::stringstream>(),
      ExecutionTracker::Get()->GetCoverage());
}

} /* namespace fuzzon */
