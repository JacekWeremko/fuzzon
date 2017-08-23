

#include "utils/logger.h"
#include "fuzzon_executionmonitor.h"

#define ATTRIBUTE_INTERFACE
#define ATTRIBUTE_NO_SANITIZE_ALL


#ifdef __x86_64
#define ATTRIBUTE_TARGET_POPCNT __attribute__((target("popcnt")))
#else
#define ATTRIBUTE_TARGET_POPCNT
#endif



extern "C"
{

ATTRIBUTE_INTERFACE
ATTRIBUTE_NO_SANITIZE_ALL
void __sanitizer_cov_module_init(uint32_t *guards, uintptr_t npcs, const char *module_name)
{
  uintptr_t PC = reinterpret_cast<uintptr_t>(__builtin_return_address(0));
  Logger::Get()->info("__sanitizer_cov_module_init module_name: " + std::string(module_name));
  Logger::Get()->info("PC: " + std::to_string(PC));
  fuzzon::ExecutionMonitor::Get()->AddTrace(PC);
}

void __sanitizer_cov(uint32_t *Guard)
{
  uintptr_t PC = reinterpret_cast<uintptr_t>(__builtin_return_address(0));
  uint32_t Idx = *Guard;
  Logger::Get()->info("__sanitizer_cov PC: " + std::to_string(PC) + "    Idx: " + std::to_string(Idx));
  fuzzon::ExecutionMonitor::Get()->AddTrace(PC);
}

ATTRIBUTE_INTERFACE
ATTRIBUTE_NO_SANITIZE_ALL
void __sanitizer_cov_trace_pc_guard(uint32_t *Guard)
{
  uintptr_t PC = reinterpret_cast<uintptr_t>(__builtin_return_address(0));
  uint32_t Idx = *Guard;
  Logger::Get()->info("__sanitizer_cov_trace_pc_guard PC: " + std::to_string(PC) + "    Idx: " + std::to_string(Idx));
  fuzzon::ExecutionMonitor::Get()->AddTrace(PC);
}

ATTRIBUTE_INTERFACE
ATTRIBUTE_NO_SANITIZE_ALL
void __sanitizer_cov_trace_pc()
{
  uintptr_t PC = reinterpret_cast<uintptr_t>(__builtin_return_address(0));
  Logger::Get()->info("__sanitizer_cov_trace_pc PC: " + std::to_string(PC));
  fuzzon::ExecutionMonitor::Get()->AddTrace(PC);
}

ATTRIBUTE_INTERFACE
void __sanitizer_cov_trace_pc_guard_init(uint32_t *start, uint32_t *stop)
{
	Logger::Get()->info("__sanitizer_cov_trace_pc_guard_init start:" + std::to_string(*start) + " stop:"+ std::to_string(*stop));
	static uint64_t N;  // Counter for the guards.
	if (start == stop || *start) return;  // Initialize only once.
	printf("INIT: %p %p\n", start, stop);
	for (uint32_t *x = start; x < stop; x++)
		*x = ++N;  // Guards should start from 1.
//  fuzzer::TPC.HandleInit(Start, Stop);
}

ATTRIBUTE_INTERFACE
ATTRIBUTE_NO_SANITIZE_ALL
void __sanitizer_cov_trace_pc_indir(uintptr_t Callee) {
  uintptr_t PC = reinterpret_cast<uintptr_t>(__builtin_return_address(0));
  Logger::Get()->info("__sanitizer_cov_trace_pc_indir PC" + std::to_string(PC) + " Callee:" + std::to_string(Callee));
//  fuzzer::TPC.HandleCallerCallee(PC, Callee);
}

ATTRIBUTE_INTERFACE
ATTRIBUTE_NO_SANITIZE_ALL
ATTRIBUTE_TARGET_POPCNT
void __sanitizer_cov_trace_cmp8(uint64_t Arg1, uint64_t Arg2) {
//  uintptr_t PC = reinterpret_cast<uintptr_t>(__builtin_return_address(0));
//  Logger::Get()->info("__sanitizer_cov_trace_cmp8 {0:x} {1:x} {2:x}", PC, Arg1, Arg2);
//  fuzzer::TPC.HandleCmp(PC, Arg1, Arg2);
}

ATTRIBUTE_INTERFACE
ATTRIBUTE_NO_SANITIZE_ALL
ATTRIBUTE_TARGET_POPCNT
void __sanitizer_cov_trace_cmp4(uint32_t Arg1, uint32_t Arg2) {
//  uintptr_t PC = reinterpret_cast<uintptr_t>(__builtin_return_address(0));
//  Logger::Get()->info("__sanitizer_cov_trace_cmp4 {0:x} {1:x} {2:x}", PC, Arg1, Arg2);
//  fuzzer::TPC.HandleCmp(PC, Arg1, Arg2);
}

ATTRIBUTE_INTERFACE
ATTRIBUTE_NO_SANITIZE_ALL
ATTRIBUTE_TARGET_POPCNT
void __sanitizer_cov_trace_cmp2(uint16_t Arg1, uint16_t Arg2) {
//  uintptr_t PC = reinterpret_cast<uintptr_t>(__builtin_return_address(0));
//  Logger::Get()->info("__sanitizer_cov_trace_cmp2 {0:x} {1:x} {2:x}", PC, Arg1, Arg2);
//  fuzzer::TPC.HandleCmp(PC, Arg1, Arg2);
}

ATTRIBUTE_INTERFACE
ATTRIBUTE_NO_SANITIZE_ALL
ATTRIBUTE_TARGET_POPCNT
void __sanitizer_cov_trace_cmp1(uint8_t Arg1, uint8_t Arg2) {
//  uintptr_t PC = reinterpret_cast<uintptr_t>(__builtin_return_address(0));
//  Logger::Get()->info("__sanitizer_cov_trace_cmp1 {0:x} {1:x} {2:x}", PC, Arg1, Arg2);
//  fuzzer::TPC.HandleCmp(PC, Arg1, Arg2);
}

ATTRIBUTE_INTERFACE
ATTRIBUTE_NO_SANITIZE_ALL
ATTRIBUTE_TARGET_POPCNT
void __sanitizer_cov_trace_switch(uint64_t Val, uint64_t *Cases) {
//  uint64_t N = Cases[0];
//  uint64_t ValSizeInBits = Cases[1];
//  uint64_t *Vals = Cases + 2;
//  // Skip the most common and the most boring case.
//  if (Vals[N - 1]  < 256 && Val < 256)
//    return;
//  uintptr_t PC = reinterpret_cast<uintptr_t>(__builtin_return_address(0));
//  size_t i;
//  uint64_t Token = 0;
//  for (i = 0; i < N; i++) {
//    Token = Val ^ Vals[i];
//    if (Val < Vals[i])
//      break;
//  }
//
//  if (ValSizeInBits == 16)
//    fuzzer::TPC.HandleCmp(PC + i, static_cast<uint16_t>(Token), (uint16_t)(0));
//  else if (ValSizeInBits == 32)
//    fuzzer::TPC.HandleCmp(PC + i, static_cast<uint32_t>(Token), (uint32_t)(0));
//  else
//    fuzzer::TPC.HandleCmp(PC + i, Token, (uint64_t)(0));
//  Logger::Get()->info("__sanitizer_cov_trace_switch {0:x} {1:x}", PC, N);
}

ATTRIBUTE_INTERFACE
ATTRIBUTE_NO_SANITIZE_ALL
ATTRIBUTE_TARGET_POPCNT
void __sanitizer_cov_trace_div4(uint32_t Val) {
//  uintptr_t PC = reinterpret_cast<uintptr_t>(__builtin_return_address(0));
//  fuzzer::TPC.HandleCmp(PC, Val, (uint32_t)0);
//  Logger::Get()->info("__sanitizer_cov_trace_div4 {0:x} {1:x}", PC, Val);
}

ATTRIBUTE_INTERFACE
ATTRIBUTE_NO_SANITIZE_ALL
ATTRIBUTE_TARGET_POPCNT
void __sanitizer_cov_trace_div8(uint64_t Val) {
//  uintptr_t PC = reinterpret_cast<uintptr_t>(__builtin_return_address(0));
//  fuzzer::TPC.HandleCmp(PC, Val, (uint64_t)0);
//  Logger::Get()->info("__sanitizer_cov_trace_div8 {0:x} {1:x}", PC, Val);
}

ATTRIBUTE_INTERFACE
ATTRIBUTE_NO_SANITIZE_ALL
ATTRIBUTE_TARGET_POPCNT
void __sanitizer_cov_trace_gep(uintptr_t Idx) {
//  uintptr_t PC = reinterpret_cast<uintptr_t>(__builtin_return_address(0));
//  fuzzer::TPC.HandleCmp(PC, Idx, (uintptr_t)0);
//  Logger::Get()->info("__sanitizer_cov_trace_gep {0:x} {1:x}", PC, Idx);
}
}  // extern "C"

