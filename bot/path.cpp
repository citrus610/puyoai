#include "path.h"

PathPositionMap::PathPositionMap()
{
    this->clear();
};

void PathPositionMap::clear()
{
    for (int x = 0; x < 6; ++x) {
        for (int y = 0; y < 5; ++y) {
            for (int r = 0; r < 4; ++r) {
                this->data[x][y][r].clear();
            }
        }
    }
};

MoveQueue PathPositionMap::get(int x, int y, Rotation rotation)
{
    assert(x >= 0 && x <= 5);
    assert(y > 10 && y < 16);
    return this->data[x][y - 11][static_cast<uint8_t>(rotation)];
};

void PathPositionMap::set(int x, int y, Rotation rotation, MoveQueue value)
{
    assert(x >= 0 && x <= 5);
    assert(y > 10 && y < 16);
    this->data[x][y - 11][static_cast<uint8_t>(rotation)] = value;
};

PathPlacementMap::PathPlacementMap()
{
    this->clear();
};

void PathPlacementMap::clear()
{
    for (int x = 0; x < 6; ++x) {
        for (int r = 0; r < 4; ++r) {
            this->data[x][r].clear();
        }
    }
};

MoveQueue PathPlacementMap::get(int x, Rotation rotation)
{
    assert(x >= 0 && x <= 5);
    assert(int(rotation) >= 0 && int(rotation) <= 4);
    return this->data[x][static_cast<uint8_t>(rotation)];
};

void PathPlacementMap::set(int x, Rotation rotation, MoveQueue value)
{
    assert(x >= 0 && x <= 5);
    assert(int(rotation) >= 0 && int(rotation) <= 4);
    this->data[x][static_cast<uint8_t>(rotation)] = value;
};

MoveQueue PathFinder::find(Field& field, Placement placement, Pair pair)
{
    if (placement.x == 2 && placement.rotation == Rotation::UP) {
        MoveQueue drop;
        drop.add(Move::MOVE_DROP);
        return drop;
    }

    bool equal_pair = pair.first == pair.second;

    int height[6];
    field.get_height(height);

    std::vector<PathFinderNode> queue;
    PathPositionMap queue_map = PathPositionMap();
    PathPlacementMap locks_map = PathPlacementMap();

    queue.push_back({{ 2, 11, Rotation::UP }, MoveQueue()});
    queue_map.set(2, 11, Rotation::UP, MoveQueue());

    while (!queue.empty())
    {
        PathFinderNode node = queue.back();
        queue.pop_back();

        if (queue_map.get(node.position.x, node.position.y, node.position.rotation).get_size() != node.path.get_size()) {
            continue;
        }

        PathFinder::expand(field, height, node, queue, queue_map);
        PathFinder::lock(node, locks_map, equal_pair);
    }

    Position position = { .x = int8_t(placement.x), .y = 0, .rotation = placement.rotation };
    if (equal_pair) {
        position.normalize();
    }

    MoveQueue result = locks_map.get(position.x, position.rotation);
    result.add(Move::MOVE_DROP);
    return result;
};

void PathFinder::expand(Field& field, int height[6], PathFinderNode& node, std::vector<PathFinderNode>& queue, PathPositionMap& queue_map)
{
    PathFinderNode right = node;
    if (right.position.move_right(field, height)) {
        if (right.path.get_size() > 0 && right.path[right.path.get_size() - 1] == Move::MOVE_RIGHT) {
            right.path.add(Move::MOVE_NONE);
        }
        right.path.add(Move::MOVE_RIGHT);
        if (queue_map.get(right.position.x, right.position.y, right.position.rotation).get_size() == 0 ||
            queue_map.get(right.position.x, right.position.y, right.position.rotation).get_size() >= right.path.get_size()) {
            queue.push_back(right);
            queue_map.set(right.position.x, right.position.y, right.position.rotation, right.path);
        }
    }

    PathFinderNode left = node;
    if (left.position.move_left(field, height)) { 
        if (left.path.get_size() > 0 && left.path[left.path.get_size() - 1] == Move::MOVE_LEFT) { 
            left.path.add(Move::MOVE_NONE); 
        } 
        left.path.add(Move::MOVE_LEFT);
        if (queue_map.get(left.position.x, left.position.y, left.position.rotation).get_size() == 0 || 
            queue_map.get(left.position.x, left.position.y, left.position.rotation).get_size() >= left.path.get_size()) { 
            queue.push_back(left);
            queue_map.set(left.position.x, left.position.y, left.position.rotation, left.path);
        }
    }

    PathFinderNode cw = node;
    if (cw.position.move_cw(field, height) && cw.position.y < 13 && cw.position.y + ROTATION_OFFSET[static_cast<uint8_t>(cw.position.rotation)][1] < 13) {
        if (cw.path.get_size() > 0 && cw.path[cw.path.get_size() - 1] == Move::MOVE_CW) {
            cw.path.add(Move::MOVE_NONE); 
        }
        cw.path.add(Move::MOVE_CW);
        if (queue_map.get(cw.position.x, cw.position.y, cw.position.rotation).get_size() == 0 || 
            queue_map.get(cw.position.x, cw.position.y, cw.position.rotation).get_size() >= cw.path.get_size()) {
            queue.push_back(cw);
            queue_map.set(cw.position.x, cw.position.y, cw.position.rotation, cw.path);
        }
    }

    PathFinderNode ccw = node;
    if (ccw.position.move_ccw(field, height) && ccw.position.y < 13 && ccw.position.y + ROTATION_OFFSET[static_cast<uint8_t>(ccw.position.rotation)][1] < 13) {
        if (ccw.path.get_size() > 0 && ccw.path[ccw.path.get_size() - 1] == Move::MOVE_CCW) { 
            ccw.path.add(Move::MOVE_NONE); 
        } 
        ccw.path.add(Move::MOVE_CCW);
        if (queue_map.get(ccw.position.x, ccw.position.y, ccw.position.rotation).get_size() == 0 || 
            queue_map.get(ccw.position.x, ccw.position.y, ccw.position.rotation).get_size() >= ccw.path.get_size()) {
            queue.push_back(ccw);
            queue_map.set(ccw.position.x, ccw.position.y, ccw.position.rotation, ccw.path);
        }
    }

    // PathFinderNode m180 = node;
    // if (m180.position.move_180(field, height) && m180.position.y < 13 && m180.position.y + ROTATION_OFFSET[m180.position.rotation][1] < 13) {
    //     if (m180.path.get_size() > 0 && m180.path[m180.path.get_size() - 1] == Move::MOVE_180) { 
    //         m180.path.add(Move::MOVE_NONE); 
    //     } 
    //     m180.path.add(Move::MOVE_180);
    //     if (queue_map.get(m180.position.x, m180.position.y, m180.position.rotation).get_size() == 0 || 
    //         queue_map.get(m180.position.x, m180.position.y, m180.position.rotation).get_size() >= m180.path.get_size()) {
    //         queue.push_back(m180);
    //         queue_map.set(m180.position.x, m180.position.y, m180.position.rotation, m180.path);
    //     }
    // }
};

void PathFinder::lock(PathFinderNode& node, PathPlacementMap& locks_map, bool equal_pair)
{
    Position position = node.position;
    if (equal_pair) {
        position.normalize();
    }

    if (locks_map.get(position.x, position.rotation).get_size() == 0 ||
        locks_map.get(position.x, position.rotation).get_size() > node.path.get_size()) {
        locks_map.set(position.x, position.rotation, node.path);
    }
};