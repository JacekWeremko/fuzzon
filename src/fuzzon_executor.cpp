
#include "fuzzon_executor.h"
#include "utils/logger.h"

#include "fuzzon_executiontracker.h"

#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <boost/process.hpp>
#include <boost/asio/io_service.hpp>



//extern int FuzzonSUTEntryPoint(int argc, char **argv);


namespace fuzzon {

Executor::Executor(std::string sut_path) : sut_path_(sut_path)
{
	ExecutionTracker::Get(ExecutionTracker::Monitor);
}

Executor::~Executor() {
	// TODO Auto-generated destructor stub
}


//std::unique_ptr<ExecutionData> Executor::ExecuteBlocking(TestCase& input)
ExecutionData Executor::ExecuteBlocking(TestCase& input)
{
	ExecutionTracker::Get()->Reset();

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

	Logger::Get()->debug(std::string("sut_path_: ") + sut_path_);
	Logger::Get()->debug(std::string("argc: ") + std::to_string(argc) + std::string("  argv:") + input.string());
//	Logger::Get()->debug(std::string("  argv : ") + argv_string);
//	Logger::Get()->debug(std::string("  argv : ") + input.string());


	Logger::Get()->debug("Execution begin");
	boost::process::group group;

	boost::asio::io_service ios;
//	std::future<std::string> sut_std_out;
//	std::future<std::string> sut_std_err;
//	boost::process::child sut(sut_path_, input.string(), group,
//			boost::process::std_out > sut_std_out,
//			boost::process::std_err > sut_std_err,
//			ios);
//	boost::process::child sut(sut_path_, input.string(), group,
//			boost::process::std_out > sut_std_out,
//			ios);
	std::string sut_std_out_value, sut_std_err_value;
	boost::process::ipstream sut_std_out_ips;
	boost::process::ipstream sut_std_err_ips;

	boost::process::child sut(sut_path_, input.string(), group,
			boost::process::std_out > sut_std_out_ips,
			boost::process::std_err > sut_std_err_ips);


	std::error_code ec = std::error_code(-999, std::system_category());
//	if (!sut.wait_for(std::chrono::seconds(execution_timeout_sec_)))
//	{
//		sut.terminate(ec);
//	}
	if (!group.wait_for(std::chrono::seconds(execution_timeout_sec_)))
	{
		group.terminate(ec);
	}
	sut.wait();
	ec = std::error_code(sut.exit_code(), std::generic_category());


	/* TODO: performance */
	std::string line;
    std::stringstream sut_std_out;
    while (std::getline(sut_std_out_ips, line))
    	sut_std_out << line << std::endl;

    std::stringstream sut_std_err;
    while (std::getline(sut_std_err_ips, line))
    	sut_std_err << line << std::endl;

	Logger::Get()->debug("Execution end");

	delete [] argv;



	Logger::Get()->debug(std::string("  ec: ") + std::to_string(ec.value()));
	Logger::Get()->debug(std::string("  sut_std_out: ") + sut_std_out.str());
	Logger::Get()->debug(std::string("  sut_std_err: ") + sut_std_err.str());

	ExecutionTracker::Get()->GetCoverage()->PrintTrace();

	try
	{
//	auto execution_output = std::make_unique<ExecutionData>(input, ec, sut_std_out, sut_std_err,*ExecutionTracker::Get()->GetCoverage());
	ExecutionData execution_output(input, ec, sut_std_out, sut_std_err, ExecutionTracker::Get()->GetCoverage());
	}
	catch(std::runtime_error& ex)
	{
		std::cout << ec.message();
	}

	ExecutionData sdg(input, ec, sut_std_out, sut_std_err, ExecutionTracker::Get()->GetCoverage());
	ExecutionTracker::Get()->GetCoverage()->PrintTrace();
	return sdg;
}

} /* namespace fuzzon */
