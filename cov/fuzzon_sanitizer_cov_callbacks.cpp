/*
 * Copyright [2017] Jacek Weremko
 */

#include "./fuzzon_executiontracker.h"

#include <string>
#include "./utils/logger.h"

#ifdef __x86_64
#define ATTRIBUTE_TARGET_POPCNT __attribute__((target("popcnt")))
#else
#define ATTRIBUTE_TARGET_POPCNT
#endif

extern "C" {

void __sanitizer_cov_module_init(uint32_t* guards,
                                 uintptr_t npcs,
                                 const char* module_name) {
  uintptr_t PC = reinterpret_cast<uintptr_t>(__builtin_return_address(0));
  LOG_TRACE("__sanitizer_cov_module_init");
  LOG_TRACE("  module_name: " + std::string(module_name));
  LOG_TRACE("  PC: " + std::to_string(PC));
  fuzzon::ExecutionTracker::Get()->TracePC(PC);
}

void __sanitizer_cov(uint32_t* Guard) {
  uintptr_t PC = reinterpret_cast<uintptr_t>(__builtin_return_address(0));
  uint32_t Idx = *Guard;
  LOG_TRACE("__sanitizer_cov");
  LOG_TRACE("  PC: " + std::to_string(PC) + "    Idx: " + std::to_string(Idx));
  fuzzon::ExecutionTracker::Get()->TracePC(PC);
}

void __sanitizer_cov_trace_pc_guard(uint32_t* Guard) {
  uintptr_t PC = reinterpret_cast<uintptr_t>(__builtin_return_address(0));
  uint32_t Idx = *Guard;
  LOG_TRACE("__sanitizer_cov_trace_pc_guard");
  LOG_TRACE("  PC: " + std::to_string(PC) + "    Idx: " + std::to_string(Idx));
  fuzzon::ExecutionTracker::Get()->TracePC(Idx, PC);
}

void __sanitizer_cov_trace_pc() {
  uintptr_t PC = reinterpret_cast<uintptr_t>(__builtin_return_address(0));
  LOG_TRACE("__sanitizer_cov_trace_pc");
  LOG_TRACE("  PC: " + std::to_string(PC));
  fuzzon::ExecutionTracker::Get()->TracePC(PC);
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
    //    LOG_TRACE("  guard : " + std::to_string(*P));
  }
  LOG_TRACE("  PC_total : " + std::to_string(pc_total));
  fuzzon::ExecutionTracker::Get()->SetPCLimit(pc_total);
}

void __sanitizer_cov_trace_pc_indir(uintptr_t Callee) {
  uintptr_t PC = reinterpret_cast<uintptr_t>(__builtin_return_address(0));
  LOG_TRACE("__sanitizer_cov_trace_pc_indir");
  LOG_TRACE("  PC: " + std::to_string(PC) +
            "    Callee: " + std::to_string(Callee));
}

ATTRIBUTE_TARGET_POPCNT
void __sanitizer_cov_trace_cmp8(uint64_t Arg1, uint64_t Arg2) {
  uintptr_t PC = reinterpret_cast<uintptr_t>(__builtin_return_address(0));
  LOG_TRACE("__sanitizer_cov_trace_cmp8");
  LOG_TRACE("  Arg1: " + std::to_string(Arg1) +
            "    Arg2: " + std::to_string(Arg2));
}

ATTRIBUTE_TARGET_POPCNT
void __sanitizer_cov_trace_cmp4(uint32_t Arg1, uint32_t Arg2) {
  LOG_TRACE("__sanitizer_cov_trace_cmp4");
  LOG_TRACE("  Arg1: " + std::to_string(Arg1) +
            "    Arg2: " + std::to_string(Arg2));
}

ATTRIBUTE_TARGET_POPCNT
void __sanitizer_cov_trace_cmp2(uint16_t Arg1, uint16_t Arg2) {
  LOG_TRACE("__sanitizer_cov_trace_cmp2");
  LOG_TRACE("  Arg1: " + std::to_string(Arg1) +
            "    Arg2: " + std::to_string(Arg2));
}

ATTRIBUTE_TARGET_POPCNT
void __sanitizer_cov_trace_cmp1(uint8_t Arg1, uint8_t Arg2) {
  LOG_TRACE("__sanitizer_cov_trace_cmp1");
  LOG_TRACE("  Arg1: " + std::to_string(Arg1) +
            "    Arg2: " + std::to_string(Arg2));
}

ATTRIBUTE_TARGET_POPCNT
void __sanitizer_cov_trace_switch(uint64_t Val, uint64_t* Cases) {
  LOG_TRACE("__sanitizer_cov_trace_switch");
  LOG_TRACE("  Val: " + std::to_string(Val));
  LOG_TRACE("  Cases[0]: " + std::to_string(Cases[0]));
  LOG_TRACE("  Cases[1]: " + std::to_string(Cases[1]));
}

ATTRIBUTE_TARGET_POPCNT
void __sanitizer_cov_trace_div4(uint32_t Val) {
  LOG_TRACE("__sanitizer_cov_trace_div4");
  LOG_TRACE("  Val: " + std::to_string(Val));
}

ATTRIBUTE_TARGET_POPCNT
void __sanitizer_cov_trace_div8(uint64_t Val) {
  LOG_TRACE("__sanitizer_cov_trace_div8");
  LOG_TRACE("  Val: " + std::to_string(Val));
}

ATTRIBUTE_TARGET_POPCNT
void __sanitizer_cov_trace_gep(uintptr_t Idx) {
  LOG_TRACE("__sanitizer_cov_trace_gep");
  LOG_TRACE("  Idx: " + std::to_string(Idx));
}

}  // extern "C"
