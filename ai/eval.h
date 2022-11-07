#pragma once

#include "../core/core.h"
#include "detect.h"

struct Weight
{
    i32 link_ver = 0;
    i32 link_hor = 0;
    i32 d_height = 0;
    i32 d_height_sq = 0;
    i32 symm = 0;
    i32 symm_sq = 0;
    i32 shape_u = 0;
    i32 shape_u_sq = 0;

    i32 ptnl_chain_count = 0;
    i32 ptnl_chain_score = 0;
    i32 ptnl_chain_added = 0;
    i32 ptnl_chain_frame = 0;
    i32 ptnl_chain_height = 0;
};

class Evaluator
{
public:
    static i32 evaluate(Field& field, std::optional<Detect::Score> detect, u8 frame, Weight& w);
    static void link(Field& field, i32& link_ver, i32& link_hor);
    static void d_height(u8 height[6], i32& d_height, i32& d_height_sq);
    static void symm(u8 height[6], i32& symm, i32& symm_sq);
    static void shape_u(u8 height[6], i32& shape_u, i32& shape_u_sq);
};

constexpr Weight DEFAULT_WEIGHT = {
    .link_ver = 25,
    .link_hor = 25,
    .d_height = -5,
    .d_height_sq = -1,
    .symm = -5,
    .symm_sq = -2,
    .shape_u = -10,
    .shape_u_sq = -10,

    .ptnl_chain_count = 85,
    .ptnl_chain_score = 1,
    .ptnl_chain_added = -100,
    .ptnl_chain_frame = -100,
    .ptnl_chain_height = 100,
};