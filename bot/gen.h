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

    for (int8_t x = 4; x >= 0; --x) {
        if (height[x] > 12 || height[x + 1] > 12 || (height[2] > 10 && (x == 1 || x == 2))) {
            continue;
        }

        result[count] = Placement { uint8_t(x), Rotation::RIGHT };
        ++count;
    }

    for (int8_t x = 5; x > 0; --x) {
        if (height[x] > 12 || height[x - 1] > 12 || (height[2] > 10 && (x == 2 || x == 3))) {
            continue;
        }

        result[count] = Placement { uint8_t(x), Rotation::LEFT };
        ++count;
    }

    for (int8_t x = 5; x >= 0; --x) {
        if (height[x] > 11 || (height[2] > 9 && x == 2)) {
            continue;
        }

        result[count] = Placement { uint8_t(x), Rotation::UP };
        ++count;
    }

    for (int8_t x = 5; x >= 0; --x) {
        if (height[x] > 11 || (height[2] > 9 && x == 2)) {
            continue;
        }

        result[count] = Placement { uint8_t(x), Rotation::DOWN };
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