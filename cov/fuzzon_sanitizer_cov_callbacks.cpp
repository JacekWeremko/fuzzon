/*
 * Copyright [2017] Jacek Weremko
 */

#include "./fuzzon_executiontracker.h"

#include <string>
#include "./utils/logger.h"

extern "C" {

void __sanitizer_cov_module_init(uint32_t* guards,
                                 uintptr_t npcs,
                                 const char* module_name) {
  uintptr_t pc = reinterpret_cast<uintptr_t>(__builtin_return_address(0));
  LOG_TRACE("__sanitizer_cov_module_init");
  LOG_TRACE("  module_name: " + std::string(module_name));
  LOG_TRACE("  pc: " + std::to_string(pc));
  fuzzon::ExecutionTracker::Get()->TracePC(pc);
}

void __sanitizer_cov(uint32_t* Guard) {
  uintptr_t pc = reinterpret_cast<uintptr_t>(__builtin_return_address(0));
  uint32_t idx = *Guard;
  LOG_TRACE("__sanitizer_cov");
  LOG_TRACE("  pc: " + std::to_string(pc) + "    idx: " + std::to_string(idx));
  fuzzon::ExecutionTracker::Get()->TracePC(pc);
}

void __sanitizer_cov_trace_pc_guard(uint32_t* Guard) {
  uintptr_t pc = reinterpret_cast<uintptr_t>(__builtin_return_address(0));
  uint32_t idx = *Guard;
  LOG_TRACE("__sanitizer_cov_trace_pc_guard");
  LOG_TRACE("  pc: " + std::to_string(pc) + "    idx: " + std::to_string(idx));
  fuzzon::ExecutionTracker::Get()->TracePC(idx, pc);
}

void __sanitizer_cov_trace_pc() {
  uintptr_t pc = reinterpret_cast<uintptr_t>(__builtin_return_address(0));
  LOG_TRACE("__sanitizer_cov_trace_pc");
  LOG_TRACE("  pc: " + std::to_string(pc));
  fuzzon::ExecutionTracker::Get()->TracePC(pc);
}

void __sanitizer_cov_trace_pc_guard_init(uint32_t* start, uint32_t* stop) {
  LOG_TRACE("__sanitizer_cov_trace_pc_guard_init");
  LOG_TRACE("  start: " + std::to_string(*start));
  LOG_TRACE("  stop : " + std::to_string(*stop));

  if (start == stop || *start) {
    return;
  }
  int pc_total = 0;
  for (uint32_t* P = start; P < stop; P++) {
    pc_total++;
    *P = pc_total;
    LOG_TRACE("  guard : " + std::to_string(*P));
  }
  LOG_TRACE("  pc_total : " + std::to_string(pc_total));
  fuzzon::ExecutionTracker::Get()->SetPCLimit(pc_total);
}

void __sanitizer_cov_trace_pc_indir(uintptr_t callee) {
  uintptr_t pc = reinterpret_cast<uintptr_t>(__builtin_return_address(0));
  LOG_TRACE("__sanitizer_cov_trace_pc_indir");
  LOG_TRACE("  pc: " + std::to_string(pc));
  LOG_TRACE("  callee: " + std::to_string(callee));
}

void __sanitizer_cov_trace_cmp8(uint64_t arg1, uint64_t arg2) {
  uintptr_t pc = reinterpret_cast<uintptr_t>(__builtin_return_address(0));
  LOG_TRACE("__sanitizer_cov_trace_cmp8");
  LOG_TRACE("  arg1: " + std::to_string(arg1));
  LOG_TRACE("  arg2: " + std::to_string(arg1));
  fuzzon::ExecutionTracker::Get()->TraceCmp(pc, arg1, arg2);
}

void __sanitizer_cov_trace_cmp4(uint32_t arg1, uint32_t arg2) {
  uintptr_t pc = reinterpret_cast<uintptr_t>(__builtin_return_address(0));
  LOG_TRACE("__sanitizer_cov_trace_cmp4");
  LOG_TRACE("  arg1: " + std::to_string(arg1));
  LOG_TRACE("  arg2: " + std::to_string(arg1));
  fuzzon::ExecutionTracker::Get()->TraceCmp(pc, arg1, arg2);
}

void __sanitizer_cov_trace_cmp2(uint16_t arg1, uint16_t arg2) {
  uintptr_t pc = reinterpret_cast<uintptr_t>(__builtin_return_address(0));
  LOG_TRACE("__sanitizer_cov_trace_cmp2");
  LOG_TRACE("  arg1: " + std::to_string(arg1));
  LOG_TRACE("  arg2: " + std::to_string(arg1));
  fuzzon::ExecutionTracker::Get()->TraceCmp(pc, arg1, arg2);
}

void __sanitizer_cov_trace_cmp1(uint8_t arg1, uint8_t arg2) {
  uintptr_t pc = reinterpret_cast<uintptr_t>(__builtin_return_address(0));
  LOG_TRACE("__sanitizer_cov_trace_cmp1");
  LOG_TRACE("  arg1: " + std::to_string(arg1));
  LOG_TRACE("  arg2: " + std::to_string(arg1));
  fuzzon::ExecutionTracker::Get()->TraceCmp(pc, arg1, arg2);
}

void __sanitizer_cov_trace_switch(uint64_t value, uint64_t* cases) {
  uintptr_t pc = reinterpret_cast<uintptr_t>(__builtin_return_address(0));
  LOG_TRACE("__sanitizer_cov_trace_switch");
  LOG_TRACE("  value: " + std::to_string(value));
  LOG_TRACE("  cases[0]: " + std::to_string(cases[0]));
  LOG_TRACE("  cases[1]: " + std::to_string(cases[1]));
}

void __sanitizer_cov_trace_div4(uint32_t value) {
  uintptr_t pc = reinterpret_cast<uintptr_t>(__builtin_return_address(0));
  LOG_TRACE("__sanitizer_cov_trace_div4");
  LOG_TRACE("  value: " + std::to_string(value));
  fuzzon::ExecutionTracker::Get()->TraceDiv(pc, value);
}

void __sanitizer_cov_trace_div8(uint64_t value) {
  uintptr_t pc = reinterpret_cast<uintptr_t>(__builtin_return_address(0));
  LOG_TRACE("__sanitizer_cov_trace_div8");
  LOG_TRACE("  value: " + std::to_string(value));
  fuzzon::ExecutionTracker::Get()->TraceDiv(pc, value);
}

void __sanitizer_cov_trace_gep(uintptr_t idx) {
  uintptr_t pc = reinterpret_cast<uintptr_t>(__builtin_return_address(0));
  LOG_TRACE("__sanitizer_cov_trace_gep");
  LOG_TRACE("  idx: " + std::to_string(idx));
  fuzzon::ExecutionTracker::Get()->TraceGep(pc, idx);
}

}  // extern "C"
