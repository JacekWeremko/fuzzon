#ifndef FUZZON_EXECUTIONTRACKER_H_
#define FUZZON_EXECUTIONTRACKER_H_

#include "fuzzon_coverage.h"

#include <string>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

namespace fuzzon {

class ExecutionTracker {
 public:
  enum WorkingMode { SUT, Monitor };

  static ExecutionTracker* Get(WorkingMode mode = WorkingMode::SUT,
                               Coverage::TrackMode track_mode = Coverage::Raw) {
    static ExecutionTracker monitor_(mode, track_mode);
    return &monitor_;
  }

  ExecutionTracker(ExecutionTracker const&) = delete;
  void operator=(ExecutionTracker const&) = delete;

  void Reset();
  const Coverage* GetCoverage() { return cov_; }

  void SetPCLimit(size_t value) { cov_->SetPCGuardsCount(value); }
  void TracePC(uintptr_t pc) { cov_->TracePC(pc); }
  void TracePC(uint32_t guard_idx, uintptr_t pc) { cov_->TracePC(guard_idx); }

  template <typename valueT>
  void TraceCmp(uintptr_t pc, valueT arg1, valueT arg2) {
    //    cov_->TraceData(pc, arg1 ^ arg2);
    cov_->TraceCmp(pc, arg1, arg2);
  }

  template <typename valueT>
  void TraceDiv(uintptr_t pc, valueT value) {
    // cov_->TraceData(pc, value);
  }

  void TraceGep(uintptr_t pc, uint32_t idx) {
    // cov_->TraceData(pc, idx);
  }

 private:
  explicit ExecutionTracker(WorkingMode mode, Coverage::TrackMode track_mode);

  const std::string shared_memory_buffer_name = "fuzzon_cov";
  boost::interprocess::shared_memory_object shared_memory_;
  boost::interprocess::mapped_region region_;
  WorkingMode mode_;
  Coverage* cov_;
};

} /* namespace fuzzon */

#endif /* FUZZON_EXECUTIONTRACKER_H_ */
