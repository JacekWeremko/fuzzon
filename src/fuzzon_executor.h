
#ifndef FUZZON_EXECUTOR_H_
#define FUZZON_EXECUTOR_H_

#include "fuzzon_testcase.h"
#include "fuzzon_executiondata.h"
#include <string>
#include <memory>

namespace fuzzon {

class Executor {
public:
	Executor(std::string sut_path);
	virtual ~Executor();

	ExecutionData ExecuteBlocking(TestCase& input);
//	std::unique_ptr<ExecutionData> ExecuteBlocking(TestCase& input);

private:
	std::string sut_path_;
	const int execution_timeout_sec_ = 60 * 1; // 60 sec
};

} /* namespace fuzzon */

#endif /* FUZZON_EXECUTOR_H_ */
