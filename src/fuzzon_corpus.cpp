
#include "fuzzon_corpus.h"

#include <set>
#include <limits>

#include <boost/assert.hpp>

namespace fuzzon {

Corpus::Corpus() {
	// TODO Auto-generated constructor stub

}

Corpus::~Corpus() {
	// TODO Auto-generated destructor stub
}

bool Corpus::IsInteresting(const ExecutionData& am_i)
{
	// TODO: decision should be done based on overall coverage and new paths
//	static std::set<std::error_code> error_codes;
//	auto inserted = error_codes.insert(am_i.ec_);
//	return inserted.second;

	//TODO: calculate coverage (path) hash in order to improve performance
	for(auto& current : execution_hisotry_)
	{
//		if (current.second.coverage_.pc_flow_hash == am_i.coverage_.pc_flow_hash)
		if (current.coverage_ == am_i.coverage_)
		{
			current.similar_execution_coutner_++;
			return false;
		}
	}
	return true;
}

void Corpus::AddExecutionData(ExecutionData& add_me_to_corpus)
{
	//TODO: optimize memory footprint
	execution_hisotry_.push_back(add_me_to_corpus);
}

TestCase Corpus::SelectFavorite()
{
	BOOST_ASSERT(execution_hisotry_.size() > 0);

	// FIXME: ...
	std::vector<ExecutionData*> execution_hisotry_copy;
	for(auto& current : execution_hisotry_)
	{
		execution_hisotry_copy.push_back(&current);
	}

	std::vector<ExecutionData*> lowest_usage_data;
	// find min s(i) - test cases used as mutation base
	{
		auto lowest_usage_count = std::numeric_limits<size_t>::max();
		for(auto& current : execution_hisotry_copy)
		{
			if (current->mutation_usage_count_ < lowest_usage_count)
			{
				lowest_usage_data.clear();
				lowest_usage_count = current->mutation_usage_count_;
				lowest_usage_data.push_back(current);
			}
			else if (current->mutation_usage_count_ == lowest_usage_count)
			{
				lowest_usage_data.push_back(current);
			}
		}
	}


	std::vector<ExecutionData*> lowest_similar_execution_data;
	// find min f(i) - lowest frequency path
	{
		auto lowest_similar_execution_coutner = std::numeric_limits<size_t>::max();
		for (auto& current : lowest_usage_data)
		{
			if (current->similar_execution_coutner_ < lowest_similar_execution_coutner)
			{
				lowest_similar_execution_data.clear();
				lowest_similar_execution_coutner = current->similar_execution_coutner_;
				lowest_similar_execution_data.push_back(current);
			}
			else if (current->similar_execution_coutner_ == lowest_similar_execution_coutner)
			{
				lowest_similar_execution_data.push_back(current);
			}

		}
	}

	ExecutionData* result = nullptr;
	// find min t(i) time and size
	{
		auto lowest_time_and_size = std::numeric_limits<double>::max();
		for (auto& current : lowest_similar_execution_data)
		{
			double current_time_and_size = current->execution_time_ * current->input_.length();
			if (current_time_and_size < lowest_time_and_size)
			{
				result = current;
			}

		}
	}

	return result->input_;
}

} /* namespace fuzzon */
