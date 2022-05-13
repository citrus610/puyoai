#pragma once

#include <vector>
#include <fstream>
#include <string>
#include <iomanip>
#include "../lib/nlohmann/json.hpp"
using json = nlohmann::json;
using order_json = nlohmann::ordered_json;

#include "../bot/decision.h"

namespace LTPuyo
{

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(HeuristicAccumulate,
    connection,
    connection_horizontal,
    disconnection
)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(HeuristicEvaluation,
    bumpiness,
    bumpiness_sq,
    sidewell,
    sidewell_sq
)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Heuristic, accumulate, evaluation)

};