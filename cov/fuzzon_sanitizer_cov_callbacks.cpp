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

void __sanitizer_cov_module_init(uint32_t* guards, uintptr_t npcs, const char* module_name) {
  uintptr_t PC = reinterpret_cast<uintptr_t>(__builtin_return_address(0));
  Logger::Get()->trace("__sanitizer_cov_module_init");
  Logger::Get()->trace("  module_name: " + std::string(module_name));
  Logger::Get()->trace("  PC: " + std::to_string(PC));
  fuzzon::ExecutionTracker::Get()->TracePC(PC);
}

void __sanitizer_cov(uint32_t* Guard) {
  uintptr_t PC = reinterpret_cast<uintptr_t>(__builtin_return_address(0));
  uint32_t Idx = *Guard;
  Logger::Get()->trace("__sanitizer_cov");
  Logger::Get()->trace("  PC: " + std::to_string(PC) + "    Idx: " + std::to_string(Idx));
  fuzzon::ExecutionTracker::Get()->TracePC(PC);
}

void __sanitizer_cov_trace_pc_guard(uint32_t* Guard) {
  uintptr_t PC = reinterpret_cast<uintptr_t>(__builtin_return_address(0));
  uint32_t Idx = *Guard;
  Logger::Get()->trace("__sanitizer_cov_trace_pc_guard");
  Logger::Get()->trace("  PC: " + std::to_string(PC) + "    Idx: " + std::to_string(Idx));
  fuzzon::ExecutionTracker::Get()->TracePC(Idx, PC);
}

void __sanitizer_cov_trace_pc() {
  uintptr_t PC = reinterpret_cast<uintptr_t>(__builtin_return_address(0));
  Logger::Get()->trace("__sanitizer_cov_trace_pc");
  Logger::Get()->trace("  PC: " + std::to_string(PC));
  fuzzon::ExecutionTracker::Get()->TracePC(PC);
}

void __sanitizer_cov_trace_pc_guard_init(uint32_t* start, uint32_t* stop) {
  Logger::Get()->trace("__sanitizer_cov_trace_pc_guard_init");
  Logger::Get()->trace("  start: " + std::to_string(*start));
  Logger::Get()->trace("  stop : " + std::to_string(*stop));

  if (start == stop || *start) {
    return;
  }
  int pc_total = 0;
  for (uint32_t* P = start; P < stop; P++) {
    pc_total++;
    *P = pc_total;
    //    Logger::Get()->trace("  guard : " + std::to_string(*P));
  }
  Logger::Get()->trace("  PC_total : " + std::to_string(pc_total));
  fuzzon::ExecutionTracker::Get()->SetPCLimit(pc_total);
}

void __sanitizer_cov_trace_pc_indir(uintptr_t Callee) {
  uintptr_t PC = reinterpret_cast<uintptr_t>(__builtin_return_address(0));
  Logger::Get()->trace("__sanitizer_cov_trace_pc_indir");
  Logger::Get()->trace("  PC: " + std::to_string(PC) + "    Callee: " + std::to_string(Callee));
}

ATTRIBUTE_TARGET_POPCNT
void __sanitizer_cov_trace_cmp8(uint64_t Arg1, uint64_t Arg2) {
  uintptr_t PC = reinterpret_cast<uintptr_t>(__builtin_return_address(0));
  Logger::Get()->trace("__sanitizer_cov_trace_cmp8");
  Logger::Get()->trace("  Arg1: " + std::to_string(Arg1) + "    Arg2: " + std::to_string(Arg2));
}

ATTRIBUTE_TARGET_POPCNT
void __sanitizer_cov_trace_cmp4(uint32_t Arg1, uint32_t Arg2) {
  Logger::Get()->trace("__sanitizer_cov_trace_cmp4");
  Logger::Get()->trace("  Arg1: " + std::to_string(Arg1) + "    Arg2: " + std::to_string(Arg2));
}

ATTRIBUTE_TARGET_POPCNT
void __sanitizer_cov_trace_cmp2(uint16_t Arg1, uint16_t Arg2) {
  Logger::Get()->trace("__sanitizer_cov_trace_cmp2");
  Logger::Get()->trace("  Arg1: " + std::to_string(Arg1) + "    Arg2: " + std::to_string(Arg2));
}

ATTRIBUTE_TARGET_POPCNT
void __sanitizer_cov_trace_cmp1(uint8_t Arg1, uint8_t Arg2) {
  Logger::Get()->trace("__sanitizer_cov_trace_cmp1");
  Logger::Get()->trace("  Arg1: " + std::to_string(Arg1) + "    Arg2: " + std::to_string(Arg2));
}

ATTRIBUTE_TARGET_POPCNT
void __sanitizer_cov_trace_switch(uint64_t Val, uint64_t* Cases) {
  Logger::Get()->trace("__sanitizer_cov_trace_switch");
  Logger::Get()->trace("  Val: " + std::to_string(Val));
  Logger::Get()->trace("  Cases[0]: " + std::to_string(Cases[0]));
  Logger::Get()->trace("  Cases[1]: " + std::to_string(Cases[1]));
}

ATTRIBUTE_TARGET_POPCNT
void __sanitizer_cov_trace_div4(uint32_t Val) {
  Logger::Get()->trace("__sanitizer_cov_trace_div4");
  Logger::Get()->trace("  Val: " + std::to_string(Val));
}

ATTRIBUTE_TARGET_POPCNT
void __sanitizer_cov_trace_div8(uint64_t Val) {
  Logger::Get()->trace("__sanitizer_cov_trace_div8");
  Logger::Get()->trace("  Val: " + std::to_string(Val));
}

ATTRIBUTE_TARGET_POPCNT
void __sanitizer_cov_trace_gep(uintptr_t Idx) {
  Logger::Get()->trace("__sanitizer_cov_trace_gep");
  Logger::Get()->trace("  Idx: " + std::to_string(Idx));
}

}  // extern "C"
