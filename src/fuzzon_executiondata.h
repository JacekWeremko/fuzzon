#ifndef FUZZON_EXECUTIONDATA_H_
#define FUZZON_EXECUTIONDATA_H_

#include "fuzzon_testcase.h"
#include "fuzzon_coverage.h"

#include <system_error>
#include <sstream>
#include <chrono>

namespace fuzzon
{

struct ExecutionData
{
	ExecutionData(TestCase tc, std::error_code erc, int exc, bool gracefully_finished,
			std::chrono::microseconds execution_time,
			std::stringstream& std_out, std::stringstream& std_err,
			const Coverage* cov) :
			input(tc), error_code(erc), exit_code(exc), gracefully_finished(gracefully_finished),
			execution_time(execution_time),
			std_out(std_out.str()), std_err(std_err.str()), /* TODO: performance */
			coverage(*cov), coverage_coutner_(0), mutation_counter_(0)
	{
		coverage.Compress();
	}

	TestCase input;
	std::error_code error_code;
	int exit_code;
	bool gracefully_finished;
	std::chrono::microseconds execution_time;

	std::string std_out;
	std::string std_err;
	Coverage coverage;

	//TODO: shouln't be part of this class
	size_t coverage_coutner_;
	size_t mutation_counter_;
};

} /* namespace fuzzon */

#endif /* FUZZON_EXECUTIONDATA_H_ */
