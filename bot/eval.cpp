#include "eval.h"

namespace LTPuyo
{

void Evaluator::evaluate(Node& node, Node& parent, Placement placement, Puyo puyo[2])
{
    node.score.evaluation = 0;

    int height[6];
    node.field.get_height(height);

    // node.score.evaluation += *std::max_element(height, height + 6) * this->heuristic.evaluation.height;

    node.score.evaluation += Evaluator::bumpiness(height) * this->heuristic.evaluation.bumpiness;
    node.score.evaluation += Evaluator::bumpiness_sq(height) * this->heuristic.evaluation.bumpiness_sq;

    int sidewell[2];
    Evaluator::sidewell(height, sidewell);
    node.score.evaluation += sidewell[0] * this->heuristic.evaluation.sidewell;
    node.score.evaluation += sidewell[1] * this->heuristic.evaluation.sidewell_sq;

    int connection[2];
    Evaluator::connection(parent.field, placement, puyo, connection);
    node.score.accumulate += connection[0] * this->heuristic.accumulate.connection;
    node.score.accumulate += connection[1] * this->heuristic.accumulate.connection;
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
    result += (height[0] - height[1]) * (height[0] - height[1]);
    result += (height[2] - height[3]) * (height[2] - height[3]);
    result += (height[4] - height[5]) * (height[4] - height[5]);
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

void Evaluator::connection(Field previous, Placement placement, Puyo puyo[2], int result[2])
{
    result[0] = 0;
    result[1] = 0;

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
    result[1] += previous.get_puyo(placement.x - 1, height[placement.x]) == pair[0];
    result[1] += previous.get_puyo(placement.x + 1, height[placement.x]) == pair[0];

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
    result[1] += previous.get_puyo(second_puyo_x - 1, height[second_puyo_x]) == pair[1];
    result[1] += previous.get_puyo(second_puyo_x + 1, height[second_puyo_x]) == pair[1];
};

// int Evaluator::detect_shape(Field field, Placement placement, Puyo puyo[2], int height[6], Shape shape)
// {
//     int result = 0;

//     int8_t position[2][2] = { 0 };

//     switch (placement.rotation)
//     {
//     case Rotation::UP:
//         position[0][0] = placement.x;
//         position[0][1] = height[placement.x] - 2;
//         position[1][0] = placement.x;
//         position[1][1] = height[placement.x] - 1;
//         break;
//     case Rotation::RIGHT:
//         position[0][0] = placement.x;
//         position[0][1] = height[placement.x] - 1;
//         position[1][0] = placement.x + 1;
//         position[1][1] = height[placement.x + 1] - 1;
//         break;
//     case Rotation::LEFT:
//         position[0][0] = placement.x;
//         position[0][1] = height[placement.x] - 1;
//         position[1][0] = placement.x - 1;
//         position[1][1] = height[placement.x - 1] - 1;
//         break;
//     case Rotation::DOWN:
//         position[0][0] = placement.x;
//         position[0][1] = height[placement.x] - 1;
//         position[1][0] = placement.x;
//         position[1][1] = height[placement.x] - 2;
//         break;
//     }

//     for (int p = 0; p < 2; ++p) {
//         for (int i = 0; i < 4; ++i) {
//             if (
//                 field.get_puyo(position[p][0] + SHAPE[shape][0][0] - SHAPE[shape][i][0], position[p][1] + SHAPE[shape][0][1] - SHAPE[shape][i][1]) == puyo[p] &&
//                 field.get_puyo(position[p][0] + SHAPE[shape][1][0] - SHAPE[shape][i][0], position[p][1] + SHAPE[shape][1][1] - SHAPE[shape][i][1]) == puyo[p] &&
//                 field.get_puyo(position[p][0] + SHAPE[shape][2][0] - SHAPE[shape][i][0], position[p][1] + SHAPE[shape][2][1] - SHAPE[shape][i][1]) == puyo[p] &&
//                 field.get_puyo(position[p][0] + SHAPE[shape][3][0] - SHAPE[shape][i][0], position[p][1] + SHAPE[shape][3][1] - SHAPE[shape][i][1]) == puyo[p]
//             ) {
//                 return 1;
//             }
//             if (
//                 field.get_puyo(position[p][0] - SHAPE[shape][0][0] + SHAPE[shape][i][0], position[p][1] - SHAPE[shape][0][1] + SHAPE[shape][i][1]) == puyo[p] &&
//                 field.get_puyo(position[p][0] - SHAPE[shape][1][0] + SHAPE[shape][i][0], position[p][1] - SHAPE[shape][1][1] + SHAPE[shape][i][1]) == puyo[p] &&
//                 field.get_puyo(position[p][0] - SHAPE[shape][2][0] + SHAPE[shape][i][0], position[p][1] - SHAPE[shape][2][1] + SHAPE[shape][i][1]) == puyo[p] &&
//                 field.get_puyo(position[p][0] - SHAPE[shape][3][0] + SHAPE[shape][i][0], position[p][1] - SHAPE[shape][3][1] + SHAPE[shape][i][1]) == puyo[p]
//             ) {
//                 return 1;
//             }
//         }
//     }

//     return result;
// };

};