/*
 * logger.h
 *
 *  Created on: Apr 29, 2017
 *      Author: dablju
 */

#include "logger.h"

#include <iostream>

//#define BOOST_ALL_NO_LIB 1   // do not auto link, instead you link manually to your import libraries.
//#define BOOST_ALL_DYN_LINK 1 // tells you will link to shared libraries
#define BOOST_LOG_DYN_LINK 1 // explicitly says to link boost::log symbols to shared libraries

//#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup.hpp>
//#include <boost/log/sinks/text_ostream_backend.hpp>

#include <boost/log/trivial.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>


namespace logging = boost::log;
namespace src = boost::log::sources;
namespace keywords = boost::log::keywords;


Logger::Logger(std::string output_direcotry)
{
    static const std::string log_format("[%TimeStamp%][%Severity%][%LineID%]: %Message%");

    boost::log::register_simple_formatter_factory<boost::log::trivial::severity_level, char>("Severity");

    //auto file_name = boost::filesystem::path(output_direcotry).append("execution.log");
//    auto file_name = boost::filesystem::path(output_direcotry).append("execution.log");
//    boost::log::add_file_log(
//        boost::log::keywords::file_name = "sample_%N.log",
//		boost::log::keywords::rotation_size = 10 * 1024 * 1024,
//		boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0),
//        boost::log::keywords::format = log_format,
//        boost::log::keywords::auto_flush = true
//    );
//    boost::log::add_file_log(
//		boost::log::keywords::format = log_format,
//		boost::log::keywords::auto_flush = true
//    );

//    boost::log::add_file_log(
//
//		boost::log::keywords::rotation_size = 10 * 1024 * 1024,
//		boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0),
//    boost::log::keywords::file_name = "sample_%N.log"
//    );
//        boost::log::add_file_log("sample_.log");
//        logging::add_file_log
//          (
//              keywords::file_name = boost::filesystem::path("sample_%N.log")
//          );
//    logging::add_file_log
//        (
//            keywords::file_name = "sample_%N.log",
//            keywords::rotation_size = 10 * 1024 * 1024,
//            keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),
//            keywords::format = "[%TimeStamp%]: %Message%"
//        );
//    boost::log::keywords::


    boost::log::add_console_log(
        std::cout,
        boost::log::keywords::format = log_format,
        boost::log::keywords::auto_flush = true
    );

    boost::log::add_common_attributes();

    boost::log::core::get()->set_filter(
        boost::log::trivial::severity>= boost::log::trivial::trace
    );
}

void Logger::trace(std::string message)
{
	BOOST_LOG_TRIVIAL(trace) << message;
}

void Logger::debug(std::string message)
{
	BOOST_LOG_TRIVIAL(debug) << message;
}
void Logger::info(std::string message)
{
	BOOST_LOG_TRIVIAL(info) << message;
}

void Logger::warning(std::string message)
{
	BOOST_LOG_TRIVIAL(warning) << message;
}

void Logger::error(std::string message)
{
	BOOST_LOG_TRIVIAL(error) << message;
}

void Logger::critical(std::string message)
{
	BOOST_LOG_TRIVIAL(fatal) << message;
}

