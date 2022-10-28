#pragma once

#include "move.h"

namespace Path
{

    typedef avec<Move::Type, 32> Queue;

    class PositionMap
    {
    public:
        Queue data[6][5][4];
    public:
        PositionMap();
    public:
        void clear();
    public:
        Queue get(i8 x, i8 y, Direction::Type direction);
        void set(i8 x, i8 y, Direction::Type direction, Queue value);
    };

    class PlacementMap
    {
    public:
        Queue data[6][4];
    public:
        PlacementMap();
    public:
        void clear();
    public:
        Queue get(u8 x, Direction::Type direction);
        void set(u8 x, Direction::Type direction, Queue value);
    };

    class Finder
    {
    public:
        struct Node
        {
            Move::Position position;
            Queue path;
        };
    public:
        static Queue find(Field& field, Move::Placement placement, Cell::Pair pair);
        static void expand(Field& field, u8 height[6], Node& node, std::vector<Node>& queue, PositionMap& queue_map);
        static void lock(Node& node, PlacementMap& locks_map, bool equal_pair);
        static bool above_stack_move(Field& field, Move::Placement placement);
    };

};