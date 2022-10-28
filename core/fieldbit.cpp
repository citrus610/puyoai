#include "fieldbit.h"

bool FieldBit::operator == (const FieldBit& other)
{
    for (int i = 0; i < 6; ++i) {
        if (this->cols[i] != other.cols[i]) {
            return false;
        }
    }
    return true;
};

bool FieldBit::operator != (const FieldBit& other)
{
    return !(*this == other);
};

FieldBit FieldBit::operator | (const FieldBit& other)
{
    FieldBit result;
    for (int x = 0; x < 6; ++x) {
        result.cols[x] = this->cols[x] | other.cols[x];
    }
    return result;
};

FieldBit FieldBit::operator & (const FieldBit& other)
{
    FieldBit result;
    for (int x = 0; x < 6; ++x) {
        result.cols[x] = this->cols[x] & other.cols[x];
    }
    return result;
};

FieldBit FieldBit::shift_up(const FieldBit& fieldbit)
{
    FieldBit result;
    for (int x = 0; x < 6; ++x) {
        result.cols[x] = fieldbit.cols[x] << 1;
    }
    return result;
};

FieldBit FieldBit::shift_down(const FieldBit& fieldbit)
{
    FieldBit result;
    for (int x = 0; x < 6; ++x) {
        result.cols[x] = fieldbit.cols[x] >> 1;
    }
    return result;
};

FieldBit FieldBit::shift_right(const FieldBit& fieldbit)
{
    FieldBit result;
    for (int i = 5; i > 0; --i) {
        result.cols[i] = fieldbit.cols[i - 1];
    }
    result.cols[0] = 0;
    return result;
};

FieldBit FieldBit::shift_left(const FieldBit& fieldbit)
{
    FieldBit result;
    for (int i = 0; i < 5; ++i) {
        result.cols[i] = fieldbit.cols[i + 1];
    }
    result.cols[5] = 0;
    return result;
};

FieldBit FieldBit::expand(const FieldBit& fieldbit)
{
    return FieldBit::shift_up(fieldbit) | FieldBit::shift_down(fieldbit) | FieldBit::shift_right(fieldbit) | FieldBit::shift_left(fieldbit) | fieldbit;
};

u32 FieldBit::get_count()
{
    u32 result = 0;
    for (int i = 0; i < 6; ++i) {
        result += std::popcount(this->cols[i]);
    }
    return result;
};

bool FieldBit::get_bit(i8 x, i8 y)
{
    if (x < 0 || x > 5 || y < 0 || y > 12) {
        return false;
    }
    return this->cols[x] & (1U << y);
};

FieldBit FieldBit::get_pop_mask()
{
    FieldBit org = *this;
    
    for (int i = 0; i < 6; ++i) {
        org.cols[i] &= (1 << 12) - 1;
    }

    FieldBit shift_up = FieldBit::shift_up(org) & org;
    FieldBit shift_down = FieldBit::shift_down(org) & org;
    FieldBit shift_right = FieldBit::shift_right(org) & org;
    FieldBit shift_left = FieldBit::shift_left(org) & org;

    FieldBit u_and_d = shift_up & shift_down;
    FieldBit l_and_r = shift_right & shift_left;
    FieldBit u_or_d = shift_up | shift_down;
    FieldBit l_or_r = shift_right | shift_left;

    FieldBit link_3 = (u_and_d & l_or_r) | (l_and_r & u_or_d);
    FieldBit link_2 = u_and_d | l_and_r | (u_or_d & l_or_r);
    FieldBit link_2_up = FieldBit::shift_up(link_2) & link_2;
    FieldBit link_2_down = FieldBit::shift_down(link_2) & link_2;
    FieldBit link_2_right = FieldBit::shift_right(link_2) & link_2;
    FieldBit link_2_left = FieldBit::shift_left(link_2) & link_2;

    return FieldBit::expand(link_3 | link_2_up | link_2_down | link_2_right | link_2_left) & org;
};

u32 FieldBit::get_group_count(i8 x, i8 y)
{
    FieldBit org = *this;
    for (int i = 0; i < 6; ++i) {
        org.cols[i] &= (1 << 12) - 1;
    }

    FieldBit a;
    if (y == 0)
        a.cols[x] = 0b11;
    else
        a.cols[x] = 0b111 << (y - 1);
    if (x > 0) a.cols[x - 1] = 1U << y;
    if (x < 5) a.cols[x + 1] = 1U << y;

    FieldBit b = a & org;
    if (x > 0) b.cols[x - 1] |= b.cols[x];
    if (x < 5) b.cols[x + 1] |= b.cols[x];
    if (x > 1) b.cols[x - 2] |= b.cols[x - 1];
    if (x < 4) b.cols[x + 2] |= b.cols[x + 1];
    b.cols[x] |= (b.cols[x] << 1) | (b.cols[x] >> 1);
    if (x > 0) b.cols[x - 1] |= (b.cols[x - 1] << 1) | (b.cols[x - 1] >> 1);
    if (x < 5) b.cols[x + 1] |= (b.cols[x + 1] << 1) | (b.cols[x + 1] >> 1);

    FieldBit c = b & org;
    if (x > 0) c.cols[x - 1] |= c.cols[x];
    if (x < 5) c.cols[x + 1] |= c.cols[x];
    if (x > 1) c.cols[x - 2] |= c.cols[x - 1];
    if (x < 4) c.cols[x + 2] |= c.cols[x + 1];
    if (x > 2) c.cols[x - 3] |= c.cols[x - 2];
    if (x < 3) c.cols[x + 3] |= c.cols[x + 2];
    c.cols[x] |= (c.cols[x] << 1) | (c.cols[x] >> 1);
    if (x > 0) c.cols[x - 1] |= (c.cols[x - 1] << 1) | (c.cols[x - 1] >> 1);
    if (x < 5) c.cols[x + 1] |= (c.cols[x + 1] << 1) | (c.cols[x + 1] >> 1);
    if (x > 1) c.cols[x - 2] |= (c.cols[x - 2] << 1) | (c.cols[x - 2] >> 1);
    if (x < 4) c.cols[x + 2] |= (c.cols[x + 2] << 1) | (c.cols[x + 2] >> 1);

    FieldBit d = c & org;
    return d.get_count();
};

void FieldBit::pop(FieldBit& mask)
{
    for (int x = 0; x < 6; ++x) {
        this->cols[x] = pext16(this->cols[x], ~mask.cols[x]);
    }
};