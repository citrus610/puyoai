#include "eval.h"

void Evaluator::evaluate(Node& node, Node& parent, Placement placement, Pair pair)
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

    int side_bias = std::max(0, height[0] + height[1] + height[2] - height[3] - height[4] - height[5]);
    node.score.evaluation += side_bias * this->heuristic.evaluation.side_bias;

    int connection[3];
    Evaluator::connection(parent.field, placement, pair, connection);
    node.score.accumulate += connection[0] * this->heuristic.accumulate.connection;
    node.score.accumulate += connection[1] * this->heuristic.accumulate.connection_horizontal;
    node.score.accumulate += connection[2] * this->heuristic.accumulate.connection_vertical_side;
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

void Evaluator::connection(Field previous, Placement placement, Pair pair, int result[3])
{
    result[0] = 0;
    result[1] = 0;
    result[2] = 0;

    Puyo puyo[2] = { pair.first, pair.second };
    if (placement.rotation == Rotation::DOWN) {
        puyo[0] = pair.second;
        puyo[1] = pair.first;
    }

    int height[6];
    previous.get_height(height);

    result[0] += previous.get_puyo(placement.x - 1, height[placement.x]) == puyo[0];
    result[0] += previous.get_puyo(placement.x + 1, height[placement.x]) == puyo[0];
    result[0] += previous.get_puyo(placement.x, height[placement.x] - 1) == puyo[0];
    result[1] += previous.get_puyo(placement.x - 1, height[placement.x]) == puyo[0] && height[placement.x] < 4;
    result[1] += previous.get_puyo(placement.x + 1, height[placement.x]) == puyo[0] && height[placement.x] < 4;
    result[2] += previous.get_puyo(placement.x, height[placement.x] - 1) == puyo[0] && (placement.x == 0 || placement.x == 5) && height[placement.x] > 0;

    previous.set_puyo(placement.x, height[placement.x], puyo[0]);
    ++height[placement.x];
    
    int second_puyo_x = placement.x;
    if (placement.rotation == Rotation::RIGHT) {
        ++second_puyo_x;
    }
    else if (placement.rotation == Rotation::LEFT) {
        --second_puyo_x;
    }

    result[0] += previous.get_puyo(second_puyo_x - 1, height[second_puyo_x]) == puyo[1];
    result[0] += previous.get_puyo(second_puyo_x + 1, height[second_puyo_x]) == puyo[1];
    result[0] += previous.get_puyo(second_puyo_x, height[second_puyo_x] - 1) == puyo[1];
    result[1] += previous.get_puyo(second_puyo_x - 1, height[second_puyo_x]) == puyo[1] && height[second_puyo_x] < 4;
    result[1] += previous.get_puyo(second_puyo_x + 1, height[second_puyo_x]) == puyo[1] && height[second_puyo_x] < 4;
    result[2] += previous.get_puyo(second_puyo_x, height[second_puyo_x] - 1) == puyo[1] && (second_puyo_x == 0 || second_puyo_x == 5) && height[second_puyo_x] > 0;
};