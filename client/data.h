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
    connection,
    connection_horizontal,
    connection_vertical_side
)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(HeuristicEvaluation,
    height_third_column,
    well,
    well_sq,
    bump,
    bump_sq,
    shape_u,
    shape_u_sq,
    side_bias
)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Heuristic, accumulate, evaluation)