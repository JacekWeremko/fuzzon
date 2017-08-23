/*
 * fuzzon_generator.h
 *
 *  Created on: Aug 20, 2017
 *      Author: dablju
 */

#ifndef FUZZON_GENERATOR_H_
#define FUZZON_GENERATOR_H_

#include <string>
#include "fuzzon_testcase.h"

namespace fuzzon
{

class Generator
{
public:
	Generator(std::string format_filepath, std::string output_directory);

	TestCase generateNext();

private:
	std::string format_filepath_;
	std::string output_directory_;
};

}

#endif /* FUZZON_GENERATOR_H_ */
