/*
* fuzzon_corpus.h
 *
 *  Created on: Aug 22, 2017
 *      Author: dablju
 */

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

	bool IsInteresting(ExecutionData am_i);

	void AddExecutionData(ExecutionData add_me_to_corpus);

	TestCase SelectFavorite();

private:
	std::vector<ExecutionData> execution_hisotry_;
};

} /* namespace fuzzon */

#endif /* FUZZON_CORPUS_H_ */
