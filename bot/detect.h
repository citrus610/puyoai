#pragma once

#include "../core/field.h"

struct DetectorScore
{
    int chain_count = 0;
    int puyo_add_count = 0;
};

class Detector
{
public:
    static DetectorScore detect(Field& field);
};

static inline bool operator < (const DetectorScore& a, const DetectorScore& b) {
    if (a.chain_count == b.chain_count) {
        return b.puyo_add_count < a.puyo_add_count;
    }
    return a.chain_count < b.chain_count;
};

static inline bool operator == (const DetectorScore& a, const DetectorScore& b) {
    return a.chain_count == b.chain_count && a.puyo_add_count == b.puyo_add_count;
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