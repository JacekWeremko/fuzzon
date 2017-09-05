#ifndef FUZZON_H_
#define FUZZON_H_

#include "fuzzon_executor.h"
#include "fuzzon_corpus.h"

#include <boost/filesystem.hpp>

namespace fuzzon
{

class Fuzzon
{
public:
	Fuzzon(std::string output_dir, std::string sut_path, int sut_runtime_timeout);

	void GenerationPhase(std::string input_format, int test_cases_to_generate);
	void MutationPhaseDeterministic();
	void MutationPhaseNonDeterministic(int test_cases_to_mutate = -1);

private:
	std::string output_dir_;
	Corpus corpus_;
	Executor execution_monitor_;
};

}

#endif /* FUZZON_H_ */
