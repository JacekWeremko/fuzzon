/*
 * fuzzon_executor.cpp
 *
 *  Created on: Aug 22, 2017
 *      Author: dablju
 */

#include "fuzzon_executor.h"

#include <boost/process.hpp>

extern int FuzzonSUTEntryPoint(int argc, char **argv);


namespace fuzzon {

Executor::Executor(std::string sut_path) : sut_path_(sut_path)
{
	// TODO Auto-generated constructor stub

}

Executor::~Executor() {
	// TODO Auto-generated destructor stub
}


ExecutionData Executor::ExecuteBlocking(TestCase input)
{
	ExecutionMonitor::Get()->Reset();

	//	boost::process::system(sut_path_ + " " + input.string(), ec);
	//std::error_code ec = std::error_code(FuzzonSUTEntryPoint(input.data(), input.length()), std::generic_category());
	int argc = input.argc() + 1;
	auto argv_string = sut_path_ + " " + input.string();
	char* argv = const_cast<char*>(argv_string.c_str());

	std::error_code ec = std::error_code(FuzzonSUTEntryPoint(argc, &argv), std::generic_category());

	ExecutionData execution_output(input, ec, ExecutionMonitor::Get()->GetCoverage());


	ExecutionMonitor::Get()->PrintTrace();
	return execution_output;
}

} /* namespace fuzzon */
