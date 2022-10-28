#pragma once

#include "../core/core.h"

class Detect
{
public:
    struct Score
    {
        u32 score = 0;
        u8 chain = 0;
        u8 added = 0;
        u8 height = 0;
    };
public:
    static Score detect(Field& field);
public:
    static void bench();
};

static inline bool operator < (const Detect::Score& a, const Detect::Score& b) {
    if (a.chain == b.chain) {
        return a.score < b.score;
    }
    return a.chain < b.chain;
};

static inline bool operator == (const Detect::Score& a, const Detect::Score& b) {
    return a.score == b.score && a.chain == b.chain;
};