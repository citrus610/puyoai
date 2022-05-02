#pragma once

#include "node.h"
#include "gen.h"

namespace LTPuyo
{

enum Shape
{
    STAIR13,
    STAIR22,
    SW112,
    SW211,
    SW103,
    SW13,
    SW31,
    GTR,
    SHAPE_COUNT
};

constexpr int8_t SHAPE[Shape::SHAPE_COUNT][4][2] = 
{
    {{0, 0}, {1, -1}, {1, -2}, {1, -3}}, // STAIR13
    {{0, 0}, {0, -1}, {1, -2}, {1, -3}}, // STAIR22
    {{0, 0}, {1, -1}, {0, -2}, {0, -3}}, // SW112
    {{0, 0}, {0, -1}, {1, -2}, {0, -3}}, // SW211
    {{0, 0}, {0, -2}, {1, -2}, {0, -3}}, // SW103
    {{0, 0}, {1, -1}, {0, -2}, {1, -2}}, // SW13
    {{0, 0}, {1, 0}, {1, -1}, {0, -2}},  // SW31
    {{0, 0}, {1, -1}, {-1, -2}, {0, -2}} // GTR
};

struct HeuristicAccumulate
{
    int connection = 0;
    int disconnection = 0;
    int shape[Shape::SHAPE_COUNT] = { 0 };
};

struct HeuristicEvaluation
{
    int height_left = 0;
    int height_center = 0;
    int height_right = 0;
    int bumpiness_left = 0;
    int bumpiness_right = 0;
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
    static int bumpiness_left(int height[6]);
    static int bumpiness_right(int height[6]);
    static int connection(Field previous, Placement placement, Puyo puyo[2]);
    static int detect_shape(Field field, Placement placement, Puyo puyo[2], int height[6], Shape shape);
};

constexpr Heuristic DEFAULT_HEURISTIC()
{
    Heuristic result;

    result.accumulate.connection = 100;
    result.accumulate.disconnection = -25;
    result.accumulate.shape[0] = 25;
    result.accumulate.shape[1] = 25;
    result.accumulate.shape[2] = 50;
    result.accumulate.shape[3] = 50;
    result.accumulate.shape[4] = 50;
    result.accumulate.shape[5] = 50;
    result.accumulate.shape[6] = 50;
    result.accumulate.shape[7] = 50;
    result.evaluation.height_left = -30;
    result.evaluation.height_center = -5;
    result.evaluation.height_right = -30;
    result.evaluation.bumpiness_left = -20;
    result.evaluation.bumpiness_right = -20;

    return result;
};

};