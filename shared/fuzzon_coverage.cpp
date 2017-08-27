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
	pc_flow_[current % ARRAY_ELEMENTS(pc_flow_)]++;
	lact_pc_ = PC >> 1;
}

void Coverage::PrintTrace() const
{
	std::stringstream output;
	for (size_t i=0; i< ARRAY_ELEMENTS(pc_flow_); i++)
	{
		if (pc_flow_[i] != 0)
		{
			output << i << ":" << std::hex << pc_flow_[i] << " ";
		}
	}
	Logger::Get()->info("pc_flow	: " + output.str());
	return;
}

} /* namespace fuzzon */
