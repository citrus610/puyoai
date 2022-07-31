#pragma once

#include "node.h"
#include "gen.h"

struct HeuristicAccumulate
{
    int link = 0;
    int link_hor_bottom = 0;
    int link_ver_side = 0;
};

struct HeuristicEvaluation
{
    int height_col_3 = 0;
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
    void evaluate_evaluation(Node& node);
    void evaluate_accumulate(Node& node, Node& parent, Placement placement, Pair pair);
public:
    static void link(Field& field, int height[6], int x, Puyo puyo, int result[3]);
public:
    static void well(int height[6], int result[2]);
    static void bump(int height[6], int result[2]);
    static void shape_u(int height[6], int result[2]);
};

constexpr Heuristic DEFAULT_HEURISTIC()
{
    Heuristic result;

    result.accumulate.link = 251;
    result.accumulate.link_hor_bottom = 0;
    result.accumulate.link_ver_side = 237;
    
    result.evaluation.height_col_3 = -81;
    result.evaluation.well = -65;
    result.evaluation.well_sq = -7;
    result.evaluation.bump = -13;
    result.evaluation.bump_sq = -42;
    result.evaluation.shape_u = -102;
    result.evaluation.shape_u_sq = -105;
    result.evaluation.side_bias = -4;

    return result;
};