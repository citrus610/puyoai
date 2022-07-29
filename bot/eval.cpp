#include "eval.h"

void Evaluator::evaluate(Node& node, Node& parent, Placement placement, Pair pair)
{
    Evaluator::evaluate_evaluation(node);
    Evaluator::evaluate_accumulate(node, parent, placement, pair);
};

void Evaluator::evaluate_evaluation(Node& node)
{
    node.score.evaluation = 0;

    int height[6];
    node.field.get_height(height);

    node.score.evaluation += height[2] * this->heuristic.evaluation.height_third_column;

    int height_min = height[0];
    int height_max = height[0];
    for (int i = 1; i < 6; ++i) {
        if (height[i] < height_min) {
            height_min = height[i];
        }
        if (height[i] > height_max) {
            height_max = height[i];
        }
    }

    int height_delta = height_max - height_min;
    node.score.evaluation += height_delta * this->heuristic.evaluation.height_delta;
    node.score.evaluation += height_delta * height_delta * this->heuristic.evaluation.height_delta_sq;

    int well[2];
    Evaluator::well(height, well);
    node.score.evaluation += well[0] * this->heuristic.evaluation.well;
    node.score.evaluation += well[1] * this->heuristic.evaluation.well_sq;

    int bump[2];
    Evaluator::bump(height, bump);
    node.score.evaluation += bump[0] * this->heuristic.evaluation.bump;
    node.score.evaluation += bump[1] * this->heuristic.evaluation.bump_sq;

    int shape_u[2];
    Evaluator::shape_u(height, shape_u);
    node.score.evaluation += shape_u[0] * this->heuristic.evaluation.shape_u;
    node.score.evaluation += shape_u[1] * this->heuristic.evaluation.shape_u_sq;

    int side_bias = std::abs(height[0] + height[1] + height[2] - height[3] - height[4] - height[5]);
    node.score.evaluation += side_bias * this->heuristic.evaluation.side_bias;

    int pattern_middle_y = Evaluator::pattern_middle_y(node.field);
    node.score.evaluation += pattern_middle_y * this->heuristic.evaluation.pattern_middle_y;

    int pattern_left_l = Evaluator::pattern_left_l(node.field);
    node.score.evaluation += pattern_left_l * this->heuristic.evaluation.pattern_left_l;
};

void Evaluator::evaluate_accumulate(Node& node, Node& parent, Placement placement, Pair pair)
{
    Field previous_field = parent.field;

    // Place 1st puyo
    Puyo puyo[2] = { pair.first, pair.second };
    if (placement.rotation == Rotation::DOWN) {
        puyo[0] = pair.second;
        puyo[1] = pair.first;
    }

    int height[6];
    previous_field.get_height(height);

    // Link count
    int link[4] = { 0, 0, 0, 0 };
    Evaluator::link(previous_field, height, placement.x, puyo[0], link);
    node.score.accumulate += link[0] * this->heuristic.accumulate.link;
    node.score.accumulate += link[1] * this->heuristic.accumulate.link_hor_bottom;
    node.score.accumulate += link[2] * this->heuristic.accumulate.link_hor_left;
    node.score.accumulate += link[3] * this->heuristic.accumulate.link_ver_side;

    // Ugly placement
    int ugly = Evaluator::ugly(previous_field, height, placement.x, puyo[0]);
    node.score.accumulate += ugly * this->heuristic.accumulate.ugly;

    // Place 2nd puyo
    previous_field.set_puyo(placement.x, height[placement.x], puyo[0]);
    ++height[placement.x];
    
    int second_puyo_x = placement.x;
    if (placement.rotation == Rotation::RIGHT) {
        ++second_puyo_x;
    }
    else if (placement.rotation == Rotation::LEFT) {
        --second_puyo_x;
    }

    // Link count
    link[0] = 0;
    link[1] = 0;
    link[2] = 0;
    link[3] = 0;
    Evaluator::link(previous_field, height, second_puyo_x, puyo[1], link);
    node.score.accumulate += link[0] * this->heuristic.accumulate.link;
    node.score.accumulate += link[1] * this->heuristic.accumulate.link_hor_bottom;
    node.score.accumulate += link[2] * this->heuristic.accumulate.link_hor_left;
    node.score.accumulate += link[3] * this->heuristic.accumulate.link_ver_side;

    // Ugly placement
    ugly = Evaluator::ugly(previous_field, height, second_puyo_x, puyo[1]);
    node.score.accumulate += ugly * this->heuristic.accumulate.ugly;
};

void Evaluator::well(int height[6], int result[2])
{
    result[0] = 0;
    result[1] = 0;

    for (int i = 1; i < 5; ++i) {
        int value = std::min(std::max(0, height[i - 1] - height[i]), std::max(0, height[i + 1] - height[i]));
        result[0] += value;
        result[1] += value * value;
    }

    int value_0 = std::max(0, height[1] - height[0]);
    int value_5 = std::max(0, height[4] - height[5]);

    result[0] += value_0 + value_5;
    result[1] += value_0 * value_0 + value_5 * value_5;
};

void Evaluator::bump(int height[6], int result[2])
{
    result[0] = 0;
    result[1] = 0;

    for (int i = 0; i < 5; ++i) {
        int value = std::abs(height[i] - height[i + 1]);
        result[0] += value;
        result[1] += value * value;
    }
};

void Evaluator::shape_u(int height[6], int result[2])
{
    result[0] = 0;
    result[1] = 0;

    for (int i = 0; i < 2; ++i) {
        if (height[i] < height[i + 1]) {
            int value = height[i + 1] - height[i];
            result[0] += value;
            result[1] += value * value;
        }
    }

    for (int i = 3; i < 5; ++i) {
        if (height[i] > height[i + 1]) {
            int value = height[i] - height[i + 1];
            result[0] += value;
            result[1] += value * value;
        }
    }
};

int Evaluator::pattern_middle_y(Field& field)
{
    int result = 0;

    result += field.get_puyo(2, 0) == field.get_puyo(2, 2) && field.get_puyo(2, 0) == field.get_puyo(3, 1);

    if (result) {
        result += field.get_puyo(2, 0) == field.get_puyo(4, 1);
        result += field.get_puyo(2, 0) == field.get_puyo(5, 1);
        result += field.get_puyo(2, 0) == field.get_puyo(2, 3);
    }
    
    return result;
};

int Evaluator::pattern_left_l(Field& field)
{
    return
        (field.get_puyo(0, 1) == field.get_puyo(0, 2) && field.get_puyo(0, 1) == field.get_puyo(1, 1)) ||
        (field.get_puyo(1, 1) == field.get_puyo(1, 2) && field.get_puyo(1, 1) == field.get_puyo(2, 1));
};

void Evaluator::link(Field& field, int height[6], int x, Puyo puyo, int result[4])
{
    // Normal limk count
    result[0] += field.get_puyo(x - 1, height[x]) == puyo;
    result[0] += field.get_puyo(x + 1, height[x]) == puyo;
    result[0] += field.get_puyo(x, height[x] - 1) == puyo;

    // Link count horizontal under height of 3 - link bottom
    result[1] += field.get_puyo(x - 1, height[x]) == puyo && height[x] < 3;
    result[1] += field.get_puyo(x + 1, height[x]) == puyo && height[x] < 3;

    // Link count horizontal left side
    result[2] += field.get_puyo(x - 1, height[x]) == puyo && height[x] > 2 && (x == 1 || x == 5);
    result[2] += field.get_puyo(x + 1, height[x]) == puyo && height[x] > 2 && (x == 0 || x == 4);

    // Link count vertical side
    result[3] += field.get_puyo(x, height[x] - 1) == puyo && (x == 0 || x == 5) && height[x] > 0;
};

int Evaluator::ugly(Field& field, int height[6], int x, Puyo puyo)
{
    int result = 0;

    // Ugly 1: same color across a well
    result += x > 1 && height[x - 1] <= height[x] && height[x - 1] < height[x - 2] && (field.puyo[static_cast<int>(puyo)].column[x - 2] & (1 << height[x]));
    result += x < 4 && height[x + 1] <= height[x] && height[x + 1] < height[x + 2] && (field.puyo[static_cast<int>(puyo)].column[x + 2] & (1 << height[x]));

    // Ugly 2: same color above diagonally
    result += x > 0 && height[x] < height[x - 1] && (field.puyo[static_cast<int>(puyo)].column[x - 1] & (1 << (height[x] + 1)));
    result += x < 5 && height[x] < height[x + 1] && (field.puyo[static_cast<int>(puyo)].column[x + 1] & (1 << (height[x] + 1)));

    // Ugly 3: same color below diagonally
    result += x > 0 && height[x] == height[x - 1] && (field.puyo[static_cast<int>(puyo)].column[x - 1] & (1 << (height[x] - 1)));
    result += x < 5 && height[x] == height[x + 1] && (field.puyo[static_cast<int>(puyo)].column[x + 1] & (1 << (height[x] - 1)));

    return result;
};