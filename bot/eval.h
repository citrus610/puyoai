#pragma once

#include "node.h"
#include "gen.h"

struct HeuristicAccumulate
{
    int link = 0;
    int link_hor_bottom = 0;
    int link_hor_left = 0;
    int link_ver_side = 0;
    int ugly = 0;
};

struct HeuristicEvaluation
{
    int height_third_column = 0;
    int height_delta = 0;
    int height_delta_sq = 0;
    int well = 0;
    int well_sq = 0;
    int bump = 0;
    int bump_sq = 0;
    int shape_u = 0;
    int shape_u_sq = 0;
    int side_bias = 0;
    int pattern_middle_y = 0;
    int pattern_left_l = 0;
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
    static void link(Field& field, int height[6], int x, Puyo puyo, int result[4]);
    static int ugly(Field& field, int height[6], int x, Puyo puyo);
public:
    static void well(int height[6], int result[2]);
    static void bump(int height[6], int result[2]);
    static void shape_u(int height[6], int result[2]);
    static int pattern_middle_y(Field& field);
    static int pattern_left_l(Field& field);
};

constexpr Heuristic DEFAULT_HEURISTIC()
{
    Heuristic result;

    result.accumulate.link = 100;
    result.accumulate.link_hor_bottom = 100;
    result.accumulate.link_hor_left = 100;
    result.accumulate.link_ver_side = 100;
    result.accumulate.ugly = -1000;
    
    result.evaluation.height_third_column = -50;
    result.evaluation.height_delta = -250;
    result.evaluation.height_delta_sq = -50;
    result.evaluation.well = -200;
    result.evaluation.well_sq = -200;
    result.evaluation.bump = -250;
    result.evaluation.bump_sq = -100;
    result.evaluation.shape_u = -500;
    result.evaluation.shape_u_sq = -200;
    result.evaluation.side_bias = -200;
    result.evaluation.pattern_middle_y = 500;
    result.evaluation.pattern_left_l = 500;

    return result;
};