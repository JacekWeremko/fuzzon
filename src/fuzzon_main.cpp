/*
 * Copyright [2017] Jacek Weremko
 */

#include "./fuzzon.h"

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/date_time.hpp>
#include <string>

#include "./fuzzon_random.h"

#ifndef UNIT_TEST

// using namespace std;
namespace po = boost::program_options;
namespace fs = boost::filesystem;

int main(int argc, char** argv) {
  // TODO: configure w.r.t **argv

  auto time_now = boost::posix_time::second_clock::local_time();
  auto time_now_str = boost::posix_time::to_simple_string(time_now);

  po::options_description desc("Options");
  desc.add_options()("help", "produce help message");
  desc.add_options()("sut,s", po::value<fs::path>(),
                     "Software under test path");
  desc.add_options()("input_format,i", po::value<fs::path>(),
                     "JSON Input format file path");
  desc.add_options()("out,o", po::value<fs::path>(), "Output directory path");
  desc.add_options()("sut_timeout,t", po::value<int>(),
                     "Max allowed time to execute SUT");
  desc.add_options()("generate,g", po::value<int>(),
                     "Number of test cases to exercise in generation phase.");
  desc.add_options()("mutate,m", po::value<int>(),
                     "Number of test cases to mutate in mutation phase.");
  desc.add_options()("white_chars_preservation,w", po::value<int>(),
                     "Preserve white characters during mutations.");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.empty() || vm.count("help")) {
    std::cout << desc << std::endl;
    return 1;
  }

  if (!vm.count("sut") || !vm.count("input_format")) {
    std::cout << "REQUIRED : sut and input_format" << std::endl;
    return 2;
  }

  auto sut = vm["sut"].as<fs::path>();
  auto input_format = vm["input_format"].as<fs::path>();

  auto output = vm.count("out") ? vm["out"].as<fs::path>() : sut.parent_path();
  output /= (fs::path("fuzzon_").concat(sut.filename().c_str())) / time_now_str;

  auto sut_timeout = vm.count("t") ? vm["t"].as<int>() : 1000;
  auto generate = vm.count("generate") ? vm["generate"].as<int>() : 100;
  auto mutate = vm.count("mutate") ? vm["mutate"].as<int>() : -1;
  auto white_chars_preservation =
      vm.count("white_chars_preservation")
          ? vm["white_chars_preservation"].as<bool>()
          : true;

  std::string input_alphabet("abcd");
  fuzzon::Random::Get()->SetAlphabet(input_alphabet);
  fuzzon::Fuzzon crazy_fuzzer(output.string(), sut.string(), sut_timeout);

  crazy_fuzzer.Generation(input_format.string(), generate);
  crazy_fuzzer.MutationDeterministic(white_chars_preservation);
  crazy_fuzzer.MutationNonDeterministic(mutate, white_chars_preservation);

  return 0;
}

#endif
