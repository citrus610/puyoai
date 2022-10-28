#pragma once

#include "detect.h"
#include "move.h"
#include "eval.h"
#include "ttable.h"

namespace Search
{
    struct Attack
    {
        u32 score = 0;
        u8 count = 0;
        u8 frame = 0;
        bool all_clear = false;
    };

    struct Candidate
    {
        Move::Placement placement = Move::Placement();
        Field plan = Field();
        i32 eval = INT32_MIN;
        i32 eval_im = INT32_MIN;
        std::vector<Attack> attack;
    };
    
    struct Result
    {
        std::vector<Candidate> candidate;
    };

    struct Score
    {
        i32 eval = INT32_MIN;
        Field plan = Field();
    };

    Result search(Field field, std::vector<Cell::Pair> queue, u32 score_bound, Weight w = DEFAULT_WEIGHT);
    Score dfs(Field& field, std::vector<Cell::Pair>& queue, u32 score_bound, Weight& w, TTable<Score>& ttable, std::vector<Attack>& attack, u8 depth, u8 frame);
};