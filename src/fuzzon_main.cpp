

#include "fuzzon.h"
#include "fuzzon_random.h"

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

//		auto sut_pathb      = current_path/"test"/"application"/"branchness"/"Debug"/"branchness";
//		auto intput_formatb = current_path/"test"/"application"/"branchness"/"branchness.json";

		auto sut_pathb      = current_path/"test"/"application"/"arrayness"/"Debug"/"arrayness";
		auto intput_formatb = current_path/"test"/"application"/"arrayness"/"arrayness_propsal2.json";

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


	auto sut_runtime_timeout = 10;
	auto iterations = 50;
	auto test_cases_to_generate = 55;
	auto test_cases_to_mutate = -1;
	auto statistics_print_interval = 10;

	std::string input_alphabet("abcd");
	fuzzon::Random::Get()->SetAlphabet(input_alphabet);
	fuzzon::Fuzzon crazy_fuzzer = fuzzon::Fuzzon(output_dir.string(), sut_path, sut_runtime_timeout);

	crazy_fuzzer.GenerationPhase(intput_format, test_cases_to_generate);
	crazy_fuzzer.MutationPhaseDeterministic();
	crazy_fuzzer.MutationPhaseNonDeterministic(test_cases_to_mutate);

	return 0;
}


#endif

