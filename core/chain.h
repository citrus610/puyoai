#pragma once

#include "def.h"

namespace Chain
{
    struct Link
    {
        u8 count[Cell::COUNT - 1] = { 0 };
    };

    struct Data
    {
        avec<Link, 19> links = avec<Link, 19>();
    };

    constexpr u32 COLOR_BONUS[] = { 0, 0, 3, 6, 12, 24 };
    constexpr u32 GROUP_BONUS[] = { 0, 0, 0, 0, 0, 2, 3, 4, 5, 6, 7, 10 };
    constexpr u32 POWER[] = { 0, 8, 16, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512 };

    constexpr u32 get_score(Data& chain)
    {
        u32 result = 0;

        for (int i = 0; i < chain.links.get_size(); ++i) {
            u32 cleared =
                chain.links[i].count[0] +
                chain.links[i].count[1] +
                chain.links[i].count[2] +
                chain.links[i].count[3];
            u32 color =
                chain.links[i].count[0] > 0 +
                chain.links[i].count[1] > 0 +
                chain.links[i].count[2] > 0 +
                chain.links[i].count[3] > 0;
            u32 color_bonus = COLOR_BONUS[color];
            u32 group_bonus =
                GROUP_BONUS[std::min(chain.links[i].count[0], u8(11))] +
                GROUP_BONUS[std::min(chain.links[i].count[1], u8(11))] +
                GROUP_BONUS[std::min(chain.links[i].count[2], u8(11))] +
                GROUP_BONUS[std::min(chain.links[i].count[3], u8(11))];
            u32 power = POWER[i];
            result += 10 * cleared * std::clamp(color_bonus + group_bonus + power, 1U, 999U);
        }

        return result;
    };
};