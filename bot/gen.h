#pragma once

#include "field.h"

namespace LTPuyo
{

class Position
{
public:
    int8_t x = 0;
    int8_t y = 0;
    Rotation rotation = Rotation::UP;
public:
    bool move_right(Field& field, int height[6]);
    bool move_left(Field& field, int height[6]);
    bool move_cw(Field& field, int height[6]);
    bool move_ccw(Field& field, int height[6]);
    bool move_180(Field& field, int height[6]);
public:
    void normalize();
};

struct Placement
{
    uint8_t x = 0;
    Rotation rotation = Rotation::UP;
};

class PositionMap
{
public:
    bool data[6][5][4];
public:
    PositionMap();
public:
    void clear();
public:
    bool get(int x, int y, Rotation rotation);
    void set(int x, int y, Rotation rotation, bool value);
};

class PlacementMap
{
public:
    bool full[6][4];
    bool partial[6][2];
public:
    PlacementMap();
public:
    void clear();
public:
    bool get_full(int x, Rotation rotation);
    void set_full(int x, Rotation rotation, bool value);
public:
    bool get_partial(int x, int idx);
    void set_partial(int x, int idx, bool value);
};

class Generator
{
public:
    static void generate(Field& field, Pair pair, avec<Placement, 22>& placement);
    static void expand(Field& field, int height[6], Position& position, avec<Position, 120>& queue, PositionMap& queue_map);
    static void lock(Field& field, int height[6], Position& position, avec<Placement, 22>& locks, PlacementMap& locks_map, bool equal_pair);
};

static void bench_genmove(Field field, int iter)
{
    using namespace std;

    int64_t time = 0;

    for (int i = 0; i < iter; ++i) {
        Field copy = field;
        avec<Placement, 22> placement;

        auto time_1 = chrono::high_resolution_clock::now();
        Generator::generate(copy, { Puyo::RED, Puyo::BLUE }, placement);
        auto time_2 = chrono::high_resolution_clock::now();

        time += chrono::duration_cast<chrono::nanoseconds>(time_2 - time_1).count();
    }

    cout << "time: " << (time / iter) << " ns" << endl;
};

static void print_genmove(Field field, Pair pair)
{
    using namespace std;

    avec<Placement, 22> placement;
    Generator::generate(field, pair, placement);

    for (int i = 0; i < placement.get_size(); ++i) {
        Field copy = field;
        Puyo puyo[2] = { pair.first, pair.second };
        copy.drop_pair(placement[i].x, puyo, placement[i].rotation);
        copy.print();
        cin.get();
        system("cls");
    }
};

};