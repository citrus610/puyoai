#include "path.h"

namespace Path
{
    PositionMap::PositionMap()
    {
        this->clear();
    };

    void PositionMap::clear()
    {
        for (int x = 0; x < 6; ++x) {
            for (int y = 0; y < 5; ++y) {
                for (int r = 0; r < 4; ++r) {
                    this->data[x][y][r].clear();
                }
            }
        }
    };

    Queue PositionMap::get(i8 x, i8 y, Direction::Type direction)
    {
        assert(x >= 0 && x <= 5);
        assert(y > 10 && y < 16);
        return this->data[x][y - 11][static_cast<uint8_t>(direction)];
    };

    void PositionMap::set(i8 x, i8 y, Direction::Type direction, Queue value)
    {
        assert(x >= 0 && x <= 5);
        assert(y > 10 && y < 16);
        this->data[x][y - 11][static_cast<uint8_t>(direction)] = value;
    };

    PlacementMap::PlacementMap()
    {
        this->clear();
    };

    void PlacementMap::clear()
    {
        for (int x = 0; x < 6; ++x) {
            for (int r = 0; r < 4; ++r) {
                this->data[x][r].clear();
            }
        }
    };

    Queue PlacementMap::get(u8 x, Direction::Type direction)
    {
        assert(x >= 0 && x <= 5);
        assert(int(direction) >= 0 && int(direction) <= 4);
        return this->data[x][static_cast<uint8_t>(direction)];
    };

    void PlacementMap::set(u8 x, Direction::Type direction, Queue value)
    {
        assert(x >= 0 && x <= 5);
        assert(int(direction) >= 0 && int(direction) <= 4);
        this->data[x][static_cast<uint8_t>(direction)] = value;
    };

    Queue Finder::find(Field& field, Move::Placement placement, Cell::Pair pair)
    {
        if (placement.x == 2 && placement.direction == Direction::Type::UP) {
            Queue drop;
            drop.add(Move::Type::DROP);
            return drop;
        }

        bool equal_pair = pair.first == pair.second;

        u8 height[6];
        field.get_height(height);

        std::vector<Finder::Node> queue;
        PositionMap queue_map = PositionMap();
        PlacementMap locks_map = PlacementMap();

        queue.push_back({{ 2, 11, Direction::Type::UP }, Queue()});
        queue_map.set(2, 11, Direction::Type::UP, Queue());

        while (!queue.empty())
        {
            Finder::Node node = queue.back();
            queue.pop_back();

            if (queue_map.get(node.position.x, node.position.y, node.position.direction).get_size() != node.path.get_size()) {
                continue;
            }

            Finder::expand(field, height, node, queue, queue_map);
            Finder::lock(node, locks_map, equal_pair);
        }

        Move::Position position = { .x = int8_t(placement.x), .y = 0, .direction = placement.direction };
        if (equal_pair) {
            position.normalize();
        }

        Queue result = locks_map.get(position.x, position.direction);
        result.add(Move::Type::DROP);
        return result;
    };

    void Finder::expand(Field& field, u8 height[6], Finder::Node& node, std::vector<Finder::Node>& queue, PositionMap& queue_map)
    {
        Finder::Node right = node;
        if (right.position.move_right(field, height)) {
            if (right.path.get_size() > 0 && right.path[right.path.get_size() - 1] == Move::Type::RIGHT) {
                right.path.add(Move::Type::NONE);
            }
            right.path.add(Move::Type::RIGHT);
            if (queue_map.get(right.position.x, right.position.y, right.position.direction).get_size() == 0 ||
                queue_map.get(right.position.x, right.position.y, right.position.direction).get_size() >= right.path.get_size()) {
                queue.push_back(right);
                queue_map.set(right.position.x, right.position.y, right.position.direction, right.path);
            }
        }

        Finder::Node left = node;
        if (left.position.move_left(field, height)) { 
            if (left.path.get_size() > 0 && left.path[left.path.get_size() - 1] == Move::Type::LEFT) { 
                left.path.add(Move::Type::NONE); 
            } 
            left.path.add(Move::Type::LEFT);
            if (queue_map.get(left.position.x, left.position.y, left.position.direction).get_size() == 0 || 
                queue_map.get(left.position.x, left.position.y, left.position.direction).get_size() >= left.path.get_size()) { 
                queue.push_back(left);
                queue_map.set(left.position.x, left.position.y, left.position.direction, left.path);
            }
        }

        Finder::Node cw = node;
        if (cw.position.move_cw(field, height) && cw.position.y < 13 && cw.position.y + Direction::get_offset_y(cw.position.direction) < 13) {
            if (cw.path.get_size() > 0 && cw.path[cw.path.get_size() - 1] == Move::Type::CW) {
                cw.path.add(Move::Type::NONE); 
            }
            cw.path.add(Move::Type::CW);
            if (queue_map.get(cw.position.x, cw.position.y, cw.position.direction).get_size() == 0 || 
                queue_map.get(cw.position.x, cw.position.y, cw.position.direction).get_size() >= cw.path.get_size()) {
                queue.push_back(cw);
                queue_map.set(cw.position.x, cw.position.y, cw.position.direction, cw.path);
            }
        }

        Finder::Node ccw = node;
        if (ccw.position.move_ccw(field, height) && ccw.position.y < 13 && ccw.position.y + Direction::get_offset_y(cw.position.direction) < 13) {
            if (ccw.path.get_size() > 0 && ccw.path[ccw.path.get_size() - 1] == Move::Type::CCW) { 
                ccw.path.add(Move::Type::NONE); 
            } 
            ccw.path.add(Move::Type::CCW);
            if (queue_map.get(ccw.position.x, ccw.position.y, ccw.position.direction).get_size() == 0 || 
                queue_map.get(ccw.position.x, ccw.position.y, ccw.position.direction).get_size() >= ccw.path.get_size()) {
                queue.push_back(ccw);
                queue_map.set(ccw.position.x, ccw.position.y, ccw.position.direction, ccw.path);
            }
        }

#ifdef ENABLE_MOVE_180
        Finder::Node m180 = node;
        if (m180.position.move_180(field, height) && m180.position.y < 13 && m180.position.y + Direction::get_offset_y(m180.position.direction) < 13) {
            if (m180.path.get_size() > 0 && m180.path[m180.path.get_size() - 1] == Move::Type::M180) { 
                m180.path.add(Move::Type::NONE); 
            } 
            m180.path.add(Move::Type::M180);
            if (queue_map.get(m180.position.x, m180.position.y, m180.position.direction).get_size() == 0 || 
                queue_map.get(m180.position.x, m180.position.y, m180.position.direction).get_size() >= m180.path.get_size()) {
                queue.push_back(m180);
                queue_map.set(m180.position.x, m180.position.y, m180.position.direction, m180.path);
            }
        }
#endif
    };

    void Finder::lock(Finder::Node& node, PlacementMap& locks_map, bool equal_pair)
    {
        Move::Position position = node.position;
        if (equal_pair) {
            position.normalize();
        }

        if (locks_map.get(position.x, position.direction).get_size() == 0 ||
            locks_map.get(position.x, position.direction).get_size() > node.path.get_size()) {
            locks_map.set(position.x, position.direction, node.path);
        }
    };

    bool Finder::above_stack_move(Field& field, Move::Placement placement)
    {
        if (placement.x > 2) {
            for (int x = 2; x < placement.x; ++x) {
                if (field.get_height(x) > 10) {
                    return true;
                }
            }
        }
        else {
            for (int x = 2; x > placement.x; --x) {
                if (field.get_height(x) > 10) {
                    return true;
                }
            }
        }
        return false;
    };
};