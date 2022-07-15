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
#include <x86intrin.h>
#include <condition_variable>
#include <stdalign.h>
#include "avec.h"
#include "../lib/nlohmann/json.hpp"

constexpr int CHAIN_COLOR_BONUS_SIZE = 6;
constexpr int CHAIN_GROUP_BONUS_SIZE = 12;
constexpr int CHAIN_POWER_SIZE = 20;

constexpr int CHAIN_COLOR_BONUS[CHAIN_COLOR_BONUS_SIZE] = { 0, 0, 3, 6, 12, 24 };
constexpr int CHAIN_GROUP_BONUS[CHAIN_GROUP_BONUS_SIZE] = { 0, 0, 0, 0, 0, 2, 3, 4, 5, 6, 7, 10 };
constexpr int CHAIN_POWER[CHAIN_POWER_SIZE] = { 0, 0, 8, 16, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512 };

constexpr int8_t ROTATION_OFFSET[4][2] = {
    { 0, 1 },
    { 1, 0 },
    { 0, -1 },
    { -1, 0 }
};

enum class Puyo : uint8_t
{
    RED,
    YELLOW,
    GREEN,
    BLUE,
    GARBAGE,
    COUNT,
    NONE
};

enum class Rotation : uint8_t
{
    UP,
    RIGHT,
    DOWN,
    LEFT
};

using Pair = std::pair<Puyo, Puyo>;

struct Chain
{
    int link[19] = { 0 };
    int color[19] = { 0 };
    int count = 0;
};

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