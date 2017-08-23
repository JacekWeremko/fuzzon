
#include "fuzzon.h"
#include "fuzzon_generator.h"
#include "fuzzon_mutator.h"
#include "fuzzon_executor.h"
#include "fuzzon_corpus.h"

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

int Fuzzon::Run(std::string sut_path, std::string input_format)
{
	auto crash_dir  = output_dir_ / boost::filesystem::path(DIR_NAME_CRASH);
	auto corpus_dir = output_dir_ / boost::filesystem::path(DIR_NAME_CORPUS);
	auto paths_dir  = output_dir_ / boost::filesystem::path(DIR_NAME_PATHS);
	auto tmp_dir    = output_dir_ / boost::filesystem::path(DIR_NAME_TMP);

	boost::filesystem::create_directories(crash_dir);
	boost::filesystem::create_directories(corpus_dir);
	boost::filesystem::create_directories(paths_dir);
	boost::filesystem::create_directories(tmp_dir);

	Executor execution_monitor(sut_path);
	Corpus corpus;

	auto iterations = 2;
	Generator test_cases_generator(input_format, tmp_dir.string());
	Mutator test_cases_mutator(false);
	while(iterations--)
	{
		// generation phase
		{
			int test_cases_to_generate = 1;
			while(test_cases_to_generate--)
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
			auto test_cases_to_mutate = 2;
			while(test_cases_to_mutate--)
			{
				TestCase favorite = corpus.SelectFavorite();
				TestCase mutated = test_cases_mutator.Mutate(favorite);
				auto execution_data = execution_monitor.ExecuteBlocking(mutated);
				if (corpus.IsInteresting(execution_data))
				{
					corpus.AddExecutionData(execution_data);
				}
			}
		}
	}


	return 0;
}

}

