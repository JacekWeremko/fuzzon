
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

	size_t similar_execution_coutner_;
	size_t mutation_usage_count_;
	double execution_time_;
};

} /* namespace fuzzon */

#endif /* FUZZON_EXECUTIONDATA_H_ */
