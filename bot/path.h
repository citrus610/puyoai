#pragma once

#include "gen.h"

namespace LTPuyo
{

enum class Move : uint8_t
{
    MOVE_RIGHT,
    MOVE_LEFT,
    MOVE_CW,
    MOVE_CCW,
    MOVE_180,
    MOVE_DROP,
    MOVE_NONE
};

typedef avec<Move, 32> MoveQueue;

class PathPositionMap
{
public:
    MoveQueue data[6][5][4];
public:
    PathPositionMap();
public:
    void clear();
public:
    MoveQueue get(int x, int y, Rotation rotation);
    void set(int x, int y, Rotation rotation, MoveQueue value);
};

class PathPlacementMap
{
public:
    MoveQueue data[6][4];
public:
    PathPlacementMap();
public:
    void clear();
public:
    MoveQueue get(int x, Rotation rotation);
    void set(int x, Rotation rotation, MoveQueue value);
};

struct PathFinderNode
{
    Position position;
    MoveQueue path;
};

class PathFinder
{
public:
    static MoveQueue find(Field& field, Placement placement, Pair pair);
    static void expand(Field& field, int height[6], PathFinderNode& node, std::vector<PathFinderNode>& queue, PathPositionMap& queue_map);
    static void lock(PathFinderNode& node, PathPlacementMap& locks_map, bool equal_pair);
};

};