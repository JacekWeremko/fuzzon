/*
 * fuzzon.h
 *
 *  Created on: Aug 21, 2017
 *      Author: dablju
 */

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
	int Run(std::string sut_path, std::string intput_format);
private:
	std::string output_dir_;

};

}

#endif /* FUZZON_H_ */
