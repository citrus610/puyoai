#pragma once

#include "def.h"

struct FieldMono;

class Slice
{
public:
    uint16_t data[7] = { 0 };
    uint16_t size;
public:
    void from(int16_t column);
    void erase(int idx);
};

class FieldMonoSlice
{
public:
    Slice column[6] = { 0 };
public:
    void from(FieldMono& field_mono);
    void find_connect_slice(int x, int idx, FieldMono& mask);
    bool find_poppable_mask(FieldMono& mask);
};