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

namespace LTPuyo
{

constexpr int CHAIN_COLOR_BONUS_SIZE = 6;
constexpr int CHAIN_GROUP_BONUS_SIZE = 12;
constexpr int CHAIN_POWER_SIZE = 20;

constexpr int CHAIN_COLOR_BONUS[CHAIN_COLOR_BONUS_SIZE] = { 0, 0, 3, 6, 12, 24 };
constexpr int CHAIN_GROUP_BONUS[CHAIN_GROUP_BONUS_SIZE] = { 0, 0, 0, 0, 0, 2, 3, 4, 5, 6, 7, 10 };
constexpr int CHAIN_POWER[CHAIN_POWER_SIZE] = { 0, 0, 8, 16, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512 };

enum Puyo : uint8_t
{
    RED,
    YELLOW,
    GREEN,
    BLUE,
    GARBAGE,
    COUNT,
    NONE
};

enum Rotation : uint8_t
{
    UP,
    RIGHT,
    DOWN,
    LEFT
};

struct Chain
{
    int link[19] = { 0 };
    int count = 0;
    bool color[Puyo::COUNT - 1] = { false };
};

class FieldMono
{
public:
    uint16_t column[6] = { 0 };
public:
    int popcount(int x);
    int popcount();
public:
    void expand(FieldMono& result);
public:
    FieldMono operator | (const FieldMono& other);
    FieldMono operator & (const FieldMono& other);
};

class Field
{
public:
    FieldMono puyo[Puyo::COUNT] = { 0 };
public:
    void set_puyo(int x, int y, Puyo puyo);
public:
    Puyo get_puyo(int x, int y);
    void get_height(int height[6]);
    int get_height(int x);
public:
    int popcount();
public:
    void drop_puyo(int x, Puyo puyo);
    void drop_pair(int x, Puyo puyo[2], Rotation rotation);
public:
    void poppable_mask(FieldMono& mask, bool color[Puyo::COUNT - 1]);
    void pop(Chain& chain);
public:
    static int calculate_point(Chain& chain);
    static int calculate_garbage(Chain& chain, int target_point = 70, bool all_clear = false);
public:
    void from(const char c[13][7]);
    void print();
};

class Slice
{
public:
    uint16_t data[7] = { 0 };
    uint16_t size;
public:
    void from(uint16_t column);
    void erase(int idx);
};

class FieldMonoSlice
{
public:
    Slice column[6] = { 0 };
public:
    void from(FieldMono field_mono);
    void find_connect_slice(int x, int idx, FieldMono& mask);
    bool find_poppable_mask(FieldMono& mask);
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

constexpr char convert_puyo_char(Puyo puyo)
{
    switch (puyo)
    {
    case Puyo::RED:
        return 'R';
    case Puyo::YELLOW:
        return 'Y';
    case Puyo::GREEN:
        return 'G';
    case Puyo::BLUE:
        return 'B';
    case Puyo::GARBAGE:
        return '#';
    default:
        return '.';
    }
    return '.';
};

constexpr Puyo convert_char_puyo(char c)
{
    switch (c)
    {
    case 'R':
        return Puyo::RED;
    case 'Y':
        return Puyo::YELLOW;
    case 'G':
        return Puyo::GREEN;
    case 'B':
        return Puyo::BLUE;
    case '#':
        return Puyo::GARBAGE;
    default:
        return Puyo::NONE;
    }
    return Puyo::NONE;
};

static void bench_pop(Field field, int iter)
{
    using namespace std;

    int64_t time = 0;

    for (int i = 0; i < iter; ++i) {
        Field copy = field;
        Chain chain;

        auto time_1 = chrono::high_resolution_clock::now();
        copy.pop(chain);
        auto time_2 = chrono::high_resolution_clock::now();

        time += chrono::duration_cast<chrono::nanoseconds>(time_2 - time_1).count();
    }

    cout << "time: " << (time / iter) << " ns" << endl;
};

};