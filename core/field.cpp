#include "field.h"

bool Field::operator == (const Field& other)
{
    for (u8 i = 0; i < Cell::COUNT; ++i) {
        if (this->data[i] != other.data[i]) {
            return false;
        }
    }
    return true;
};

bool Field::operator != (const Field& other)
{
    return !(*this == other);
};

void Field::set_cell(i8 x, i8 y, Cell::Type cell)
{
    assert(x >= 0 && x < 6);
    assert(y >= 0 && y < 13);
    this->data[static_cast<u8>(cell)].cols[x] |= 1 << y;
};

u32 Field::get_count()
{
    u8 height[6];
    this->get_height(height);
    return height[0] + height[1] + height[2] + height[3] + height[4] + height[5];
};

u32 Field::get_count(u8 height[6])
{
    return height[0] + height[1] + height[2] + height[3] + height[4] + height[5];
};

Cell::Type Field::get_cell(i8 x, i8 y)
{
    if (x < 0 || x > 5 || y < 0 || y > 12) {
        return Cell::Type::NONE;
    }
    for (u8 i = 0; i < Cell::COUNT; ++i) {
        if ((this->data[i].cols[x] >> y) & 1) {
            return Cell::Type(i);
        }
    }
    return Cell::Type::NONE;
};

u8 Field::get_height(i8 x)
{
    assert(x >= 0 && x < 6);
    uint16_t column_or = this->data[0].cols[x];
    for (u8 i = 1; i < Cell::COUNT; ++i) {
        column_or |= this->data[i].cols[x];
    }
    return 16 - std::countl_zero(column_or);
};

u8 Field::get_height_max()
{
    u8 result = 0;
    for (int i = 0; i < 6; ++i) {
        result = std::max(result, this->get_height(i));
    }
    return result;
};

void Field::get_height(u8 height[6])
{
    for (int i = 0; i < 6; ++i) {
        height[i] = this->get_height(i);
    }
};

FieldBit Field::get_pop_mask()
{
    FieldBit result;
    for (u8 i = 0; i < Cell::COUNT - 1; ++i) {
        result = result | this->data[i].get_pop_mask();
    }
    return result;
};

FieldBit Field::get_pop_mask(Chain::Link& link)
{
    FieldBit result;
    for (u8 i = 0; i < Cell::COUNT - 1; ++i) {
        FieldBit mask = this->data[i].get_pop_mask();
        result = result | mask;
        link.count[i] = mask.get_count();
    }
    return result;
};

u8 Field::get_drop_pair_frame(i8 x, Direction::Type direction)
{
    return 1 + (this->get_height(x) != this->get_height(x + Direction::get_offset_x(direction)));
};

bool Field::is_occupied(i8 x, i8 y)
{
    if (x < 0 || x > 5 || y < 0) {
        return true;
    }
    return y < this->get_height(x);
};

bool Field::is_occupied(i8 x, i8 y, u8 height[6])
{
    if (x < 0 || x > 5 || y < 0) {
        return true;
    }
    return y < height[x];
};

bool Field::is_colliding_pair(i8 x, i8 y, Direction::Type direction)
{
    u8 height[6];
    this->get_height(height);
    return this->is_colliding_pair(x, y, direction, height);
};

bool Field::is_colliding_pair(i8 x, i8 y, Direction::Type direction, u8 height[6])
{
    return this->is_occupied(x, y, height) || this->is_occupied(x + Direction::get_offset_x(direction), y + Direction::get_offset_y(direction), height);
};

void Field::drop_puyo(i8 x, Cell::Type cell)
{
    assert(x >= 0 && x < 6);
    u8 height = this->get_height(x);
    if (height < 13) {
        this->set_cell(x, height, cell);
    }
};

void Field::drop_pair(i8 x, Direction::Type direction, Cell::Pair pair)
{
    assert(x >= 0 && x < 6);
    switch (direction)
    {
    case Direction::Type::UP:
        this->drop_puyo(x, pair.first);
        this->drop_puyo(x, pair.second);
        break;
    case Direction::Type::RIGHT:
        this->drop_puyo(x, pair.first);
        this->drop_puyo(x + 1, pair.second);
        break;
    case Direction::Type::DOWN:
        this->drop_puyo(x, pair.second);
        this->drop_puyo(x, pair.first);
        break;
    case Direction::Type::LEFT:
        this->drop_puyo(x, pair.first);
        this->drop_puyo(x - 1, pair.second);
        break;
    }
};

Chain::Data Field::pop()
{
    Chain::Data result;

    while (true)
    {
        Chain::Link link = { 0 };
        FieldBit pop_mask = this->get_pop_mask(link);

        if (link.count[0] + link.count[1] + link.count[2] + link.count[3] == 0) {
            break;
        }

        result.links.add(link);

        pop_mask = pop_mask | (FieldBit::expand(pop_mask) & this->data[static_cast<u8>(Cell::Type::GARBAGE)]);

        for (u8 p = 0; p < Cell::COUNT; ++p) {
            this->data[p].pop(pop_mask);
        }
    }

    return result;
};

void Field::from(const char c[13][7])
{
    for (u8 p = 0; p < Cell::COUNT; ++p) {
        for (i8 x = 0; x < 6; ++x) {
            this->data[p].cols[x] = 0;
        }
    }
    for (i8 y = 0; y < 13; ++y) {
        for (i8 x = 0; x < 6; ++x) {
            if (c[12 - y][x] == '.' || c[12 - y][x] == ' ') {
                continue;
            }
            this->set_cell(x, y, Cell::from_char(c[12 - y][x]));
        }
    }
};

void Field::print()
{
    using namespace std;
    for (i8 y = 12; y >= 0; --y) {
        for (i8 x = 0; x < 6; ++x) {
            cout << Cell::to_char(this->get_cell(x, y));
        }
        cout << "\n";
    }
    cout << endl;
};