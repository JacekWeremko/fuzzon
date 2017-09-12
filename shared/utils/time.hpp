/*
 * Copyright [2017] Jacek Weremko
 */

#ifndef UTILS_TIME_HPP_
#define UTILS_TIME_HPP_

#include <chrono>

class Timeout {
 public:
  explicit Timeout(std::chrono::milliseconds timeout)
      : start_(std::chrono::system_clock::now()), timeout_(timeout) {}
  void arm(std::chrono::system_clock::time_point start) { start_ = start; }
  bool operator()() const {
    auto now = std::chrono::system_clock::now();
    return ((now - start_) > timeout_);
  }

 private:
  std::chrono::system_clock::time_point start_;
  std::chrono::milliseconds timeout_;
};

//// std::string time_format(std::chrono::system_clock::time_point format_me) {
// inline std::string time_format(std::chrono::system_clock::duration format_me)
// {
//  auto hh = std::chrono::duration_cast<std::chrono::hours>(format_me).count();
//  auto mm = std::chrono::duration_cast<std::chrono::minutes>(format_me %
//  std::chrono::hours(1)).count();
//  auto ss = std::chrono::duration_cast<std::chrono::seconds>(format_me %
//  std::chrono::minutes(1)).count();
//  auto mi = std::chrono::duration_cast<std::chrono::milliseconds>(format_me %
//  std::chrono::seconds(1)).count();
//
//  std::stringstream formatted;
//  formatted << hh << ":" << mm << ":" << ss << ":" << mi;
//  return formatted.str();
//}

inline std::string time_format(const std::chrono::microseconds format_me) {
  auto hh = std::chrono::duration_cast<std::chrono::hours>(format_me).count();
  auto mm = std::chrono::duration_cast<std::chrono::minutes>(
                format_me % std::chrono::hours(1))
                .count();
  auto ss = std::chrono::duration_cast<std::chrono::seconds>(
                format_me % std::chrono::minutes(1))
                .count();
  auto ml = std::chrono::duration_cast<std::chrono::milliseconds>(
                format_me % std::chrono::seconds(1))
                .count();
  auto mi = std::chrono::duration_cast<std::chrono::microseconds>(
                format_me % std::chrono::milliseconds(1))
                .count();
  std::stringstream formatted;
  formatted << hh << ":" << mm << ":" << ss << ":" << ml << ":" << mi;
  return formatted.str();
}

#endif /* UTILS_TIME_HPP_ */
