#pragma once

#include <vector>
#include <fstream>
#include <string>
#include <iomanip>
#include "../lib/nlohmann/json.hpp"
using json = nlohmann::json;
using order_json = nlohmann::ordered_json;

#include "../bot/ai.h"

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(HeuristicAccumulate,
    link,
    link_hor_bottom,
    link_hor_left,
    link_ver_side,
    ugly
)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(HeuristicEvaluation,
    height_third_column,
    height_delta,
    height_delta_sq,
    well,
    well_sq,
    bump,
    bump_sq,
    shape_u,
    shape_u_sq,
    side_bias,
    pattern_middle_y,
    pattern_left_l
)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Heuristic, accumulate, evaluation)