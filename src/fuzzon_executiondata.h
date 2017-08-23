/*
 * fuzzon_output.h
 *
 *  Created on: Aug 22, 2017
 *      Author: dablju
 */

#ifndef FUZZON_EXECUTIONDATA_H_
#define FUZZON_EXECUTIONDATA_H_

#include "fuzzon_testcase.h"
#include "fuzzon_coverage.h"

#include <system_error>


namespace fuzzon {

class ExecutionData {
public:
	ExecutionData(TestCase input, std::error_code ec, Coverage coverage);

	virtual ~ExecutionData();

//private:
	TestCase input_;
	std::error_code ec_;
	Coverage coverage_;
};

} /* namespace fuzzon */

#endif /* FUZZON_EXECUTIONDATA_H_ */
