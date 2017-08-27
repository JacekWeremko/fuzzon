#ifndef FUZZON_H_
#define FUZZON_H_

#include "utils/logger.h"

#include <boost/filesystem.hpp>

namespace fuzzon
{

class Fuzzon
{
public:
	Fuzzon(std::string output_dir);
	int Run(std::string sut_path, std::string input_format, int iterations,
			int test_cases_to_generate, int test_cases_to_mutate,
			int statistics_print_interval);
private:
	std::string output_dir_;
};

}

#endif /* FUZZON_H_ */
