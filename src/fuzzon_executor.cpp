
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

namespace fuzzon {

Executor::Executor(std::string sut_path) : sut_path_(sut_path)
{
	ExecutionTracker::Get(ExecutionTracker::Monitor);
}

Executor::~Executor() {
	// TODO Auto-generated destructor stub
}

ExecutionData Executor::ExecuteBlocking(TestCase& input)
{
	ExecutionTracker::Get()->Reset();
//	Logger::Get()->debug(std::string("sut_path_: ") + sut_path_);
//	Logger::Get()->debug(std::string("argv : ") + input.string());

	std::string sut_std_out_value, sut_std_err_value;
	boost::process::ipstream sut_std_out_ips;
	boost::process::ipstream sut_std_err_ips;
	boost::process::child sut(sut_path_, input.string(),
			boost::process::std_out > sut_std_out_ips,
			boost::process::std_err > sut_std_err_ips);

	std::error_code ec = std::error_code(-999, std::system_category());
	auto is_timeout_occure = sut.wait_for(std::chrono::seconds(execution_timeout_sec_), ec);
	if (is_timeout_occure)
	{
		sut.terminate(ec);
	}

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
	return ExecutionData(input, ec, sut_std_out, sut_std_err, ExecutionTracker::Get()->GetCoverage());
}

} /* namespace fuzzon */
