
#ifndef FUZZON_CORPUS_H_
#define FUZZON_CORPUS_H_

#include "fuzzon_executiondata.h"
#include "fuzzon_testcase.h"


#include <vector>

namespace fuzzon {

class Corpus {
public:
	Corpus();
	virtual ~Corpus();

	bool IsInteresting(const ExecutionData& am_i);

	void AddExecutionData(ExecutionData& add_me_to_corpus);

	TestCase SelectFavorite();

private:
//	Coverage totoal_coverage_;

//	std::vector<std::pair<size_t, ExecutionData>> execution_hisotry_;
	std::vector<ExecutionData> execution_hisotry_;
};

} /* namespace fuzzon */

#endif /* FUZZON_CORPUS_H_ */
