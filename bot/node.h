#pragma once

#include "detect.h"

struct EvalScore
{
    int accumulate = 0;
    int evaluation = 0;
};

struct NodeScore
{
    EvalScore eval = EvalScore();
    DetectorScore dect = DetectorScore();
};

struct Node
{
    Field field = Field();
    NodeScore score = NodeScore();
};

static inline bool operator < (const EvalScore& a, const EvalScore& b)
{
    return a.accumulate + a.evaluation < b.accumulate + b.evaluation;
};

static inline bool operator < (const NodeScore& a, const NodeScore& b)
{
    if (a.dect == b.dect) {
        return a.eval < b.eval;
    }
    return a.dect < b.dect;
};

static inline bool operator < (const Node& a, const Node& b)
{
    return a.score < b.score;
};