
#include "fuzzon_executiondata.h"

namespace fuzzon {

ExecutionData::ExecutionData(TestCase input, std::error_code ec,
		std::stringstream& std_out, std::stringstream& std_err,
		const Coverage* coverage) :
		input_(input), ec_(ec),
		std_out_(std_out.str()), std_err_(std_err.str()), /* TODO: performance */
		coverage_(*coverage),
		similar_execution_coutner_(0), mutation_usage_count_(0), execution_time_(0.0f)
{
	coverage_.Compress();
}

ExecutionData::~ExecutionData() {
	// TODO Auto-generated destructor stub
}

} /* namespace fuzzon */
