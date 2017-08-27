

#ifndef DEBUG_UTILS_LOGGER_H_
#define DEBUG_UTILS_LOGGER_H_

#define LOGGING_ENABLED 1

#include <string>


class Logger {
public:
	Logger(std::string output_direcotry);

	Logger(Logger const&) = delete;
	void operator=(Logger const&) = delete;

	static Logger* Get(std::string output_direcotry = "")
	{
		static Logger logger(output_direcotry);
		return &logger;
	}

	void trace(std::string message);
	void debug(std::string message);
	void info(std::string message);
	void warning(std::string message);
	void error(std::string message);
	void critical(std::string message);
};

#endif /* DEBUG_UTILS_LOGGER_H_ */
