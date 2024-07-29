#pragma once

// This is a Playdate version, so we force 32-bit

#include <stdint.h>

#define CPU_32BIT 1
#define CPU_64BIT 0

#if CPU_64BIT
#define CPU_BITS 64
#define uintCPUWord_t uint64_t
#define intCPUWord_t int64_t
#else
#define CPU_BITS 32
#define uintCPUWord_t uint32_t
#define intCPUWord_t int32_t
#endif
