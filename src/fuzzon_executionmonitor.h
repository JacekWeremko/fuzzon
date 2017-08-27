
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
		cov_ = Coverage(Coverage::Flow);
	}

	const Coverage* GetCoverage()
	{
		return &cov_;
	}


	void TracePC(uintptr_t PC)
	{
		cov_.TracePC(PC);
	}

private:
	ExecutionMonitor();

	Coverage cov_;
};

} /* namespace fuzzon */

#endif /* FUZZON_EXECUTIONMONITOR_H_ */
