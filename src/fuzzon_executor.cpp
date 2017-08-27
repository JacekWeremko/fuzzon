#include "fuzzon_executor.h"
#include "fuzzon_executiontracker.h"
#include "utils/logger.h"

#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <boost/process.hpp>


namespace fuzzon {

Executor::Executor(std::string sut_path, int execution_timeout_sec) :
		sut_path_(sut_path), execution_timeout_sec_(execution_timeout_sec)
{
	ExecutionTracker::Get(ExecutionTracker::Monitor);
	Logger::Get()->debug(std::string("SUT path: ") + sut_path_);
}

ExecutionData Executor::ExecuteBlocking(TestCase& input)
{
	ExecutionTracker::Get()->Reset();
//	Logger::Get()->debug(std::string("argv : ") + input.string());

	std::string sut_std_out_value, sut_std_err_value;
	boost::process::ipstream sut_std_out_ips;
	boost::process::ipstream sut_std_err_ips;
	boost::process::child sut(sut_path_, input.string(),
			boost::process::std_out > sut_std_out_ips,
			boost::process::std_err > sut_std_err_ips);

	std::error_code ec;
    auto start = std::chrono::system_clock::now();
	auto gracefully_finished = sut.wait_for(std::chrono::seconds(execution_timeout_sec_), ec);
	auto finish = std::chrono::system_clock::now();
	if (!gracefully_finished)
	{
		sut.terminate(ec);
		finish = std::chrono::system_clock::now();
	}
	auto exit_code = sut.exit_code();

	/* TODO: performance */
	std::string line;
    std::stringstream sut_std_out;
    while (std::getline(sut_std_out_ips, line))
    	sut_std_out << line << std::endl;

    std::stringstream sut_std_err;
    while (std::getline(sut_std_err_ips, line))
    	sut_std_err << line << std::endl;

//	Logger::Get()->debug(std::string("  ec: ") + std::to_string(ec.value()));
//	Logger::Get()->debug(std::string("  ec: ") + std::string(ec.message()));
//	Logger::Get()->debug(std::string("  sut_std_out: ") + sut_std_out.str());
//	Logger::Get()->debug(std::string("  sut_std_err: ") + sut_std_err.str());
	return ExecutionData(
			input, ec, exit_code, gracefully_finished,
			std::chrono::microseconds((finish - start).count()),
			sut_std_out, sut_std_err,
			ExecutionTracker::Get()->GetCoverage());
}

} /* namespace fuzzon */
