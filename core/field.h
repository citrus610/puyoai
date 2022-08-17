#pragma once

#include "slice.h"

// One-color field
// Field is represented as a column-major bitboard, six u16 as six columns
// Can ultilize fast instruction like lzcnt, tzcnt, pext, popcnt
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
public:
    bool operator == (const FieldMono& other);
    bool operator != (const FieldMono& other);
};

// A puyo field
class Field
{
public:
    FieldMono puyo[static_cast<uint8_t>(Puyo::COUNT)] = { 0 };
public:
    void set_puyo(int x, int y, Puyo puyo);
public:
    Puyo get_puyo(int x, int y);
    void get_height(int height[6]);
    int get_height(int x);
    int get_height_max();
public:
    bool is_occupied(int height[6], int x, int y);
    bool is_colliding(int height[6], int x, int y, Rotation r);
public:
    int popcount();
    int count_group(int x, int y);
    void flood_fill_count(int height[6], bool visit[13][6], int x, int y, Puyo puyo, int& count);
public:
    void drop_puyo(int x, Puyo puyo);
    void drop_pair(int x, Rotation rotation, Puyo puyo[2]);
    void drop_pair(int x, Rotation rotation, Pair pair);
public:
    void poppable_mask(FieldMono& mask, int& color);
    bool poppable_drop(int x, Rotation rotation, Pair pair);
    void pop(Chain& chain);
public:
    static int calculate_point(Chain& chain);
public:
    bool operator == (const Field& other);
    bool operator != (const Field& other);
public:
    void from(const char c[13][7]);
    void print();
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