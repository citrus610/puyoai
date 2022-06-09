#pragma once

#include "node.h"
#include "gen.h"

namespace LTPuyo
{

struct HeuristicAccumulate
{
    int connection = 0;
    int connection_horizontal = 0;
    int connection_vertical_side = 0;
    int disconnection = 0;
};

struct HeuristicEvaluation
{
    int bumpiness = 0;
    int bumpiness_sq = 0;
    int sidewell = 0;
    int sidewell_sq = 0;
    int valley_sq = 0;
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
    static int valley_sq(int height[6]);
    static void sidewell(int height[6], int result[2]);
    static void connection(Field previous, Placement placement, Puyo puyo[2], int result[3]);
};

constexpr Heuristic DEFAULT_HEURISTIC()
{
    Heuristic result;

    result.accumulate.connection = 165;
    result.accumulate.connection_horizontal = 100;
    result.accumulate.connection_vertical_side = 100;
    result.accumulate.disconnection = -38;
    
    result.evaluation.bumpiness = -69;
    result.evaluation.bumpiness_sq = -83;
    result.evaluation.sidewell = -55;
    result.evaluation.sidewell_sq = -31;
    result.evaluation.valley_sq = -36;

    return result;
};

};