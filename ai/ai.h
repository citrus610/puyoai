#pragma once

#include "path.h"
#include "search.h"

namespace AI
{
    struct Result
    {
        Move::Placement placement;
        Field plan = Field();
        i32 eval = INT32_MIN;
    };

    namespace Data
    {
        struct Enemy
        {
            Field field { 0 };
            std::vector<Cell::Pair> queue;
            u32 incomming_attack = 0;
            u8 incomming_attack_frame = 0;
            bool all_clear = false;
        };

        struct Point
        {
            u32 target_point = 0;
            u32 bonus_point = 0;
            bool all_clear = 0;
        };
    };
    
    Result think_1p(Field field, std::vector<Cell::Pair> queue, u8 harass_bound, u32 score_bound, Weight w = DEFAULT_WEIGHT);
    Result think_2p(Field field, std::vector<Cell::Pair> queue, u8 harass_bound, u32 score_bound, Data::Enemy enemy, Data::Point point, Weight w = DEFAULT_WEIGHT);
    Result decide_build(Search::Result& search_result, u32 field_count);
    bool get_enemy_danger(Data::Enemy& enemy, Data::Point& point, Field& field);
};