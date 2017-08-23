//============================================================================
// Name        : fuzzon_main.cpp
// Author      : Jacek Weremko
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================


#include "fuzzon.h"

#include <boost/filesystem.hpp>
#include <boost/date_time.hpp>


int main(int argc, char **argv)
{
	// TODO: configure w.r.t **argv

	auto time_now     = boost::posix_time::second_clock::local_time();
	auto time_now_str = boost::posix_time::to_simple_string(time_now);
	auto output_dir   = boost::filesystem::path("Output").append(time_now_str);
	//	boost::filesystem::path output_path(boost::filesystem::current_path().append("output"));

	auto sut_path     = boost::filesystem::current_path()/"test"/"application"/"branchness"/"branchness";
	auto intput_format= boost::filesystem::current_path()/"test"/"application"/"branchness"/"branchness.json";

	fuzzon::Fuzzon crazy_fuzzer = fuzzon::Fuzzon(output_dir.string());
	return crazy_fuzzer.Run(sut_path.string(), intput_format.string());
}


