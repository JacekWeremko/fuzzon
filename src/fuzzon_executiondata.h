
#ifndef FUZZON_EXECUTIONDATA_H_
#define FUZZON_EXECUTIONDATA_H_

#include "fuzzon_testcase.h"
#include "fuzzon_coverage.h"

#include <system_error>
#include <sstream>


namespace fuzzon {

class ExecutionData {
public:
	ExecutionData(TestCase input, std::error_code ec,
			std::stringstream& std_out, std::stringstream& std_err,
			const Coverage* coverage);

	virtual ~ExecutionData();

//private:
	TestCase input_;
	std::error_code ec_;
	std::string std_out_;
	std::string std_err_;
	Coverage coverage_;

	size_t similar_execution_coutner_;
	size_t mutation_usage_count_;
	double execution_time_;
};

} /* namespace fuzzon */

#endif /* FUZZON_EXECUTIONDATA_H_ */
