#pragma once

#include "field.h"

namespace LTPuyo
{

struct Placement
{
    uint8_t x = 0;
    Rotation rotation = Rotation::UP;
};

static void generate(Field& field, Placement result[22], int& count)
{
    count = 0;

    int height[6];
    field.get_height(height);

    for (uint8_t x = 0; x < 6; ++x) {
        if (height[x] > 11 || (height[2] > 9 && x == 2)) {
            continue;
        }

        result[count] = Placement { x, Rotation::UP };
        ++count;
    }

    for (uint8_t x = 0; x < 6; ++x) {
        if (height[x] > 11 || (height[2] > 9 && x == 2)) {
            continue;
        }

        result[count] = Placement { x, Rotation::DOWN };
        ++count;
    }

    for (uint8_t x = 0; x < 5; ++x) {
        if (height[x] > 12 || height[x + 1] > 12 || (height[2] > 10 && (x == 1 || x == 2))) {
            continue;
        }

        result[count] = Placement { x, Rotation::RIGHT };
        ++count;
    }

    for (uint8_t x = 1; x < 6; ++x) {
        if (height[x] > 12 || height[x - 1] > 12 || (height[2] > 10 && (x == 2 || x == 3))) {
            continue;
        }

        result[count] = Placement { x, Rotation::LEFT };
        ++count;
    }
};

static void bench_genmove(Field field, int iter)
{
    using namespace std;

    int64_t time = 0;

    for (int i = 0; i < iter; ++i) {
        Field copy = field;
        Placement result[22];
        int count = 0;

        auto time_1 = chrono::high_resolution_clock::now();
        generate(copy, result, count);
        auto time_2 = chrono::high_resolution_clock::now();

        time += chrono::duration_cast<chrono::nanoseconds>(time_2 - time_1).count();
    }

    cout << "time: " << (time / iter) << " ns" << endl;
};

};