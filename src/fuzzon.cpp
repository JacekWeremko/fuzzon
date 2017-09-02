#include "fuzzon.h"
#include "fuzzon_generator.h"
#include "fuzzon_mutator.h"
#include "fuzzon_executor.h"
#include "fuzzon_corpus.h"
#include "fuzzon_random.h"


namespace fuzzon
{

#define DIR_NAME_CRASH  "crash"
#define DIR_NAME_CORPUS "corpus"
#define DIR_NAME_PATHS  "paths"
#define DIR_NAME_TMP    "tmp"

Fuzzon::Fuzzon(std::string output_dir) : output_dir_(output_dir)
{
	Logger::Get(output_dir_);
	Logger::Get()->info("Base directory is " + output_dir_);


}

int Fuzzon::Run(std::string sut_path, std::string input_format, int iterations,
		int test_cases_to_generate, int test_cases_to_mutate,
		int statistics_print_interval)
{
	auto crash_dir = output_dir_ / boost::filesystem::path(DIR_NAME_CRASH);
	auto corpus_dir = output_dir_ / boost::filesystem::path(DIR_NAME_CORPUS);
	auto paths_dir = output_dir_ / boost::filesystem::path(DIR_NAME_PATHS);
	auto tmp_dir = output_dir_ / boost::filesystem::path(DIR_NAME_TMP);

	boost::filesystem::create_directories(crash_dir);
	boost::filesystem::create_directories(corpus_dir);
	boost::filesystem::create_directories(paths_dir);
	boost::filesystem::create_directories(tmp_dir);

	Logger::Get()->info("input_format : " + input_format);

	Corpus corpus;
	Executor execution_monitor(sut_path, 10);
	Generator test_cases_generator(input_format);

//	std::string input_alphabet(
//			"abcdefghijklmnopqrstuvwxyz"
//			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
//			"1234567890");
	//std::string input_alphabet("abcdefghijklmnopqrstuvwxyz");

	std::string input_alphabet("abcd");

	Random::Get()->SetAlphabet(input_alphabet);
	Mutator test_cases_mutator(input_alphabet);
	while(iterations--)
	{
		// generation phase
		{
			int test_cases_to_generate_counter = test_cases_to_generate;
			while(test_cases_to_generate_counter--)
			{
				auto new_test_case = test_cases_generator.generateNext();
				auto execution_data =  execution_monitor.ExecuteBlocking(new_test_case);
				if (corpus.IsInteresting(execution_data))
				{
					corpus.AddExecutionData(execution_data);
				}
			}
		}
		// mutation phase
		{
			auto test_cases_to_mutate_counter = test_cases_to_mutate;
			while(test_cases_to_mutate_counter--)
			{
				auto favorite = corpus.SelectFavorite();
				auto mutated = test_cases_mutator.Mutate(*favorite);
				auto execution_data = execution_monitor.ExecuteBlocking(mutated);
				if (corpus.IsInteresting(execution_data))
				{
					corpus.AddExecutionData(execution_data);
				}
			}
		}
		if (iterations % statistics_print_interval == 0)
		{
			Logger::Get()->info("Progress: \r\n" + corpus.GetStatistics().str());
		}
	}

	return 0;
}

}

