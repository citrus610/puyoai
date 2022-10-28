#include "move.h"

namespace Move
{
    bool Position::move_right(Field& field, u8 height[6])
    {
        if (!field.is_colliding_pair(this->x + 1, this->y, this->direction, height)) {
            ++this->x;
            return true;
        }
        return false;
    };

    bool Position::move_left(Field& field, u8 height[6])
    {
        if (!field.is_colliding_pair(this->x - 1, this->y, this->direction, height)) {
            --this->x;
            return true;
        }
        return false;
    };

    bool Position::move_cw(Field& field, u8 height[6])
    {
        Direction::Type new_direction = Direction::Type((static_cast<u8>(this->direction) + 1) & 0b11);

        if (!field.is_colliding_pair(this->x, this->y, new_direction, height)) {
            this->direction = new_direction;
            return true;
        }

        if (!field.is_colliding_pair(this->x - Direction::get_offset_x(new_direction), this->y - Direction::get_offset_y(new_direction), new_direction, height)) {
            this->x -= Direction::get_offset_x(new_direction);
            this->y -= Direction::get_offset_y(new_direction);
            this->direction = new_direction;
            return true;
        }

        return false;
    };

    bool Position::move_ccw(Field& field, u8 height[6])
    {
        Direction::Type new_direction = Direction::Type((static_cast<u8>(this->direction) + 3) & 0b11);
        
        if (!field.is_colliding_pair(this->x, this->y, new_direction, height)) {
            this->direction = new_direction;
            return true;
        }

        if (!field.is_colliding_pair(this->x - Direction::get_offset_x(new_direction), this->y - Direction::get_offset_y(new_direction), new_direction, height)) {
            this->x -= Direction::get_offset_x(new_direction);
            this->y -= Direction::get_offset_y(new_direction);
            this->direction = new_direction;
            return true;
        }
        
        return false;
    };

    bool Position::move_180(Field& field, u8 height[6])
    {
        if (this->direction == Direction::Type::RIGHT ||
            this->direction == Direction::Type::LEFT ||
            !field.is_occupied(this->x - 1, this->y, height) || 
            !field.is_occupied(this->x + 1, this->y, height)) {
            return false;
        }

        u8 new_direction = (static_cast<u8>(this->direction) + 2) & 0b11;
        this->y += new_direction - 1;
        this->direction = Direction::Type(new_direction);

        return true;
    };

    void Position::normalize()
    {
        switch (this->direction)
        {
        case Direction::Type::LEFT:
            --this->x;
            this->direction = Direction::Type::RIGHT;
            break;
        case Direction::Type::DOWN:
            --this->y;
            this->direction = Direction::Type::UP;
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
        memset(this->data, false, 6 * 2 * 4 * sizeof(bool));
    };

    bool PositionMap::get(i8 x, i8 y, Direction::Type direction)
    {
        assert(x >= 0 && x <= 5);
        assert(y > 10 && y < 16);
        return this->data[x][y - 11][static_cast<u8>(direction)];
    };

    void PositionMap::set(i8 x, i8 y, Direction::Type direction, bool value)
    {
        assert(x >= 0 && x <= 5);
        assert(y > 10 && y < 16);
        this->data[x][y - 11][static_cast<u8>(direction)] = value;
    };

    PlacementMap::PlacementMap()
    {
        this->clear();
    };

    void PlacementMap::clear()
    {
        memset(this->data, false, 6 * 4 * sizeof(bool));
    };

    bool PlacementMap::get(u8 x, Direction::Type direction)
    {
        assert(x <= 5);
        return this->data[x][static_cast<u8>(direction)];
    };

    void PlacementMap::set(u8 x, Direction::Type direction, bool value)
    {
        assert(x <= 5);
        this->data[x][static_cast<u8>(direction)] = value;
    };

    avec<Placement, 22> Generator::generate(Field& field, bool equal_pair)
    {
        avec<Placement, 22> placement;

        u8 height[6];
        field.get_height(height);

        if (height[2] > 11) {
            return avec<Placement, 22>();
        }

        if (*std::max_element(height, height + 6) < 12) {
            for (u8 x = 0; x < 6; ++x) {
                placement.add({ x, Direction::Type::UP });
            }

            for (u8 x = 0; x < 5; ++x) {
                placement.add({ x, Direction::Type::RIGHT });
            }

            if (equal_pair) {
                return placement;
            }

            for (u8 x = 0; x < 6; ++x) {
                placement.add({ x, Direction::Type::DOWN });
            }

            for (u8 x = 1; x < 6; ++x) {
                placement.add({ x, Direction::Type::LEFT });
            }
        }
        else {
            avec<Position, 48> queue = avec<Position, 48>();
            PositionMap queue_map = PositionMap();
            PlacementMap locks_map = PlacementMap();

            queue.add({ 2, 11, Direction::Type::UP });
            queue_map.set(2, 11, Direction::Type::UP, true);

            while (queue.get_size() > 0)
            {
                Position position = queue[queue.get_size() - 1];
                queue.pop();

                Generator::expand(field, height, position, queue, queue_map);
                Generator::lock(field, height, position, placement, locks_map, equal_pair);
            }
        }

        return placement;
    };

    void Generator::expand(Field& field, u8 height[6], Position& position, avec<Position, 48>& queue, PositionMap& queue_map)
    {
        Position right = position;
        if (right.move_right(field, height) && !queue_map.get(right.x, right.y, right.direction)) {
            queue.add(right);
            queue_map.set(right.x, right.y, right.direction, true);
        }

        Position left = position;
        if (left.move_left(field, height) && !queue_map.get(left.x, left.y, left.direction)) {
            queue.add(left);
            queue_map.set(left.x, left.y, left.direction, true);
        }

        Position cw = position;
        if (cw.move_cw(field, height) && !queue_map.get(cw.x, cw.y, cw.direction) && cw.y < 13 && cw.y + Direction::get_offset_y(cw.direction) < 13) {
            queue.add(cw);
            queue_map.set(cw.x, cw.y, cw.direction, true);
        }

        Position ccw = position;
        if (ccw.move_ccw(field, height) && !queue_map.get(ccw.x, ccw.y, ccw.direction) && ccw.y < 13 && ccw.y + Direction::get_offset_y(ccw.direction) < 13) {
            queue.add(ccw);
            queue_map.set(ccw.x, ccw.y, ccw.direction, true);
        }

#ifdef ENABLE_MOVE_180
        Position m180 = position;
        if (m180.move_180(field, height) && !queue_map.get(m180.x, m180.y, m180.direction) && m180.y < 13 && m180.y + Direction::get_offset_y(m180.direction) < 13) {
            queue.add(m180);
            queue_map.set(m180.x, m180.y, m180.direction, true);
        }
#endif
    };

    void Generator::lock(Field& field, u8 height[6], Position& position, avec<Placement, 22>& locks, PlacementMap& locks_map, bool equal_pair)
    {
        if (equal_pair) {
            position.normalize();
        }

        if (!locks_map.get(position.x, position.direction)) {
            locks.add({ u8(position.x), position.direction });
            locks_map.set(position.x, position.direction, true);
        }
    };

    void Generator::bench(Field field, int iter)
    {
        using namespace std;

        i64 time = 0;

        for (int i = 0; i < iter; ++i) {
            Field copy = field;
            avec<Placement, 22> placement;

            auto time_1 = chrono::high_resolution_clock::now();
            placement = Generator::generate(copy, Cell::Type::RED == Cell::Type::BLUE);
            auto time_2 = chrono::high_resolution_clock::now();

            time += chrono::duration_cast<chrono::nanoseconds>(time_2 - time_1).count();
        }

        cout << "time: " << (time / iter) << " ns" << endl;
    };

    void Generator::print(Field field, Cell::Pair pair)
    {
        using namespace std;

        avec<Placement, 22> placement = Generator::generate(field, pair.first == pair.second);

        for (int i = 0; i < placement.get_size(); ++i) {
            Field copy = field;
            copy.drop_pair(placement[i].x, placement[i].direction, pair);
            copy.print();
            cin.get();
            system("cls");
        }
    };
};