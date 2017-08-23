
#include "fuzzon_executiondata.h"

namespace fuzzon {

ExecutionData::ExecutionData(TestCase input, std::error_code ec, Coverage coverage) :
		input_(input), ec_(ec), coverage_(coverage)
{
	// TODO Auto-generated constructor stub

}

ExecutionData::~ExecutionData() {
	// TODO Auto-generated destructor stub
}

} /* namespace fuzzon */
