#include "eval.h"

namespace LTPuyo
{

void Evaluator::evaluate(Node& node, Node& parent, Placement placement, Puyo puyo[2])
{
    node.score.evaluation = 0;

    int height[6];
    node.field.get_height(height);

    node.score.evaluation += Evaluator::bumpiness(height) * this->heuristic.evaluation.bumpiness;
    node.score.evaluation += Evaluator::bumpiness_sq(height) * this->heuristic.evaluation.bumpiness_sq;

    node.score.evaluation += Evaluator::valley_sq(height) * this->heuristic.evaluation.valley_sq;

    int sidewell[2];
    Evaluator::sidewell(height, sidewell);
    node.score.evaluation += sidewell[0] * this->heuristic.evaluation.sidewell;
    node.score.evaluation += sidewell[1] * this->heuristic.evaluation.sidewell_sq;

    int connection[3];
    Evaluator::connection(parent.field, placement, puyo, connection);
    node.score.accumulate += connection[0] * this->heuristic.accumulate.connection;
    node.score.accumulate += connection[1] * this->heuristic.accumulate.connection_horizontal;
    node.score.accumulate += connection[2] * this->heuristic.accumulate.connection_vertical_side;
    node.score.accumulate += (3 - connection[0]) * this->heuristic.accumulate.disconnection;
};

int Evaluator::bumpiness(int height[6])
{
    int result = 0;
    result += std::abs(height[0] - height[1]);
    result += std::abs(height[1] - height[2]);
    result += std::abs(height[2] - height[3]);
    result += std::abs(height[3] - height[4]);
    result += std::abs(height[4] - height[5]);
    return result;
};

int Evaluator::bumpiness_sq(int height[6])
{
    int result = 0;
    result += (std::abs(height[0] - height[1]) + std::abs(height[1] - height[2])) * (std::abs(height[0] - height[1]) + std::abs(height[1] - height[2]));
    result += (std::abs(height[3] - height[4]) + std::abs(height[4] - height[5])) * (std::abs(height[3] - height[4]) + std::abs(height[4] - height[5]));
    return result;
};

int Evaluator::valley_sq(int height[6])
{
    int result = 0;

    if (height[0] + height[1] - height[2] - height[3] < 0) {
        result += (height[0] + height[1] - height[2] - height[3]) * (height[0] + height[1] - height[2] - height[3]) / 4;
    }

    if (height[4] + height[5] - height[2] - height[3] < 0) {
        result += (height[4] + height[5] - height[2] - height[3]) * (height[4] + height[5] - height[2] - height[3]) / 4;
    }

    return result;
};

void Evaluator::sidewell(int height[6], int result[2])
{
    result[0] = 0;
    result[1] = 0;
    if (height[1] > height[0]) {
        result[0] += height[1] - height[0];
        result[1] += (height[1] - height[0]) * (height[1] - height[0]);
    }
    if (height[4] > height[5]) {
        result[0] += height[4] - height[5];
        result[1] += (height[4] - height[5]) * (height[4] - height[5]);
    }
};

void Evaluator::connection(Field previous, Placement placement, Puyo puyo[2], int result[3])
{
    result[0] = 0;
    result[1] = 0;
    result[2] = 0;

    Puyo pair[2] = { puyo[0], puyo[1] };
    if (placement.rotation == Rotation::DOWN) {
        pair[0] = puyo[1];
        pair[1] = puyo[0];
    }

    int height[6];
    previous.get_height(height);

    result[0] += previous.get_puyo(placement.x - 1, height[placement.x]) == pair[0];
    result[0] += previous.get_puyo(placement.x + 1, height[placement.x]) == pair[0];
    result[0] += previous.get_puyo(placement.x, height[placement.x] - 1) == pair[0];
    // result[1] += previous.get_puyo(placement.x - 1, height[placement.x]) == pair[0];
    // result[1] += previous.get_puyo(placement.x + 1, height[placement.x]) == pair[0];
    result[1] += previous.get_puyo(placement.x - 1, height[placement.x]) == pair[0] && height[placement.x] < 4;
    result[1] += previous.get_puyo(placement.x + 1, height[placement.x]) == pair[0] && height[placement.x] < 4;
    result[2] += previous.get_puyo(placement.x, height[placement.x] - 1) == pair[0] && (placement.x == 0 || placement.x == 5) && height[placement.x] > 0;

    previous.set_puyo(placement.x, height[placement.x], pair[0]);
    ++height[placement.x];
    
    int second_puyo_x = placement.x;
    if (placement.rotation == Rotation::RIGHT) {
        ++second_puyo_x;
    }
    else if (placement.rotation == Rotation::LEFT) {
        --second_puyo_x;
    }

    result[0] += previous.get_puyo(second_puyo_x - 1, height[second_puyo_x]) == pair[1];
    result[0] += previous.get_puyo(second_puyo_x + 1, height[second_puyo_x]) == pair[1];
    result[0] += previous.get_puyo(second_puyo_x, height[second_puyo_x] - 1) == pair[1];
    // result[1] += previous.get_puyo(second_puyo_x - 1, height[second_puyo_x]) == pair[1];
    // result[1] += previous.get_puyo(second_puyo_x + 1, height[second_puyo_x]) == pair[1];
    result[1] += previous.get_puyo(second_puyo_x - 1, height[second_puyo_x]) == pair[1] && height[second_puyo_x] < 4;
    result[1] += previous.get_puyo(second_puyo_x + 1, height[second_puyo_x]) == pair[1] && height[second_puyo_x] < 4;
    result[2] += previous.get_puyo(second_puyo_x, height[second_puyo_x] - 1) == pair[1] && (second_puyo_x == 0 || second_puyo_x == 5) && height[second_puyo_x] > 0;
};

};