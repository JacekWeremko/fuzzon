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
                   Executor::Mode mode)
    : sut_path_(sut_path),
      execution_timeout_(std::chrono::milliseconds(execution_timeout_ms)),
      mode_(mode) {
  ExecutionTracker::Get(ExecutionTracker::Monitor);
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
  // hack : instead of linking  with asan <- this is not quite possible since
  // some symbols not easly avaliable
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

struct process_hanlders : ex::async_handler {
  template <typename Sequence>
  std::function<void(int, const std::error_code&)> on_exit_handler(
      Sequence& exec) {
    auto handler = exec.handler;
    return [handler](int exit_code, const std::error_code& ec) {
      std::cout << "on_exit_handler: " << exit_code << " ec:" << ec
                << std::endl;
    };
  }

  template <typename Sequence>
  void on_setup(bp::extend::posix_executor<Sequence>& exec) {
    std::cout << "on_setup: " << exec.exe << std::endl;
  }

  template <typename Sequence>
  void on_error(bp::extend::posix_executor<Sequence>& exec,
                const std::error_code& ec) {
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
  void on_fork_error(bp::extend::posix_executor<Sequence>& exec,
                     const std::error_code& ec) {
    std::cout << "on_fork_error: " << exec.exe << " ec:" << ec << std::endl;
  }

  template <typename Sequence>
  void on_exec_error(bp::extend::posix_executor<Sequence>& exec,
                     const std::error_code& ec) {
    std::cout << "on_exec_error: " << exec.exe << " ec:" << ec << std::endl;
  }

  template <typename Sequence>
  void on_exec_error(bp::extend::posix_executor<Sequence>& exec,
                     const std::error_code& ec) const {
    std::cout << "on_exec_error: " << exec.exe << " ec:" << ec << std::endl;
  }
};

struct std_handler {
  using async_handler =
      std::function<void(const boost::system::error_code& ec, size_t n)>;

  std_handler(boost::asio::io_service& ios,
              size_t N = 4096,
              async_handler stdout_handler = nullptr)
      : buffer_(N), async_buffer_(boost::asio::buffer(buffer_)) {
    our_streamer = std::make_shared<std::stringstream>();

    boost::filesystem::remove(boost::filesystem::path(pipe_name));
    pipe_ = new boost::process::async_pipe(ios, pipe_name);
  }

  ~std_handler() {
    //    boost::filesystem::path my_pipe(pipe_name);
    //    ::fclose(my_pipe);
    //    boost::filesystem::remove(boost::filesystem::path(pipe_name));
    volatile int res = -1;

    res = ::close(pipe_->native_source());
    while (res == 0) {
      res = ::close(pipe_->native_source());
    }

    res = ::close(pipe_->native_sink());
    while (res == 0) {
      res = ::close(pipe_->native_sink());
    }

    delete pipe_;
    res = ::unlink(pipe_name.c_str());
    while (res == 0) {
      res = ::unlink(pipe_name.c_str());
    }
  }

  boost::process::async_pipe& pipe() { return *pipe_; }

  const std::string pipe_name = "fuzzon_pipe";
  std::vector<char> buffer_;
  boost::asio::mutable_buffers_1 async_buffer_;
  boost::process::async_pipe* pipe_;
  async_handler handler_;

  std::shared_ptr<std::stringstream> our_streamer;
};

struct std_read_handler : std_handler {
  std_read_handler(boost::asio::io_service& ios, size_t N)
      : std_handler(ios, N) {
    handler_ = [&](const boost::system::error_code& ec, size_t n) {
      std::copy(buffer_.begin(), buffer_.begin() + n,
                std::ostream_iterator<char>(*our_streamer.get()));
      //    std::cout << "sut_std_out:" << sut_std_out;
      if (ec == 0) {
        boost::asio::async_read(pipe(), async_buffer_, handler_);
      } else {
        pipe().async_close();
      }
    };
    boost::asio::async_read(pipe(), async_buffer_, handler_);
  }
};

struct std_write_handler : std_handler {
  std_write_handler(boost::asio::io_service& ios, size_t N)
      : std_handler(ios, N) {
    handler_ = [&](const boost::system::error_code& ec, size_t n) {
      // report N-n -> how many bytes shoud be generated?
      pipe().async_close();
    };
    call_async_write();
  }

  void call_async_write() {
    boost::asio::async_write(pipe(), async_buffer_, handler_);
  }

  void Init(TestCase& input) {
    if (buffer_.size() < input.size()) {
      buffer_.resize(2 * input.size());
    }
    std::copy(input.begin(), input.end(), buffer_.begin());
  }
};

ExecutionDataSP Executor::ExecuteProcessAsyncStdInStrems(TestCase& input,
                                                         bool) {
  ExecutionTracker::Get()->Reset();
  static ba::io_service ios;
  static std_write_handler std_in_handler(ios, 10 * 4096);
  std_in_handler.Init(input);

  //  ios.reset();
  if (!std_in_handler.pipe_->is_open()) {
    delete std_in_handler.pipe_;
    std_in_handler.pipe_ = new boost::process::async_pipe(ios);
  }
  ios.reset();

  //  boost::system::error_code boost_ec;
  //  if (ios.stopped()) {
  //    std_in_handler.pipe_.close(boost_ec);
  //
  //    auto new_pipe = boost::process::async_pipe(ios);
  //  }

  std::error_code ec;
  auto start = std::chrono::system_clock::now();
  boost::process::child sut(sut_path_, input.string(), sut_env_, ios,
                            boost::process::std_in = std_in_handler.pipe(),
                            boost::process::std_out = boost::process::null,
                            boost::process::std_err = boost::process::null);

  auto timeout_not_occured = sut.wait_for(execution_timeout_, ec);
  auto finish = std::chrono::system_clock::now();
  if (!timeout_not_occured) {
    sut.terminate(ec);
    finish = std::chrono::system_clock::now();
  }
  auto exit_code = sut.exit_code();
  ios.stop();
  std_in_handler.pipe_->close();

  return std::make_shared<ExecutionData>(
      input, ec, exit_code, !timeout_not_occured,
      std::chrono::duration_cast<std::chrono::microseconds>(finish - start),
      std::make_shared<std::stringstream>(),
      std::make_shared<std::stringstream>(),
      ExecutionTracker::Get()->GetCoverage());
}

ExecutionDataSP Executor::ExecuteProcessAsyncStdInStrems(TestCase& input, int) {
  ExecutionTracker::Get()->Reset();
  ba::io_service ios;
  std_write_handler std_in_handler(ios, 10 * 4096);
  std_in_handler.Init(input);

  //  for (int i = 0; i < 20 * 1000; ++i) {
  //    volatile std_write_handler std_in_handler(ios, 10 * 4096);
  //    volatile int a = 0;
  //    for (int i = 0; i < 1000; ++i) {
  //      a += i;
  //    }
  //  }

  std::error_code ec;
  boost::process::child sut;
  boost::system::error_code boot_ec;
  auto start = std::chrono::system_clock::now();

  sut = boost::process::child(
      sut_path_, input.string(), boost::process::std_out = boost::process::null,
      boost::process::std_err = boost::process::null,
      boost::process::std_in = std_in_handler.pipe(), sut_env_, ios, ec);

  // does not work ;(
  if (ec.value() != 0) {
    std::cout << "ExecuteProcessStdInFile fallback " << ec.value();
    return ExecuteProcessStdInFile(input);
  }

  // execution finished...execution_timeout_doest work here
  auto timeout_not_occured = sut.wait_for(execution_timeout_, ec);
  auto finish = std::chrono::system_clock::now();
  if (!timeout_not_occured) {
    sut.terminate(ec);
    finish = std::chrono::system_clock::now();
  }
  auto exit_code = sut.exit_code();
  //  ios.stop();
  //  ios.reset();

  return std::make_shared<ExecutionData>(
      input, ec, exit_code, !timeout_not_occured,
      std::chrono::duration_cast<std::chrono::microseconds>(finish - start),
      std::make_shared<std::stringstream>(),
      std::make_shared<std::stringstream>(),
      ExecutionTracker::Get()->GetCoverage());
}

ExecutionDataSP Executor::ExecuteProcessAsyncStdInStrems(TestCase& input) {
  ExecutionTracker::Get()->Reset();
  ba::io_service ios;
  std_write_handler std_in_handler(ios, 10 * 4096);
  std_in_handler.Init(input);

  //  for (int i = 0; i < 20 * 1000; ++i) {
  //    volatile std_write_handler std_in_handler(ios, 10 * 4096);
  //    volatile int a = 0;
  //    for (int i = 0; i < 1000; ++i) {
  //      a += i;
  //    }
  //  }

  std::error_code ec;
  boost::process::child sut;
  boost::system::error_code boot_ec;
  auto start = std::chrono::system_clock::now();

  sut = boost::process::child(
      sut_path_, input.string(), boost::process::std_out > boost::process::null,
      boost::process::std_err > boost::process::null,
      boost::process::std_in = std_in_handler.pipe(), sut_env_, ios);
  //  ios_.run(boot_ec);
  //
  //  if (ec.value() != 0) {
  //    volatile int debug = 5;
  //    debug = 6;
  //  }

  // execution finished...execution_timeout_doest work here
  auto timeout_not_occured = sut.wait_for(execution_timeout_, ec);
  auto finish = std::chrono::system_clock::now();
  if (!timeout_not_occured) {
    sut.terminate(ec);
    finish = std::chrono::system_clock::now();
  }
  auto exit_code = sut.exit_code();
  //  ios.stop();
  //  ios.reset();

  return std::make_shared<ExecutionData>(
      input, ec, exit_code, !timeout_not_occured,
      std::chrono::duration_cast<std::chrono::microseconds>(finish - start),
      std::make_shared<std::stringstream>(),
      std::make_shared<std::stringstream>(),
      ExecutionTracker::Get()->GetCoverage());
}

ExecutionDataSP Executor::ExecuteProcessAsyncStdAllStrems(TestCase& input) {
  ExecutionTracker::Get()->Reset();
  ba::io_service ios;

  std_write_handler std_in_handler(ios, 10 * 4096);
  std_read_handler std_out_handler(ios, 2 * 4096);
  std_read_handler std_err_handler(ios, 2 * 4096);

  std_in_handler.Init(input);
  std::error_code ec;
  boost::process::child sut;
  boost::system::error_code boot_ec;

  auto start = std::chrono::system_clock::now();
  sut = boost::process::child(sut_path_, input.string(),
                              boost::process::std_out > std_out_handler.pipe(),
                              boost::process::std_err > std_err_handler.pipe(),
                              boost::process::std_in < std_in_handler.pipe(),
                              sut_env_, ios);

  //  ios.run(boot_ec);
  while (1) {
    auto handlers = ios.poll_one(boot_ec);
    if (boot_ec.value() != 0) {
      volatile int dbeug = 1;
      break;
    }

    auto now = std::chrono::system_clock::now();
    if ((now - start) > execution_timeout_) {
      volatile int dbeug = 1;
      break;
    }
    //    handlers = ios.run_one(boot_ec);
    //    if (boot_ec.value() != 0) {
    //      volatile int dbeug = 1;
    //      break;
    //    }
  }

  //  ios.run_one(boot_ec);
  auto timeout_not_occured = sut.wait_for(execution_timeout_, ec);
  auto finish = std::chrono::system_clock::now();
  if (!timeout_not_occured) {
    sut.terminate(ec);
    finish = std::chrono::system_clock::now();
  }
  auto exit_code = sut.exit_code();

  std_in_handler.pipe().cancel();
  std_out_handler.pipe().cancel();
  std_err_handler.pipe().cancel();

  std_in_handler.pipe().close();
  std_out_handler.pipe().close();
  std_err_handler.pipe().close();

  return std::make_shared<ExecutionData>(
      input, ec, exit_code, !timeout_not_occured,
      std::chrono::duration_cast<std::chrono::microseconds>(finish - start),
      std_out_handler.our_streamer, std_out_handler.our_streamer,
      ExecutionTracker::Get()->GetCoverage());
}

ExecutionDataSP Executor::ExecuteProcessStdInFile(TestCase& input) {
  ExecutionTracker::Get()->Reset();

  auto input_file_path = boost::filesystem::path("input.txt");
  boost::filesystem::ofstream input_file(input_file_path);
  input_file << input.string();

  auto start = std::chrono::system_clock::now();
  int exit_code = bp::system(sut_path_, sut_env_, input.string(),
                             boost::process::std_out > boost::process::null,
                             boost::process::std_err > boost::process::null,
                             boost::process::std_in < input_file_path);
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

  auto stdin_ap_buffer = ba::buffer(input.string());
  auto stdin_ap = bp::async_pipe(ios);
  async_handler stdin_handler = [&](const bs::error_code& ec, size_t n) {
    stdin_ap.async_close();
  };

  auto start = std::chrono::system_clock::now();
  boost::process::child sut(
      sut_path_, input.string(), sut_env_, boost::process::std_out > stdout_ap,
      boost::process::std_err > stderr_ap, boost::process::std_in < stdin_ap);

  ba::async_write(stdin_ap, stdin_ap_buffer, stdin_handler);
  ba::async_read(stdout_ap, stdout_ap_buffer, stdout_handler);
  ba::async_read(stderr_ap, stderr_ap_buffer, stderr_handler);

  std::error_code ec;
  boost::system::error_code boot_ec;
  while (true) {
    auto now = std::chrono::system_clock::now();
    if ((now - start > execution_timeout_) || !sut.running()) {
      break;
    }

    ios.poll_one(boot_ec);
    if (boot_ec.value()) {
      break;
    }

    if (ios.stopped()) {
      ios.reset();
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

} /* namespace fuzzon */
