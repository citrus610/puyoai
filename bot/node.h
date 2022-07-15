#pragma once

#include "../core/field.h"

struct NodeScore
{
    int accumulate = 0;
    int evaluation = 0;
};

struct Node
{
    Field field = Field();
    NodeScore score = NodeScore();
};

static bool operator < (const NodeScore& a, const NodeScore& b)
{
    return a.accumulate + a.evaluation < b.accumulate + b.evaluation;
};

static bool operator < (const Node& a, const Node& b)
{
    return a.score < b.score;
};