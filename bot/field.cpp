#include "field.h"

namespace LTPuyo
{

int FieldMono::popcount(int x)
{
    assert(x >= 0 && x < 6);
    return std::popcount(this->column[x]);
};

int FieldMono::popcount()
{
    int result = 0;
    for (int i = 0; i < 6; ++i) {
        result += std::popcount(this->column[i]);
    }
    return result;
};

void FieldMono::expand(FieldMono& result)
{
    result = *this;
    for (int i = 0; i < 6; ++i) {
        result.column[i] |= this->column[i] >> 1;
    }
    for (int i = 0; i < 6; ++i) {
        result.column[i] |= this->column[i] << 1;
    }
    for (int i = 0; i < 5; ++i) {
        result.column[i] |= this->column[i + 1];
    }
    for (int i = 1; i < 6; ++i) {
        result.column[i] |= this->column[i - 1];
    }
};

FieldMono FieldMono::operator | (const FieldMono& other)
{
    FieldMono result = *this;
    for (int x = 0; x < 6; ++x) {
        result.column[x] |= other.column[x];
    }
    return result;
}

FieldMono FieldMono::operator & (const FieldMono& other)
{
    FieldMono result = *this;
    for (int x = 0; x < 6; ++x) {
        result.column[x] &= other.column[x];
    }
    return result;
};

bool FieldMono::operator == (const FieldMono& other)
{
    for (int i = 0; i < 6; ++i) {
        if (this->column[i] != other.column[i]) {
            return false;
        }
    }
    return true;
};

bool FieldMono::operator != (const FieldMono& other)
{
    return !(*this == other);
};

void Field::set_puyo(int x, int y, Puyo puyo)
{
    assert(x >= 0 && x < 6);
    assert(y >= 0 && y < 13);
    this->puyo[puyo].column[x] |= 1 << y;
};

Puyo Field::get_puyo(int x, int y)
{
    if (x < 0 || x > 5 || y < 0 || y > 12) {
        return Puyo::NONE;
    }
    for (int i = 0; i < Puyo::COUNT; ++i) {
        if (((this->puyo[i].column[x] >> y) & 1) == 1) {
            return Puyo(i);
        }
    }
    return Puyo::NONE;
};

void Field::get_height(int height[6])
{
    for (int i = 0; i < 6; ++i) {
        height[i] = this->get_height(i);
    }
};

int Field::get_height(int x)
{
    assert(x >= 0 && x < 6);
    uint16_t column_or = this->puyo[0].column[x];
    for (int i = 1; i < Puyo::COUNT; ++i) {
        column_or |= this->puyo[i].column[x];
    }
    return 16 - std::countl_zero(column_or);
};

int Field::get_height_max()
{
    int result = 0;
    for (int i = 0; i < 6; ++i) {
        result = std::max(result, this->get_height(i));
    }
    return result;
};

bool Field::is_occupied(int height[6], int x, int y)
{
    if (x < 0 || x > 5 || y < 0) {
        return true;
    }
    return y < height[x];
};

bool Field::is_colliding(int height[6], int x, int y, Rotation r)
{
    return this->is_occupied(height, x, y) || this->is_occupied(height, x + ROTATION_OFFSET[r][0], y + ROTATION_OFFSET[r][1]);
};

int Field::popcount()
{
    int height[6];
    this->get_height(height);
    return height[0] + height[1] + height[2] + height[3] + height[4] + height[5];
};

void Field::drop_puyo(int x, Puyo puyo)
{
    assert(x >= 0 && x < 6);
    int height = this->get_height(x);
    if (height < 13) {
        this->set_puyo(x, height, puyo);
    }
};

void Field::drop_pair(int x, Puyo puyo[2], Rotation rotation)
{
    assert(x >= 0 && x < 6);
    switch (rotation)
    {
    case Rotation::UP:
        this->drop_puyo(x, puyo[0]);
        this->drop_puyo(x, puyo[1]);
        break;
    case Rotation::RIGHT:
        this->drop_puyo(x, puyo[0]);
        this->drop_puyo(x + 1, puyo[1]);
        break;
    case Rotation::DOWN:
        this->drop_puyo(x, puyo[1]);
        this->drop_puyo(x, puyo[0]);
        break;
    case Rotation::LEFT:
        this->drop_puyo(x, puyo[0]);
        this->drop_puyo(x - 1, puyo[1]);
        break;
    }
};

void Field::drop_pair(int x, Pair pair, Rotation rotation)
{
    assert(x >= 0 && x < 6);
    switch (rotation)
    {
    case Rotation::UP:
        this->drop_puyo(x, pair.first);
        this->drop_puyo(x, pair.second);
        break;
    case Rotation::RIGHT:
        this->drop_puyo(x, pair.first);
        this->drop_puyo(x + 1, pair.second);
        break;
    case Rotation::DOWN:
        this->drop_puyo(x, pair.second);
        this->drop_puyo(x, pair.first);
        break;
    case Rotation::LEFT:
        this->drop_puyo(x, pair.first);
        this->drop_puyo(x - 1, pair.second);
        break;
    }
};

void Field::poppable_mask(FieldMono& mask, int& color)
{
    color = 0;
    for (uint8_t puyo = Puyo::RED; puyo < Puyo::COUNT - 1; ++puyo) {
        FieldMonoSlice field_mono_slice;
        field_mono_slice.from(this->puyo[puyo]);
        bool color_pop = field_mono_slice.find_poppable_mask(mask);
        color += color_pop;
    }
};

void Field::pop(Chain& chain)
{
    FieldMono poppable;
    this->poppable_mask(poppable, chain.color[chain.count]);

    int poppable_count = poppable.popcount();
    if (poppable_count == 0) {
        return;
    }

    chain.link[chain.count] = poppable_count;
    ++chain.count;

    FieldMono poppable_expanded;
    poppable.expand(poppable_expanded);
    FieldMono poppable_garbage = poppable_expanded & this->puyo[Puyo::GARBAGE];
    poppable = poppable | poppable_garbage;

    for (uint8_t puyo = Puyo::RED; puyo < Puyo::COUNT; ++puyo) {
        for (int x = 0; x < 6; ++x) {
            this->puyo[puyo].column[x] = pext16(this->puyo[puyo].column[x], ~poppable.column[x]);
        }
    }

    this->pop(chain);
};

int Field::calculate_point(Chain& chain)
{
    int result = 0;

    for (int i = 0; i < chain.count; ++i) {
        int puyo_cleared = chain.link[i];
        int color_bonus = CHAIN_COLOR_BONUS[chain.color[i]];
        int group_bonus = CHAIN_GROUP_BONUS[puyo_cleared];
        int chain_power = CHAIN_POWER[i + 1];

        int mul_factor = color_bonus + group_bonus + chain_power;
        if (mul_factor < 1) {
            mul_factor = 1;
        }
        else if (mul_factor > 999) {
            mul_factor = 999;
        }

        result += 10 * puyo_cleared * mul_factor;
    }

    return result;
};

int Field::calculate_garbage(Chain& chain, int target_point, bool all_clear)
{
    return Field::calculate_point(chain) / target_point + all_clear * 30;
};

bool Field::operator == (const Field& other)
{
    for (int i = 0; i < Puyo::COUNT; ++i) {
        if (this->puyo[i] != other.puyo[i]) {
            return false;
        }
    }
    return true;
};

bool Field::operator != (const Field& other)
{
    return !(*this == other);
};

void Field::from(const char c[13][7])
{
    for (int p = 0; p < Puyo::COUNT; ++p) {
        for (int x = 0; x < 6; ++x) {
            this->puyo[p].column[x] = 0;
        }
    }
    for (int y = 0; y < 13; ++y) {
        for (int x = 0; x < 6; ++x) {
            if (c[12 - y][x] == '.' || c[12 - y][x] == ' ') {
                continue;
            }
            this->set_puyo(x, y, convert_char_puyo(c[12 - y][x]));
        }
    }
};

void Field::print()
{
    using namespace std;
    for (int y = 12; y >= 0; --y) {
        for (int x = 0; x < 6; ++x) {
            cout << convert_puyo_char(this->get_puyo(x, y));
        }
        cout << "\n";
    }
    cout << endl;
};

void Slice::from(int16_t column)
{
    if (column == 0) {
        return;
    }
    column &= (1 << 12) - 1;
    this->size = 0;
    while (column) {
        int16_t filled = column | (column - 1);
        int16_t masked = (~filled & (filled + 1)) - 1;
        this->data[this->size] = masked & column;
        column ^= this->data[this->size];
        ++this->size;
    }
};

void Slice::erase(int idx)
{
    if (this->size <= 0 || idx < 0 || idx >= this->size) {
        return;
    }
    for (int i = idx; i < this->size - 1; ++i) {
        this->data[i] = this->data[i + 1];
    }
    this->size -= 1;
};

void FieldMonoSlice::from(FieldMono field_mono)
{
    for (int x = 0; x < 6; ++x) {
        this->column[x].from(field_mono.column[x]);
    }
};

void FieldMonoSlice::find_connect_slice(int x, int idx, FieldMono& mask)
{
    assert(x >= 0 && x < 6);
    assert(idx >= 0 && idx < this->column[x].size);

    uint16_t slice = this->column[x].data[idx];
    this->column[x].erase(idx);
    mask.column[x] |= slice;

    if (x > 0) {
        for (int i = this->column[x - 1].size - 1; i >= 0; --i) {
            if ((slice & this->column[x - 1].data[i]) > 0) {
                this->find_connect_slice(x - 1, i, mask);
            }
        }
    }
    if (x < 5) {
        for (int i = this->column[x + 1].size - 1; i >= 0; --i) {
            if ((slice & this->column[x + 1].data[i]) > 0) {
                this->find_connect_slice(x + 1, i, mask);
            }
        }
    }
};

bool FieldMonoSlice::find_poppable_mask(FieldMono& mask)
{
    bool result = false;
    for (int x = 0; x < 6; ++x) {
        while (this->column[x].size > 0)
        {
            FieldMono temp_mask;
            this->find_connect_slice(x, this->column[x].size - 1, temp_mask);
            if (temp_mask.popcount() > 3) {
                mask = mask | temp_mask;
                result = true;
            }
        }
    }
    return result;
};

};