#pragma once

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <time.h>
#include <cmath>
#include <chrono>
#include <vector>
#include <array>
#include <unordered_map>
#include <cassert>
#include <algorithm>
#include <bitset>
#include <bit>
#include <thread>
#include <mutex>
#include <atomic>
#include <optional>
#include <x86intrin.h>
#include <condition_variable>
#include <stdalign.h>
#include "avec.h"
#include "hash.h"

typedef int8_t i8;
typedef uint8_t u8;
typedef int16_t i16;
typedef uint16_t u16;
typedef int32_t i32;
typedef uint32_t u32;
typedef int64_t i64;
typedef uint64_t u64;

static inline uint16_t pext16(uint16_t input, uint16_t mask)
{
#ifdef USE_PEXT
    return _pext_u32(uint32_t(input), uint32_t(mask));
#else
    uint16_t result = 0;
    for (uint16_t bb = 1; mask != 0; bb += bb) {
        if (input & mask & -mask) {
            result |= bb;
        }
        mask &= (mask - 1);
    }
    return result;
#endif
};