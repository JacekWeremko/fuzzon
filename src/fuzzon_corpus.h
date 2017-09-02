#ifndef FUZZON_CORPUS_H_
#define FUZZON_CORPUS_H_

#include "fuzzon_executiondata.h"
#include "fuzzon_testcase.h"
#include <vector>
#include <sstream>


namespace fuzzon {

class Corpus {
public:
	bool IsInteresting(const ExecutionData& am_i);

	void AddExecutionData(ExecutionData& add_me_to_corpus);

	TestCase* SelectFavorite();

	std::stringstream GetStatistics();

private:
	std::vector<ExecutionData> execution_history_;
};

} /* namespace fuzzon */

#endif /* FUZZON_CORPUS_H_ */
