#include "fuzzon_corpus.h"
#include "utils/logger.h"

#include <set>
#include <limits>
#include <algorithm>
#include <numeric>
#include <chrono>

#include <boost/assert.hpp>

namespace fuzzon {

bool Corpus::IsInteresting(const ExecutionData& am_i)
{
	//TODO: calculate coverage (path) hash in order to improve performance
	for(auto& current : execution_history_)
	{
//		if (current.second.coverage_.pc_flow_hash == am_i.coverage_.pc_flow_hash)
		if (current.coverage == am_i.coverage)
		{
			current.coverage_coutner_++;
			return false;
		}
	}
	return true;
}

void Corpus::AddExecutionData(ExecutionData& add_me_to_corpus)
{
	Logger::Get()->info("Adding new test case to corpus: " + add_me_to_corpus.input.string());
	//TODO: optimize memory footprint
	execution_history_.push_back(add_me_to_corpus);
}

TestCase* Corpus::SelectFavorite()
{
	BOOST_ASSERT(execution_history_.size() > 0);

	// FIXME: ...
	std::vector<ExecutionData*> execution_hisotry_copy;
	for(auto& current : execution_history_)
	{
		execution_hisotry_copy.push_back(&current);
	}

	std::vector<ExecutionData*> lowest_usage_data;
	// find min s(i) - test cases used as mutation base
	{
		auto lowest_usage_count = std::numeric_limits<size_t>::max();
		for(auto& current : execution_hisotry_copy)
		{
			if (current->mutation_counter_ < lowest_usage_count)
			{
				lowest_usage_data.clear();
				lowest_usage_count = current->mutation_counter_;
				lowest_usage_data.push_back(current);
			}
			else if (current->mutation_counter_ == lowest_usage_count)
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
			if (current->coverage_coutner_ < lowest_similar_execution_coutner)
			{
				lowest_similar_execution_data.clear();
				lowest_similar_execution_coutner = current->coverage_coutner_;
				lowest_similar_execution_data.push_back(current);
			}
			else if (current->coverage_coutner_ == lowest_similar_execution_coutner)
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
			double current_time_and_size = current->execution_time.count() * current->input.length();
			if (current_time_and_size < lowest_time_and_size)
			{
				result = current;
			}

		}
	}

	return &result->input;
}

std::stringstream Corpus::GetStatistics()
{
	std::stringstream stats;
	stats << "Corpus size : " << std::to_string(execution_history_.size()) << std::endl;

	stats << "Tested cases: " << std::accumulate(execution_history_.begin(), execution_history_.end(), 0,
			[](int execution_counter, ExecutionData& arg) { return execution_counter + arg.coverage_coutner_; }) << std::endl;

	stats << "Test case max mutation count: " << std::max_element(execution_history_.begin(), execution_history_.end(),
			[](ExecutionData& arg1, ExecutionData& arg2) { return arg1.mutation_counter_ < arg2.mutation_counter_; })->mutation_counter_ << std::endl;
	stats << "Test case min mutation count: " << std::max_element(execution_history_.begin(), execution_history_.end(),
			[](ExecutionData& arg1, ExecutionData& arg2) { return arg1.mutation_counter_ > arg2.mutation_counter_; })->mutation_counter_ << std::endl;

	stats << "Test case max similar executions count: " << std::max_element(execution_history_.begin(), execution_history_.end(),
			[](ExecutionData& arg1, ExecutionData& arg2) { return arg1.coverage_coutner_ < arg2.coverage_coutner_; })->coverage_coutner_ << std::endl;
	stats << "Test case min similar executions count: " << std::max_element(execution_history_.begin(), execution_history_.end(),
			[](ExecutionData& arg1, ExecutionData& arg2) { return arg1.coverage_coutner_ > arg2.coverage_coutner_; })->coverage_coutner_ << std::endl;


	stats << "Test case max execution time[ms]: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::max_element(execution_history_.begin(), execution_history_.end(),
			[](ExecutionData& arg1, ExecutionData& arg2) { return arg1.execution_time < arg2.execution_time; })->execution_time).count() << std::endl;
	stats << "Test case min execution time[ms]: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::max_element(execution_history_.begin(), execution_history_.end(),
			[](ExecutionData& arg1, ExecutionData& arg2) { return arg1.execution_time > arg2.execution_time; })->execution_time).count() << std::endl;

	stats << "Gracefully finished: " << std::count_if(execution_history_.begin(), execution_history_.end(),
			[](ExecutionData& arg) { return arg.gracefully_finished == true; })	<< std::endl;

	stats << "Non zero error code: " << std::count_if(execution_history_.begin(), execution_history_.end(),
			[](ExecutionData& arg) { return arg.error_code.value() != 0; })	<< std::endl;

	stats << "Non zero return code: " << std::count_if(execution_history_.begin(), execution_history_.end(),
			[](ExecutionData& arg) { return arg.exit_code != 0; })	<< std::endl;

	stats << "Faulty test cases: " << std::count_if(execution_history_.begin(), execution_history_.end(),
			[](ExecutionData& arg) { return arg.gracefully_finished == false && arg.error_code.value() != 0; })	<< std::endl;


	return stats;
}


} /* namespace fuzzon */
