#include "eval.h"

namespace LTPuyo
{

void Evaluator::evaluate(Node& node, Node& parent, Placement placement, Puyo puyo[2])
{
    node.score.evaluation = 0;

    int height[6];
    node.field.get_height(height);

    node.score.evaluation += Evaluator::bumpiness_left(height) * this->heuristic.evaluation.bumpiness_left;
    node.score.evaluation += Evaluator::bumpiness_right(height) * this->heuristic.evaluation.bumpiness_right;
    node.score.evaluation += *std::max_element(height + 0, height + 3) * this->heuristic.evaluation.height_left;
    node.score.evaluation += *std::max_element(height + 3, height + 6) * this->heuristic.evaluation.height_right;
    node.score.evaluation += std::max(0, height[2] - 8) * this->heuristic.evaluation.height_center;

    int connection = Evaluator::connection(parent.field, placement, puyo);
    node.score.accumulate += connection * this->heuristic.accumulate.connection;
    node.score.accumulate += (3 - connection) * this->heuristic.accumulate.disconnection;
    for (int i = 0; i < Shape::SHAPE_COUNT; ++i) {
        node.score.accumulate += Evaluator::detect_shape(node.field, placement, puyo, height, Shape(i)) * this->heuristic.accumulate.shape[i];
    }
};

int Evaluator::bumpiness_left(int height[6])
{
    int result = 0;
    for (int i = 0; i < 3; ++i) {
        result += std::abs(height[i] - height[i + 1]);
    }
    return result;
};

int Evaluator::bumpiness_right(int height[6])
{
    int result = 0;
    for (int i = 3; i < 6; ++i) {
        result += std::abs(height[i] - height[i + 1]);
    }
    return result;
};

int Evaluator::connection(Field previous, Placement placement, Puyo puyo[2])
{
    int result = 0;

    Puyo pair[2] = { puyo[0], puyo[1] };
    if (placement.rotation == Rotation::DOWN) {
        pair[0] = puyo[1];
        pair[1] = puyo[0];
    }

    int height[6];
    previous.get_height(height);

    result += previous.get_puyo(placement.x - 1, height[placement.x]) == pair[0];
    result += previous.get_puyo(placement.x + 1, height[placement.x]) == pair[0];
    result += previous.get_puyo(placement.x, height[placement.x] - 1) == pair[0];

    previous.set_puyo(placement.x, height[placement.x], pair[0]);
    ++height[placement.x];
    
    int second_puyo_x = placement.x;
    if (placement.rotation == Rotation::RIGHT) {
        ++second_puyo_x;
    }
    else if (placement.rotation == Rotation::LEFT) {
        --second_puyo_x;
    }

    result += previous.get_puyo(second_puyo_x - 1, height[second_puyo_x]) == pair[1];
    result += previous.get_puyo(second_puyo_x + 1, height[second_puyo_x]) == pair[1];
    result += previous.get_puyo(second_puyo_x, height[second_puyo_x] - 1) == pair[1];

    return result;
};

int Evaluator::detect_shape(Field field, Placement placement, Puyo puyo[2], int height[6], Shape shape)
{
    int result = 0;

    int8_t position[2][2] = { 0 };

    switch (placement.rotation)
    {
    case Rotation::UP:
        position[0][0] = placement.x;
        position[0][1] = height[placement.x] - 2;
        position[1][0] = placement.x;
        position[1][1] = height[placement.x] - 1;
        break;
    case Rotation::RIGHT:
        position[0][0] = placement.x;
        position[0][1] = height[placement.x] - 1;
        position[1][0] = placement.x + 1;
        position[1][1] = height[placement.x + 1] - 1;
        break;
    case Rotation::LEFT:
        position[0][0] = placement.x;
        position[0][1] = height[placement.x] - 1;
        position[1][0] = placement.x - 1;
        position[1][1] = height[placement.x - 1] - 1;
        break;
    case Rotation::DOWN:
        position[0][0] = placement.x;
        position[0][1] = height[placement.x] - 1;
        position[1][0] = placement.x;
        position[1][1] = height[placement.x] - 2;
        break;
    }

    for (int p = 0; p < 2; ++p) {
        for (int i = 0; i < 4; ++i) {
            if (
                field.get_puyo(position[p][0] + SHAPE[shape][0][0] - SHAPE[shape][i][0], position[p][1] + SHAPE[shape][0][1] - SHAPE[shape][i][1]) == puyo[p] &&
                field.get_puyo(position[p][0] + SHAPE[shape][1][0] - SHAPE[shape][i][0], position[p][1] + SHAPE[shape][1][1] - SHAPE[shape][i][1]) == puyo[p] &&
                field.get_puyo(position[p][0] + SHAPE[shape][2][0] - SHAPE[shape][i][0], position[p][1] + SHAPE[shape][2][1] - SHAPE[shape][i][1]) == puyo[p] &&
                field.get_puyo(position[p][0] + SHAPE[shape][3][0] - SHAPE[shape][i][0], position[p][1] + SHAPE[shape][3][1] - SHAPE[shape][i][1]) == puyo[p]
            ) {
                return 1;
            }
            if (
                field.get_puyo(position[p][0] - SHAPE[shape][0][0] + SHAPE[shape][i][0], position[p][1] - SHAPE[shape][0][1] + SHAPE[shape][i][1]) == puyo[p] &&
                field.get_puyo(position[p][0] - SHAPE[shape][1][0] + SHAPE[shape][i][0], position[p][1] - SHAPE[shape][1][1] + SHAPE[shape][i][1]) == puyo[p] &&
                field.get_puyo(position[p][0] - SHAPE[shape][2][0] + SHAPE[shape][i][0], position[p][1] - SHAPE[shape][2][1] + SHAPE[shape][i][1]) == puyo[p] &&
                field.get_puyo(position[p][0] - SHAPE[shape][3][0] + SHAPE[shape][i][0], position[p][1] - SHAPE[shape][3][1] + SHAPE[shape][i][1]) == puyo[p]
            ) {
                return 1;
            }
        }
    }

    return result;
};

};