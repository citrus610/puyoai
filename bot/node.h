#pragma once

#include "field.h"

namespace LTPuyo
{

struct Score
{
    int accumulate = 0;
    int evaluation = 0;
};

struct Node
{
    Field field;
    Score score;
};

static bool operator < (const Score& a, const Score& b)
{
    return a.accumulate + a.evaluation < b.accumulate + b.evaluation;
};

static bool operator < (const Node& a, const Node& b)
{
    return a.score < b.score;
};

};