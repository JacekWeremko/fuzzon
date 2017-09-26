/*
 * Copyright [2017] Jacek Weremko
 */

#define BOOST_LOG_DYN_LINK 1

#include "./logger.h"

#include <boost/log/utility/setup.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

#include <iostream>
#include <string>

// namespace logging = boost::log;
// namespace src = boost::log::sources;
// namespace keywords = boost::log::keywords;

Logger::Logger(std::string output_direcotry, int logging_level) {
  static const std::string log_format(
      "[%TimeStamp%][%Severity%][%LineID%]: %Message%");

  boost::log::add_console_log(std::cout,
                              boost::log::keywords::format = log_format,
                              boost::log::keywords::auto_flush = true);

  boost::log::add_common_attributes();

  boost::log::core::get()->set_filter(boost::log::trivial::severity >=
                                      boost::log::trivial::fatal -
                                          logging_level);
}

void Logger::SetLevel(int logging_level) {
  boost::log::core::get()->set_filter(boost::log::trivial::severity >=
                                      boost::log::trivial::fatal -
                                          logging_level);
}

void Logger::trace(std::string message) {
  BOOST_LOG_TRIVIAL(trace) << message;
}

void Logger::debug(std::string message) {
  BOOST_LOG_TRIVIAL(debug) << message;
}
void Logger::info(std::string message) {
  BOOST_LOG_TRIVIAL(info) << message;
}

void Logger::warning(std::string message) {
  BOOST_LOG_TRIVIAL(warning) << message;
}

void Logger::error(std::string message) {
  BOOST_LOG_TRIVIAL(error) << message;
}

void Logger::critical(std::string message) {
  BOOST_LOG_TRIVIAL(fatal) << message;
}
