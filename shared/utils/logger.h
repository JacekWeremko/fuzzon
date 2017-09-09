/*
 * Copyright [2017] Jacek Weremko
 */

#ifndef SHARED_UTILS_LOGGER_H_
#define SHARED_UTILS_LOGGER_H_

#include <string>

class Logger {
 public:
  explicit Logger(std::string output_direcotry, int logging_level);

  Logger(Logger const&) = delete;
  void operator=(Logger const&) = delete;

  static Logger* Get(std::string output_direcotry = "", int logging_level = 6) {
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

#ifdef LOGGING_ENABLED
#define LOG_TRACE(message) Logger::Get()->trace(message);
#define LOG_DEBUG(message) Logger::Get()->debug(message);
#define LOG_INFO(message) Logger::Get()->info(message);
#define LOG_WARNING(message) Logger::Get()->warning(message);
#define LOG_ERROR(message) Logger::Get()->error(message);
#define LOG_CRITICAL(message) Logger::Get()->critical(message);
#else
#define LOG_TRACE(message)
#define LOG_DEBUG(message)
#define LOG_INFO(message)
#define LOG_WARNING(message)
#define LOG_ERROR(message)
#define LOG_CRITICAL(message)
#endif

#endif  // SHARED_UTILS_LOGGER_H_
