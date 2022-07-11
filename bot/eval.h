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
    void evaluate(Node& node, Node& parent, Placement placement, Puyo puyo[2]);
public:
    static void connection(Field previous, Placement placement, Puyo puyo[2], int result[3]);
public:
    static void well(int height[6], int result[2]);
    static void bump(int height[6], int result[2]);
    static void shape_u(int height[6], int result[2]);
};

constexpr Heuristic DEFAULT_HEURISTIC()
{
    Heuristic result;

    result.accumulate.connection = 100;
    result.accumulate.connection_horizontal = 0;
    result.accumulate.connection_vertical_side = 100;
    
    result.evaluation.height_third_column = -50;
    result.evaluation.well = -100;
    result.evaluation.well_sq = -20;
    result.evaluation.bump = -100;
    result.evaluation.bump_sq = -30;
    result.evaluation.shape_u = -200;
    result.evaluation.shape_u_sq = -30;
    result.evaluation.side_bias = 0;

    return result;
};

};