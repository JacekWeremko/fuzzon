
#include "fuzzon_executiondata.h"

namespace fuzzon {

ExecutionData::ExecutionData(TestCase input, std::error_code ec, Coverage coverage) :
		input_(input), ec_(ec), coverage_(coverage),
		similar_execution_coutner_(0), mutation_usage_count_(0)
{
	coverage_.Compress();
}

ExecutionData::~ExecutionData() {
	// TODO Auto-generated destructor stub
}

} /* namespace fuzzon */
