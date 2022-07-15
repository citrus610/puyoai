#pragma once

#include "node.h"
#include "gen.h"

struct HeuristicAccumulate
{
    int connection = 0;
    int connection_horizontal = 0;
    int connection_vertical_side = 0;
};

struct HeuristicEvaluation
{
    int height_third_column = 0;
    int well = 0;
    int well_sq = 0;
    int bump = 0;
    int bump_sq = 0;
    int shape_u = 0;
    int shape_u_sq = 0;
    int side_bias = 0;
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
    void evaluate(Node& node, Node& parent, Placement placement, Pair pair);
public:
    static void connection(Field previous, Placement placement, Pair pair, int result[3]);
public:
    static void well(int height[6], int result[2]);
    static void bump(int height[6], int result[2]);
    static void shape_u(int height[6], int result[2]);
};

constexpr Heuristic DEFAULT_HEURISTIC()
{
    Heuristic result;

    result.accumulate.connection = 251;
    result.accumulate.connection_horizontal = 0;
    result.accumulate.connection_vertical_side = 237;
    
    result.evaluation.height_third_column = -81;
    result.evaluation.well = -65;
    result.evaluation.well_sq = -7;
    result.evaluation.bump = -13;
    result.evaluation.bump_sq = -42;
    result.evaluation.shape_u = -102;
    result.evaluation.shape_u_sq = -105;
    result.evaluation.side_bias = -4;

    return result;
};