#pragma once

#include "def.h"
#include "cell.h"
#include "direction.h"
#include "chain.h"

class FieldBit
{
public:
    u16 cols[6] = { 0 };
public:
    bool operator == (const FieldBit& other);
    bool operator != (const FieldBit& other);
    FieldBit operator | (const FieldBit& other);
    FieldBit operator & (const FieldBit& other);
public:
    static FieldBit shift_up(const FieldBit& fieldbit);
    static FieldBit shift_down(const FieldBit& fieldbit);
    static FieldBit shift_right(const FieldBit& fieldbit);
    static FieldBit shift_left(const FieldBit& fieldbit);
    static FieldBit expand(const FieldBit& fieldbit);
public:
    u32 get_count();
    bool get_bit(i8 x, i8 y);
    FieldBit get_pop_mask();
    u32 get_group_count(i8 x, i8 y);
public:
    void pop(FieldBit& mask);
};
