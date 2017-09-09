#ifndef FUZZON_EXECUTIONTRACKER_H_
#define FUZZON_EXECUTIONTRACKER_H_

#include "fuzzon_coverage.h"

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

namespace fuzzon {

class ExecutionTracker {
 public:
  ExecutionTracker(ExecutionTracker const&) = delete;
  void operator=(ExecutionTracker const&) = delete;

  enum WorkingMode { SUT, Monitor };

  static ExecutionTracker* Get(WorkingMode mode = WorkingMode::SUT) {
    static ExecutionTracker monitor_(mode);
    return &monitor_;
  }

  void Reset();

  const Coverage* GetCoverage() { return cov_; }

  void SetPCLimit(size_t value) { cov_->SetPCLimit(value); }
  void TracePC(uintptr_t PC) { cov_->TracePC(PC); }
  void TracePC(uint32_t idx, uintptr_t PC) { cov_->TracePC(idx, PC); }

 private:
  ExecutionTracker(WorkingMode mode);

  const std::string shared_memory_buffer_name = "fuzzon_cov";
  boost::interprocess::shared_memory_object shared_memory_;
  boost::interprocess::mapped_region region_;
  WorkingMode mode_;
  Coverage* cov_;
};

} /* namespace fuzzon */

#endif /* FUZZON_EXECUTIONTRACKER_H_ */
