
#include "fuzzon_coverage.h"
#include "utils/logger.h"

#include <sstream>
#include <cmath>

#define ARRAY_ELEMENTS(array) (sizeof(array) / sizeof(array[0]))

namespace fuzzon {

Coverage::Coverage(TrackMode mode) : lact_pc_(0), mode_(mode)
{
	std::fill_n(pc_flow_, ARRAY_ELEMENTS(pc_flow_), 0);
}

Coverage::~Coverage()
{
	// TODO Auto-generated destructor stub
}

void Coverage::Compress(CompreseMode comprese_mode)
{
	if (comprese_mode == CompreseMode::Log2)
	{
		for (size_t idx=0; idx<ARRAY_ELEMENTS(pc_flow_); idx++)
		{
			if (pc_flow_[idx] == 0)
			{
				continue;
			}
			pc_flow_[idx] = std::log2(pc_flow_[idx]) + 1;
		}
	}
	return;
}

bool Coverage::operator==(const Coverage& compare_with_me) const
{
	for (size_t idx=0; idx<ARRAY_ELEMENTS(pc_flow_); idx++)
	{
		if (pc_flow_[idx] != compare_with_me.pc_flow_[idx])
		{
			return false;
		}
	}
	return true;
}

bool Coverage::operator!=(const Coverage& compare_with_me) const
{
	return !(*this == compare_with_me);
}

void Coverage::TracePC(uintptr_t PC)
{
	uintptr_t current = 0;
	if (mode_ == TrackMode::Raw)
	{
		current = PC;
	}
	else
	{
		current = lact_pc_ == 0 ? PC : (PC ^ lact_pc_);
	}

	pc_trace_.push_back(current);

	auto it = pc_counter_.find(current);
	pc_counter_[current] = it == pc_counter_.end() ? 1 : it->second + 1;

	pc_flow_[current % ARRAY_ELEMENTS(pc_flow_)]++;

	lact_pc_ = PC >> 1;
}

void Coverage::PrintTrace() const
{
//	Logger::Get()->info("pc_traces	: " + PrintPCTraces());
//	Logger::Get()->info("pc_counters	: " + PrintPCConuters());
	Logger::Get()->info("pc_flow	: " + PrintPCFlow());
}

std::string Coverage::PrintPCTraces() const
{
	std::stringstream output;
	for (const auto& elem : pc_trace_)
	{
		output << std::hex << elem << " ";
	}
	return output.str();
}

std::string Coverage::PrintPCConuters() const
{
	std::stringstream output;
	for (const auto& elem : pc_counter_)
	{
		output << std::hex << elem.first << ":" << elem.second << " ";
	}
	return output.str();
}

std::string Coverage::PrintPCFlow() const
{
	std::stringstream output;
	for (size_t i=0; i< ARRAY_ELEMENTS(pc_flow_); i++)
	{
		if (pc_flow_[i] != 0)
		{
			output << i << ":" << std::hex << pc_flow_[i] << " ";
		}
	}
	return output.str();
}

} /* namespace fuzzon */
