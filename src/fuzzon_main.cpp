/*
 * Copyright [2017] Jacek Weremko
 */

#include "./fuzzon.h"

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/date_time.hpp>
#include <vector>
#include <string>
#include <utility>

#include "./fuzzon_random.h"
#include "./utils/logger.h"

#ifndef UNIT_TEST

namespace stdch = std::chrono;
namespace po = boost::program_options;
namespace fs = boost::filesystem;

#ifdef ALTERNATIVE_MAIN
int fuzzon_main(int argc, char** argv) {
#else
int main(int argc, char** argv) {
#endif
  auto time_now = boost::posix_time::second_clock::local_time();
  auto time_now_str = boost::posix_time::to_simple_string(time_now);
  std::replace(time_now_str.begin(), time_now_str.end(), ' ', '_');

  std::string verbosity_values;

  po::options_description desc("Options");
  desc.add_options()("help", "produce help message");
  desc.add_options()("verbose,v",
                     po::value<>(&verbosity_values)->implicit_value(""),
                     "Logging level");
  desc.add_options()("sut", po::value<fs::path>(), "Software under test path");
  desc.add_options()("env_flag", po::value<std::vector<std::string>>(),
                     "Environmental flag set for SUT.");
  desc.add_options()("input_format", po::value<fs::path>(),
                     "JSON Input format file path");
  desc.add_options()("output_dir", po::value<fs::path>(),
                     "Output directory path");
  desc.add_options()(
      "corpus_seeds", po::value<fs::path>()->default_value(fs::path("")),
      "Path to directory with sample input files aka corpus seed.");
  desc.add_options()("single_test_timeout", po::value<int>()->default_value(50),
                     "Max allowed time to execute SUT");
  desc.add_options()("total_timeout",
                     po::value<int>()->default_value(1000 * 60 * 30),
                     "Total campaign timeout.");
  desc.add_options()("generate", po::value<int>()->default_value(1000),
                     "Generation phase: number of test cases to generate.");
  desc.add_options()(
      "mutate_d", po::value<int>()->default_value(1),
      "Deterministic mutation phase: level of mutations: 0-None, 1-Max");
  desc.add_options()(
      "mutate_nd", po::value<int>()->default_value(1000),
      "Non-deterministic mutation phase: number of test cases to mutate.");
  desc.add_options()("total_testcases", po::value<int>()->default_value(50000),
                     "Max total test cases number to execute");

  desc.add_options()("white_chars_preservation,w",
                     po::value<bool>()->default_value(true),
                     "Preserve white characters during mutations.");
  desc.add_options()("executor_mode", po::value<int>()->default_value(1),
                     "Execution mode : "
                     "PROCESS_ONLY_STDIN_FILE=0"
                     "PROCESS_ONLY_STDIN_ASYCN_STREAM=1"
                     "PROCESS_ALL_STD_ASYNC_STREAMS=2"
                     "THREAD=3");

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

  auto output_dir =
      vm.count("output_dir")
          ? vm["output_dir"].as<fs::path>()
          : (sut.parent_path() /
             (fs::path("fuzzon_").concat(sut.filename().c_str())) /
             time_now_str);

  auto env_flags = vm.count("env_flag")
                       ? vm["env_flag"].as<std::vector<std::string>>()
                       : std::vector<std::string>();
  auto corpus_base = vm["corpus_seeds"].as<fs::path>();
  auto sut_timeout = vm["single_test_timeout"].as<int>();
  auto test_timeout = vm["total_timeout"].as<int>();
  auto generate = vm["generate"].as<int>();
  auto mutate_d = vm["mutate_d"].as<int>();
  auto mutate_nd = vm["mutate_nd"].as<int>();
  auto total_testcases = vm["total_testcases"].as<int>();
  auto white_chars_preservation = vm["white_chars_preservation"].as<bool>();
  auto executor_mode = fuzzon::Executor::Mode(vm["executor_mode"].as<int>());

  auto verbose_level = verbosity_values.length();
  Logger::Get(output_dir.string(), verbose_level);

  //  std::string input_alphabet("abcd");
  //  fuzzon::Random::Get()->SetAlphabet(input_alphabet);
  fuzzon::Fuzzon crazy_fuzzer(output_dir.string(), sut.string(),
                              std::move(env_flags), sut_timeout, executor_mode,
                              test_timeout, total_testcases);

  //  std::vector<std::string> samples = {"a",     "ab",     "abc",    "abcd",
  //                                      "abcda", "abcdab", "abcdabc"};
  //  for (auto& sample : samples) {
  //    crazy_fuzzer.TestInput(sample);
  //  }
  //    std::vector<std::string> samples = {"0",
  //                                        "1 0",
  //                                        "1 1 1",
  //                                        "1 5 1 1 1 1 1",
  //                                        "1 21 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
  //                                        1 1 1 1 1 1",
  //                                        "2 3 1 2 3 21 1 1 1 1 1 1 1 1 1 1 1
  //                                        1 1 1 1 1 1 1 1 1 1",
  //                                        "2 21 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
  //                                        1 1 1 1 1 1 21 1 1 1 1 1 1 1 1 1 1 1
  //                                        "
  //                                        "1 1 1 1 1 1 1 1 1 1"};
  //  std::vector<std::string> samples = {"0", "1 0", "1 1 1", "1 5 1 1 1 1 1",
  //  "1 4 1 1 1 1"};
  //
  //  for (auto& sample : samples) {
  //    crazy_fuzzer.TestInput(sample);
  //  }

  //  const std::string alphabet("abcd");
  //  std::string test = "";
  //  crazy_fuzzer.TestInput(test);
  //  for (int i = 0; i < alphabet.size(); ++i) {
  //    for (int j = 0; j < alphabet.size(); ++j) {
  //      for (int k = 0; k < alphabet.size(); ++k) {
  //        for (int l = 0; l < alphabet.size(); ++l) {
  //          test = "";
  //          test += alphabet[i];
  //          test += alphabet[j];
  //          test += alphabet[k];
  //          test += alphabet[l];
  //          crazy_fuzzer.TestInput(test);
  //        }
  //      }
  //    }
  //  }

  //  for (auto i = 0; i < 5; ++i) {
  //    crazy_fuzzer.TestInput("abcd abcd abcd ");
  //  }

  crazy_fuzzer.ScanCorpus(corpus_base.string());
  crazy_fuzzer.Generation(input_format.string(), generate);
  crazy_fuzzer.MutationDeterministic(mutate_d, white_chars_preservation);
  crazy_fuzzer.MutationNonDeterministic(mutate_nd, white_chars_preservation);
  crazy_fuzzer.PrintStats();
  crazy_fuzzer.Dump();
  return 0;
}

#endif
