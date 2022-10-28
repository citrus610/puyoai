#pragma once

#include "fieldbit.h"

class Field
{
public:
    FieldBit data[Cell::COUNT] = { 0 };
public:
    bool operator == (const Field& other);
    bool operator != (const Field& other);
public:
    void set_cell(i8 x, i8 y, Cell::Type cell);
public:
    u32 get_count();
    u32 get_count(u8 height[6]);
    Cell::Type get_cell(i8 x, i8 y);
    u8 get_height(i8 x);
    u8 get_height_max();
    void get_height(u8 height[6]);
    FieldBit get_pop_mask();
    FieldBit get_pop_mask(Chain::Link& link);
    u8 get_drop_pair_frame(i8 x, Direction::Type direction);
public:
    bool is_occupied(i8 x, i8 y);
    bool is_occupied(i8 x, i8 y, u8 height[6]);
    bool is_colliding_pair(i8 x, i8 y, Direction::Type direction);
    bool is_colliding_pair(i8 x, i8 y, Direction::Type direction, u8 height[6]);
public:
    void drop_puyo(i8 x, Cell::Type cell);
    void drop_pair(i8 x, Direction::Type direction, Cell::Pair pair);
public:
    Chain::Data pop();
public:
    void from(const char c[13][7]);
    void print();
};