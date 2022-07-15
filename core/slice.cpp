#include "slice.h"
#include "field.h"

// Slice a u16/i16 
// Ex: 0b0011000111010110
// ->  0b0000000000000110
// ->  0b0000000000010000
// ->  0b0000000111000000
// ->  0b0011000000000000
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

// Slice a one-color field
void FieldMonoSlice::from(FieldMono& field_mono)
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