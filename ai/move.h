#pragma once

#include "../core/core.h"

namespace Move
{
    enum class Type
    {
        LEFT,
        RIGHT,
        CW,
        CCW,
        M180,
        DROP,
        NONE
    };

    struct Placement
    {
        u8 x = 0;
        Direction::Type direction = Direction::Type::UP;
    };

    class Position
    {
    public:
        i8 x = 0;
        i8 y = 0;
        Direction::Type direction = Direction::Type::UP;
    public:
        bool move_right(Field& field, u8 height[6]);
        bool move_left(Field& field, u8 height[6]);
        bool move_cw(Field& field, u8 height[6]);
        bool move_ccw(Field& field, u8 height[6]);
        bool move_180(Field& field, u8 height[6]);
    public:
        bool is_colliding(u8 height[6]);
    public:
        void normalize();
    };
    
    class PositionMap
    {
    public:
        bool data[6][2][4];
    public:
        PositionMap();
    public:
        void clear();
    public:
        bool get(i8 x, i8 y, Direction::Type direction);
        void set(i8 x, i8 y, Direction::Type direction, bool value);
    };

    class PlacementMap
    {
    public:
        bool data[6][4];
    public:
        PlacementMap();
    public:
        void clear();
    public:
        bool get(u8 x, Direction::Type direction);
        void set(u8 x, Direction::Type direction, bool value);
    };

    class Generator
    {
    public:
        static avec<Placement, 22> generate(Field& field, bool equal_pair);
        static void expand(Field& field, u8 height[6], Position& position, avec<Position, 48>& queue, PositionMap& queue_map);
        static void lock(Field& field, u8 height[6], Position& position, avec<Placement, 22>& locks, PlacementMap& locks_map, bool equal_pair);
    public:
        static void bench(Field field, int iter);
        static void print(Field field, Cell::Pair pair);
    };
};