/*
 * Copyright [2017] Jacek Weremko
 */

#ifndef UTILS_TIMEOUT_HPP_
#define UTILS_TIMEOUT_HPP_

#include <chrono>

class Timeout {
 public:
  explicit Timeout(std::chrono::milliseconds timeout) : start_(std::chrono::system_clock::now()), timeout_(timeout) {}
  void arm(std::chrono::system_clock::time_point start) { start_ = start; }
  bool operator()() {
    auto now = std::chrono::system_clock::now();
    return ((now - start_) > timeout_);
  }

 private:
  std::chrono::system_clock::time_point start_;
  std::chrono::milliseconds timeout_;
};

#endif /* UTILS_TIMEOUT_HPP_ */
