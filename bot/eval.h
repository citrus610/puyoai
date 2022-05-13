#pragma once

#include "node.h"
#include "gen.h"

namespace LTPuyo
{

struct HeuristicAccumulate
{
    int connection = 0;
    int connection_horizontal = 0;
    int disconnection = 0;
};

struct HeuristicEvaluation
{
    int bumpiness = 0;
    int bumpiness_sq = 0;
    int sidewell = 0;
    int sidewell_sq = 0;
};

struct Heuristic
{
    HeuristicAccumulate accumulate;
    HeuristicEvaluation evaluation;
};

class Evaluator
{
public:
    Heuristic heuristic;
public:
    void evaluate(Node& node, Node& parent, Placement placement, Puyo puyo[2]);
public:
    static int bumpiness(int height[6]);
    static int bumpiness_sq(int height[6]);
    static void sidewell(int height[6], int result[2]);
    static void connection(Field previous, Placement placement, Puyo puyo[2], int result[2]);
};

constexpr Heuristic DEFAULT_HEURISTIC()
{
    Heuristic result;

    result.accumulate.connection = 176;
    result.accumulate.connection_horizontal = 114;
    result.accumulate.disconnection = -11;
    
    result.evaluation.bumpiness = -68;
    result.evaluation.bumpiness_sq = -68;
    result.evaluation.sidewell = -54;
    result.evaluation.sidewell_sq = -33;

    return result;
};

};