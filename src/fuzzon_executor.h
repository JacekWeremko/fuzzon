/*
 * fuzzon_executor.h
 *
 *  Created on: Aug 22, 2017
 *      Author: dablju
 */

#ifndef FUZZON_EXECUTOR_H_
#define FUZZON_EXECUTOR_H_

#include <string>

#include "fuzzon_testcase.h"
#include "fuzzon_executionmonitor.h"
#include "fuzzon_executiondata.h"

namespace fuzzon {

class Executor {
public:
	Executor(std::string sut_path);
	virtual ~Executor();

	ExecutionData ExecuteBlocking(TestCase input);

private:
	std::string sut_path_;
//	ExecutionMonitor monitor_;
};

} /* namespace fuzzon */

#endif /* FUZZON_EXECUTOR_H_ */
