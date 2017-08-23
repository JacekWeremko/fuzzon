
#include "fuzzon_corpus.h"

#include <set>
#include <boost/assert.hpp>

namespace fuzzon {

Corpus::Corpus() {
	// TODO Auto-generated constructor stub

}

Corpus::~Corpus() {
	// TODO Auto-generated destructor stub
}

bool Corpus::IsInteresting(ExecutionData am_i)
{
	// TODO: decision should be done based on overall coverage and new paths
	static std::set<std::error_code> error_codes;
	auto inserted = error_codes.insert(am_i.ec_);
	return inserted.second;
}

void Corpus::AddExecutionData(ExecutionData add_me_to_corpus)
{
	//TODO: optimize memory footprint
	execution_hisotry_.push_back(add_me_to_corpus);
}

TestCase Corpus::SelectFavorite()
{
	//TODO: top score based on low frequency paths
	BOOST_ASSERT(execution_hisotry_.size() > 0);
	return execution_hisotry_.back().input_;
}

} /* namespace fuzzon */
