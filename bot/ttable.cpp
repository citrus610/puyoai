#include "ttable.h"

TTable::TTable()
{
    this->bucket = nullptr;
    this->size = 0;
};

TTable::~TTable()
{
    this->destroy();
};

void TTable::init(uint32_t size)
{
    assert((size & (size - 1)) == 0);
    if (this->bucket != nullptr) {
        assert(false);
        return;
    }
    this->size = size;
    this->bucket = new TBucket[this->size];
    this->clear();
};

void TTable::destroy()
{
    if (this->bucket == nullptr) {
        return;
    }
    delete[] this->bucket;
};

void TTable::clear()
{
    memset(this->bucket, 0, sizeof(TBucket) * this->size);
};

uint32_t TTable::hash(Field& field)
{
    return xxhash32((const void*)field.puyo, sizeof(Field), 33);
};

bool TTable::get_entry(uint32_t hash, uint16_t& depth)
{
    uint32_t index = hash & (this->size - 1);
    for (uint32_t i = 0; i < TTABLE_DEFAULT_BUCKET_SIZE; ++i) {
        if (this->bucket[index].slot[i].hash == hash) {
            depth = this->bucket[index].slot[i].depth;
            return true;
        }
    }
    return false;
};

bool TTable::add_entry(uint32_t hash, uint16_t depth)
{
    uint32_t index = hash & (this->size - 1);
    int slot_replace = -1;
    for (uint32_t i = 0; i < TTABLE_DEFAULT_BUCKET_SIZE; ++i) {
        if (this->bucket[index].slot[i].hash == hash) {
            if (this->bucket[index].slot[i] < TEntry { hash, depth }) {
                this->bucket[index].slot[i].depth = depth;
                return true;
            }
            return false;
        }
        if (slot_replace == -1 || this->bucket[index].slot[i] < this->bucket[index].slot[slot_replace]) {
            slot_replace = int(i);
        }
    }
    if (slot_replace != -1) {
        this->bucket[index].slot[slot_replace].hash = hash;
        this->bucket[index].slot[slot_replace].depth = depth;
        return true;
    }
    return false;
};

double TTable::hashful()
{
    uint32_t total = 0;
    for (uint32_t i = 0; i < this->size; ++i) {
        for (uint32_t k = 0; k < TTABLE_DEFAULT_BUCKET_SIZE; ++k) {
            if (this->bucket[i].slot[k].hash != 0 || this->bucket[i].slot[k].depth != 0) {
                total += 1;
            }
        }
    }
    return double(total) / double(this->size * TTABLE_DEFAULT_BUCKET_SIZE);
};