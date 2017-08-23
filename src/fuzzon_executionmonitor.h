/*
 * fuzzon_executionmonitor.h
 *
 *  Created on: Aug 23, 2017
 *      Author: dablju
 */

#ifndef FUZZON_EXECUTIONMONITOR_H_
#define FUZZON_EXECUTIONMONITOR_H_

#include "fuzzon_coverage.h"
#include "utils/logger.h"

#include <vector>
#include <map>
#include <sstream>
#include <iostream>
#include <memory>

namespace fuzzon {

class ExecutionMonitor {
public:
	ExecutionMonitor(ExecutionMonitor const&) = delete;
	void operator=(ExecutionMonitor const&) = delete;

	virtual ~ExecutionMonitor();

	static ExecutionMonitor* Get()
	{
		static ExecutionMonitor monitor_;
		return &monitor_;
	}

	void Reset()
	{
		pc_trace_.clear();
		pc_counter_.clear();
	}

	Coverage GetCoverage()
	{
		//TODO: implement
		Coverage cov;
		return cov;
	}


	void AddTrace(uintptr_t PC) {
		pc_trace_.push_back(PC);

		auto it = pc_counter_.find(PC);
		if (it == pc_counter_.end()) {
			pc_counter_[PC] = 1;
		} else {
			pc_counter_[PC] = it->second + 1;
		}
	}


	void PrintTrace() {
		auto traces = PrintPCTraces();
		auto counters = PrintPCConuters();

//		Logger::Get()->info("\npc_counters	: {0:s}\npc_trace	: {1:s}", counters, traces);
		Logger::Get()->info("pc_traces	: " + std::string(traces));
	}

	std::string PrintPCConuters() {
		std::string pc_counters = "";
		for (const auto& elem : pc_counter_) {
			std::stringstream stream;
			stream << std::hex << elem.first << ":" << elem.second;
			pc_counters += stream.str() + " ";
		}
		return pc_counters;

	}

	std::string PrintPCTraces() {
		std::string pc_traces = "";
		for (const auto& elem : pc_trace_) {
			std::stringstream stream;
			stream << std::hex << elem;
			pc_traces += stream.str() + " ";
		}
		return pc_traces;
	}

private:
	ExecutionMonitor();

	std::map<uintptr_t, unsigned int> pc_counter_;
	std::vector<uintptr_t> pc_trace_;
};

} /* namespace fuzzon */

#endif /* FUZZON_EXECUTIONMONITOR_H_ */
