

#include "fuzzon.h"

#include <boost/filesystem.hpp>
#include <boost/date_time.hpp>

#ifndef UNIT_TEST

int main(int argc, char **argv)
{
	// TODO: configure w.r.t **argv

	auto time_now     = boost::posix_time::second_clock::local_time();
	auto time_now_str = boost::posix_time::to_simple_string(time_now);
	auto output_dir   = boost::filesystem::path("Output").append(time_now_str);

	std::string sut_path, intput_format;
	if (argc == 1)
	{
		auto current_path   = boost::filesystem::current_path();
//		auto sut_path     = current_path/".."/"test"/"application"/"branchness"/"Debug"/"branchness";
//		auto intput_format= current_path/".."/"test"/"application"/"branchness"/"branchness.json";
//
//		auto sut_path     = current_path/"branchness";
//		auto intput_format= current_path/"branchness.json";
//
//		auto sut_path     = current_path/"test"/"application"/"branchness"/"Debug"/"branchness";
//		auto intput_format= current_path/"test"/"application"/"branchness"/"branchness.json";

		auto sut_pathb      = current_path/"test"/"application"/"branchness"/"Debug"/"branchness";
		auto intput_formatb = sut_pathb/"test"/"application"/"branchness"/"branchness.json";

		sut_path = sut_pathb.string();
		intput_format = intput_formatb.string();
	}
	else if(argc == 3)
	{
		sut_path = std::string(argv[1]);
		intput_format = std::string(argv[2]);
	}
	else
	{
		return -1;
	}

	auto iterations = 50;
	auto test_cases_to_generate = 5;
	auto test_cases_to_mutate = 200;
	auto statistics_print_interval = 10;

	fuzzon::Fuzzon crazy_fuzzer = fuzzon::Fuzzon(output_dir.string());
	return crazy_fuzzer.Run(sut_path, intput_format, iterations,
			test_cases_to_generate, test_cases_to_mutate,
			statistics_print_interval);
}


#endif

