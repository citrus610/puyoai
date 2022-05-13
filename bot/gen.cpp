#include "gen.h"

namespace LTPuyo
{

bool Position::move_right(Field& field, int height[6])
{
    if (!field.is_colliding(height, this->x + 1, this->y, this->rotation)) {
        ++this->x;
        return true;
    }
    return false;
};

bool Position::move_left(Field& field, int height[6])
{
    if (!field.is_colliding(height, this->x - 1, this->y, this->rotation)) {
        --this->x;
        return true;
    }
    return false;
};

bool Position::move_cw(Field& field, int height[6])
{
    Rotation new_rotation = Rotation((this->rotation + 1) & 0b11);

    if (!field.is_colliding(height, this->x, this->y, new_rotation)) {
        this->rotation = new_rotation;
        return true;
    }

    if (!field.is_colliding(height, this->x - ROTATION_OFFSET[new_rotation][0], this->y - ROTATION_OFFSET[new_rotation][1], new_rotation)) {
        this->x -= ROTATION_OFFSET[new_rotation][0];
        this->y -= ROTATION_OFFSET[new_rotation][1];
        this->rotation = new_rotation;
        return true;
    }

    return false;
};

bool Position::move_ccw(Field& field, int height[6])
{
    Rotation new_rotation = Rotation((this->rotation + 3) & 0b11);
    
    if (!field.is_colliding(height, this->x, this->y, new_rotation)) {
        this->rotation = new_rotation;
        return true;
    }

    if (!field.is_colliding(height, this->x - ROTATION_OFFSET[new_rotation][0], this->y - ROTATION_OFFSET[new_rotation][1], new_rotation)) {
        this->x -= ROTATION_OFFSET[new_rotation][0];
        this->y -= ROTATION_OFFSET[new_rotation][1];
        this->rotation = new_rotation;
        return true;
    }
    
    return false;
};

bool Position::move_180(Field& field, int height[6])
{
    if (this->rotation == Rotation::RIGHT ||
        this->rotation == Rotation::LEFT ||
        !field.is_occupied(height, this->x - 1, this->y) || 
        !field.is_occupied(height, this->x + 1, this->y)) {
        return false;
    }

    Rotation new_rotation = Rotation((this->rotation + 2) & 0b11);

    if (new_rotation == Rotation::DOWN && this->y == height[this->x]) {
        ++this->y;
    }

    this->rotation = new_rotation;
    return true;
};

void Position::normalize()
{
    switch (this->rotation)
    {
    case Rotation::LEFT:
        --this->x;
        this->rotation = Rotation::RIGHT;
        break;
    case Rotation::DOWN:
        --this->y;
        this->rotation = Rotation::UP;
        break;
    default:
        break;
    }
};

PositionMap::PositionMap()
{
    this->clear();
};

void PositionMap::clear()
{
    memset(this->data, false, 6 * 5 * 4 * sizeof(bool));
};

bool PositionMap::get(int x, int y, Rotation rotation)
{
    assert(x >= 0 && x <= 5);
    assert(y > 10 && y < 16);
    return this->data[x][y - 11][rotation];
};

void PositionMap::set(int x, int y, Rotation rotation, bool value)
{
    assert(x >= 0 && x <= 5);
    assert(y > 10 && y < 16);
    this->data[x][y - 11][rotation] = value;
};

PlacementMap::PlacementMap()
{
    this->clear();
};

void PlacementMap::clear()
{
    memset(this->full, false, 6 * 4 * sizeof(bool));
    memset(this->partial, false, 6 * 2 * sizeof(bool));
};

bool PlacementMap::get_full(int x, Rotation rotation)
{
    assert(x >= 0 && x <= 5);
    return this->full[x][rotation];
};

void PlacementMap::set_full(int x, Rotation rotation, bool value)
{
    assert(x >= 0 && x <= 5);
    this->full[x][rotation] = value;
};

bool PlacementMap::get_partial(int x, int idx)
{
    assert(x >= 0 && x <= 5);
    assert(idx >= 0 && idx <= 1);
    return this->partial[x][idx];
};

void PlacementMap::set_partial(int x, int idx, bool value)
{
    assert(x >= 0 && x <= 5);
    assert(idx >= 0 && idx <= 1);
    this->partial[x][idx] = value;
};

void Generator::generate(Field& field, Pair pair, avec<Placement, 22>& placement)
{
    int height[6];
    field.get_height(height);

    if (height[2] > 11) {
        return;
    }

    placement.clear();

    bool fast_mode = *std::max_element(height, height + 6) < 12;
    bool equal_pair = pair.first == pair.second;

    if (fast_mode) {
        for (int8_t x = 0; x < 6; ++x) {
            placement.add({ uint8_t(x), Rotation::UP });
        }

        for (int8_t x = 0; x  < 5; ++x) {
            placement.add({ uint8_t(x), Rotation::RIGHT });
        }

        if (pair.first == pair.second) {
            return;
        }

        for (int8_t x = 0; x < 6; ++x) {
            placement.add({ uint8_t(x), Rotation::DOWN });
        }

        for (int8_t x = 1; x < 6; ++x) {
            placement.add({ uint8_t(x), Rotation::LEFT });
        }
    }
    else {
        avec<Position, 120> queue = avec<Position, 120>();
        PositionMap queue_map = PositionMap();
        PlacementMap locks_map = PlacementMap();

        queue.add({ 2, 11, Rotation::UP });
        queue_map.set(2, 11, Rotation::UP, true);

        while (queue.get_size() > 0)
        {
            Position position = queue[queue.get_size() - 1];
            queue.pop();

            Generator::expand(field, height, position, queue, queue_map);
            Generator::lock(field, height, position, placement, locks_map, equal_pair);
        }
    }
};

void Generator::expand(Field& field, int height[6], Position& position, avec<Position, 120>& queue, PositionMap& queue_map)
{
    Position right = position;
    if (right.move_right(field, height) && !queue_map.get(right.x, right.y, right.rotation)) {
        queue.add(right);
        queue_map.set(right.x, right.y, right.rotation, true);
    }

    Position left = position;
    if (left.move_left(field, height) && !queue_map.get(left.x, left.y, left.rotation)) {
        queue.add(left);
        queue_map.set(left.x, left.y, left.rotation, true);
    }

    Position cw = position;
    if (cw.move_cw(field, height) && !queue_map.get(cw.x, cw.y, cw.rotation)) {
        queue.add(cw);
        queue_map.set(cw.x, cw.y, cw.rotation, true);
    }

    Position ccw = position;
    if (ccw.move_ccw(field, height) && !queue_map.get(ccw.x, ccw.y, ccw.rotation)) {
        queue.add(ccw);
        queue_map.set(ccw.x, ccw.y, ccw.rotation, true);
    }

    Position m180 = position;
    if (m180.move_180(field, height) && !queue_map.get(m180.x, m180.y, m180.rotation)) {
        queue.add(m180);
        queue_map.set(m180.x, m180.y, m180.rotation, true);
    }
};

void Generator::lock(Field& field, int height[6], Position& position, avec<Placement, 22>& locks, PlacementMap& locks_map, bool equal_pair)
{
    if (equal_pair) {
        position.normalize();
    }
    
    int y[2] = { 0, 0 };

    switch (position.rotation)
    {
    case Rotation::UP:
        y[0] = height[position.x];
        y[1] = height[position.x] + 1;
        break;
    case Rotation::RIGHT:
        y[0] = height[position.x];
        y[1] = height[position.x + 1];
        break;
    case Rotation::DOWN:
        y[0] = height[position.x] + 1;
        y[1] = height[position.x];
        break;
    case Rotation::LEFT:
        y[0] = height[position.x];
        y[1] = height[position.x - 1];
        break;
    }

    if (y[0] > 12 && y[1] > 12) {
        return;
    }

    if (y[0] < 13 && y[1] < 13) {
        if (!locks_map.get_full(position.x, position.rotation)) {
            locks.add({ uint8_t(position.x), position.rotation });
            locks_map.set_full(position.x, position.rotation, true);
        }
        return;
    }

    if (y[0] < 13) {
        if (!locks_map.get_partial(position.x, 0)) {
            locks.add({ uint8_t(position.x), position.rotation });
            locks_map.set_partial(position.x, 0, true);
        }
        return;
    }

    if (y[1] < 13) {
        if (!locks_map.get_partial(position.x, 1)) {
            locks.add({ uint8_t(position.x), position.rotation });
            locks_map.set_partial(position.x, 1, true);
        }
        return;
    }
};

};