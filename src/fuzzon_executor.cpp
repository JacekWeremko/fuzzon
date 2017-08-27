
#include "fuzzon_executor.h"

#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
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


ExecutionData Executor::ExecuteBlocking(TestCase& input)
{
	ExecutionMonitor::Get()->Reset();

	//	boost::process::system(sut_path_ + " " + input.string(), ec);
	//std::error_code ec = std::error_code(FuzzonSUTEntryPoint(input.data(), input.length()), std::generic_category());
	int argc = input.argc() + 1;
	auto argv_string = sut_path_ + " " + input.string();
	//char* argv = const_cast<char*>(argv_string.c_str());


	std::istringstream iss(argv_string);
	std::vector<std::string> tokens {std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>()};
	const char** argv = new const char*[tokens.size()];
	for(size_t i=0; i<tokens.size(); i++)
	{
		argv[i] = tokens[i].c_str();
	}

//	Logger::Get()->debug("Execution begin");
	Logger::Get()->debug(std::string("argc: ") + std::to_string(argc) + std::string("  argv:") + input.string());
//	Logger::Get()->debug(std::string("  argv : ") + argv_string);
//	Logger::Get()->debug(std::string("  argv : ") + input.string());
	std::error_code ec = std::error_code(FuzzonSUTEntryPoint(argc, const_cast<char**>(argv)), std::generic_category());
//	Logger::Get()->debug("Execution end");

	delete [] argv;

	ExecutionData execution_output(input, ec, *ExecutionMonitor::Get()->GetCoverage());


	ExecutionMonitor::Get()->GetCoverage()->PrintTrace();
	return execution_output;
}

} /* namespace fuzzon */
