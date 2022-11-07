#include "eval.h"

i32 Evaluator::evaluate(Field& field, std::optional<Detect::Score> detect, u8 frame, Weight& w)
{
    i32 result = 0;

    u8 height[6];
    field.get_height(height);

    i32 link_ver = 0;
    i32 link_hor = 0;
    Evaluator::link(field, link_ver, link_hor);
    result += link_ver * w.link_ver;
    result += link_hor * w.link_hor;

    i32 d_height = 0;
    i32 d_height_sq = 0;
    Evaluator::d_height(height, d_height, d_height_sq);
    result += d_height * w.d_height;
    result += d_height_sq * w.d_height_sq;

    i32 symm = 0;
    i32 symm_sq = 0;
    Evaluator::symm(height, symm, symm_sq);
    result += symm * w.symm;
    result += symm_sq * w.symm_sq;

    i32 shape_u = 0;
    i32 shape_u_sq = 0;
    Evaluator::shape_u(height, shape_u, shape_u_sq);
    result += shape_u * w.shape_u;
    result += shape_u_sq * w.shape_u_sq;

    if (detect.has_value()) {
        result += (detect.value().score >> 8) * w.ptnl_chain_score;
        result += detect.value().chain * w.ptnl_chain_count;
        result += detect.value().added * w.ptnl_chain_added;
        result += frame * w.ptnl_chain_frame;
        result += detect.value().height * w.ptnl_chain_height;
    }

    return result;
};

void Evaluator::link(Field& field, i32& link_ver, i32& link_hor)
{
    link_ver = 0;
    link_hor = 0;

    for (u8 p = 0; p < Cell::COUNT - 1; ++p) {
        link_ver += (FieldBit::shift_down(field.data[p]) & field.data[p]).get_count();
        link_hor += (FieldBit::shift_left(field.data[p]) & field.data[p]).get_count();
    }
};

void Evaluator::d_height(u8 height[6], i32& d_height, i32& d_height_sq)
{
    i32 max_height = *std::max_element(height, height + 6);
    i32 min_height = *std::min_element(height, height + 6);
    d_height = max_height - min_height;
    d_height_sq = d_height * d_height * d_height;
};

void Evaluator::symm(u8 height[6], i32& symm, i32& symm_sq)
{
    i32 value[3] = {
        std::abs(height[0] - height[5]),
        std::abs(height[1] - height[4]),
        std::abs(height[2] - height[3])
    };

    symm = value[0] + value[1] + value[2];
    symm_sq = value[0] * value[0] + value[1] * value[1] + value[2] * value[2];
};

void Evaluator::shape_u(u8 height[6], i32& shape_u, i32& shape_u_sq)
{
    shape_u = 0;
    shape_u_sq = 0;

    for (i32 i = 0; i < 2; ++i) {
        if (height[i] < height[i + 1]) {
            i32 value = height[i + 1] - height[i];
            shape_u += value;
            shape_u_sq += value * value;
        }
    }

    for (i32 i = 3; i < 5; ++i) {
        if (height[i] > height[i + 1]) {
            i32 value = height[i] - height[i + 1];
            shape_u += value;
            shape_u_sq += value * value;
        }
    }
};