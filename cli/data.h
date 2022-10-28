#pragma once

#include <vector>
#include <fstream>
#include <string>
#include <iomanip>
#include "../lib/nlohmann/json.hpp"
using json = nlohmann::json;
using order_json = nlohmann::ordered_json;

#include "../ai/ai.h"

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Weight,
    link_ver,
    link_hor,
    d_height,
    d_height_sq,
    symm,
    symm_sq,
    shape_u,
    shape_u_sq,
    ptnl_chain_count,
    ptnl_chain_score,
    ptnl_chain_added,
    ptnl_chain_height
)