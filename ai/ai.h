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
    
    Result think_1p(Field& field, std::vector<Cell::Pair>& queue, u8 harass_bound, u8 score_bound, Weight w = DEFAULT_WEIGHT);
};