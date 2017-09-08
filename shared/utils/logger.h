/*
 * Copyright [2017] Jacek Weremko
 */

#ifndef SHARED_UTILS_LOGGER_H_
#define SHARED_UTILS_LOGGER_H_

#define LOGGING_ENABLED 1

#include <string>

class Logger {
 public:
  explicit Logger(std::string output_direcotry, int logging_level);

  Logger(Logger const&) = delete;
  void operator=(Logger const&) = delete;

  static Logger* Get(std::string output_direcotry = "", int logging_level = 0) {
    static Logger logger(output_direcotry, logging_level);
    return &logger;
  }

  void trace(std::string message);
  void debug(std::string message);
  void info(std::string message);
  void warning(std::string message);
  void error(std::string message);
  void critical(std::string message);
};

#endif  // SHARED_UTILS_LOGGER_H_
