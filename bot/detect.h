#pragma once

#include "../core/field.h"

struct DetectorScore
{
    int chain_count = 0;
    int chain_score = 0;
};

class Detector
{
public:
    static DetectorScore detect(Field& field);
public:
    static void flood_fill(Field& field, int height[6], bool visit[13][6], int x, int y, Puyo puyo, FieldMono& mask);
    static void normalize_height(int height[6], int height_normalize[6], int min_x, int max_x);
    static void min_y(int height_normalize[6], int min_y[6], int min_x, int max_x);
};

static void bench_detect()
{
    using namespace std;

    const char c[13][7] = {
        "....B.",
        "....YB",
        "....YG",
        "...BBB",
        "...YYG",
        "...RYB",
        "R.GYRB",
        "BRYRGB",
        "RGGYRG",
        "RRGYRG",
        "BBYGBB",
        "BRRYGG",
        "RYYGBB",
    };

    Field field;
    field.from(c);
    field.print();

    int64_t time = 0;
    const int iter = 200000;

    for (int i = 0; i < iter; ++i) {
        auto time_1 = chrono::high_resolution_clock::now();
        Detector::detect(field);
        auto time_2 = chrono::high_resolution_clock::now();

        time += chrono::duration_cast<chrono::nanoseconds>(time_2 - time_1).count();
    }

    cout << "time: " << (time / iter) << " ns" << endl;
};