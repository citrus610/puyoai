#pragma once

#include "detect.h"
#include "../lib/parallel_hashmap/phmap.h"

constexpr int TTABLE_SIZE_DEFAUT = 1 << 12;

template <typename T>
class TTable
{
public:
    phmap::flat_hash_map<u32, T>* bucket;
    int size;
public:
    TTable();
    ~TTable();
public:
    void init(int init_size = TTABLE_SIZE_DEFAUT);
    void destroy();
public:
    u32 get_hash(Field& field);
    u32 get_index(u32 hash);
public:
    bool get_entry(u32 hash, T& entry);
    void set_entry(u32 hash, T& entry);
};

template <typename T>
TTable<T>::TTable()
{
    this->bucket = nullptr;
    this->size = 0;
};

template <typename T>
TTable<T>::~TTable()
{
    this->destroy();
};

template <typename T>
void TTable<T>::init(int init_size)
{
    assert((init_size & (init_size - 1)) == 0);
    if (this->bucket != nullptr) {
        assert(false);
        return;
    }
    this->size = init_size;
    this->bucket = new phmap::flat_hash_map<u32, T>[this->size];
};

template <typename T>
void TTable<T>::destroy()
{
    if (this->bucket == nullptr) {
        return;
    }
    delete[] this->bucket;
};

template <typename T>
u32 TTable<T>::get_hash(Field& field)
{
    return xxhash32((const void*)field.data, sizeof(Field), 31);
};

template <typename T>
u32 TTable<T>::get_index(u32 hash)
{
    return hash & (this->size - 1);
};

template <typename T>
bool TTable<T>::get_entry(u32 hash, T& entry)
{
    u32 index = this->get_index(hash);
    auto it = this->bucket[index].find(hash);
    if (it == this->bucket[index].end()) {
        return false;
    }
    entry = it->second;
    return true;
};

template <typename T>
void TTable<T>::set_entry(u32 hash, T& entry)
{
    u32 index = this->get_index(hash);
    this->bucket[index][hash] = entry;
};