#pragma once

#include "debug.h"
#include "math.h"

#define HASH_SET_MINIMUM_CAPACITY 8

#define HASH_SET_DEFAULT_LOAD_FACTOR 0.7f
#define HASH_SET_MINIMUM_LOAD_FACTOR 0.1f
#define HASH_SET_MAXIMUM_LOAD_FACTOR 0.9f

template <typename K>
struct Hash
{
    static size_t get(const K& k)
    {
        uint64_t h = 0;

        const char* ptr = static_cast<char*>(*k);
        size_t i = sizeof(K);
        for (; i >= 8; i -= 8)
        {
            h += *static_cast<uint64_t*>(ptr);
            ptr += 8;
        }
        // Append the remaining <8 bytes.
        uint64_t x = 0;
        for (; i > 0; i--)
        {
            x = (x << 8) | *ptr;
            ptr++;
        }
        h += x;

        return Hash<uint64_t>::get(h);
    }
};

enum HashSet_Entry_Type : uint8_t
{
    EMPTY = 0,
    USED,
    TOMBSTONE
};

template <class K>
struct HashSet_Entry
{
    K key;
    HashSet_Entry_Type type;
};

template<class K, typename HashGenerator = Hash<K>>
class HashSet
{
private:
    using Entry = HashSet_Entry<K>;
    using Type = HashSet_Entry_Type;

public:
    HashSet() :
        HashSet(HASH_SET_MINIMUM_CAPACITY)
    { }

    HashSet(size_t initialCapacity, float loadFactor = HASH_SET_DEFAULT_LOAD_FACTOR) :
        m_LoadFactor(loadFactor),
        m_Capacity(initialCapacity),
        m_Size(0),
        m_TombstoneCount(0)
    {
        RF_ASSERT(m_Capacity >= HASH_SET_MINIMUM_CAPACITY, "initialCapacity must be at least "#HASH_SET_MINIMUM_CAPACITY);
        RF_ASSERT(isPO2(m_Capacity), "initialCapacity must be a power of two!");

        RF_ASSERT(loadFactor >= HASH_SET_MINIMUM_LOAD_FACTOR, "loadFactor must be at least "#HASH_SET_MINIMUM_LOAD_FACTOR);
        RF_ASSERT(loadFactor <= HASH_SET_MAXIMUM_LOAD_FACTOR, "loadFactor must be at most "#HASH_SET_MAXIMUM_LOAD_FACTOR);

        m_Table = new Entry[m_Capacity];
        for (size_t i = 0; i < m_Capacity; i++)
            m_Table[i].type = Type::EMPTY;
    }

    HashSet(const HashSet& hashSet) :
        m_LoadFactor(hashSet.m_LoadFactor),
        m_Capacity(hashSet.m_Capacity),
        m_Size(hashSet.m_Size),
        m_TombstoneCount(hashSet.m_TombstoneCount)
    {
        RF_ASSERT(hashSet.m_Table != nullptr, "other hash set has been deleted");

        m_Table = new Entry[m_Capacity];
        for (size_t i = 0; i < m_Capacity; i++)
            m_Table[i] = hashSet.m_Table[i];
    }

    ~HashSet()
    {
        delete[] m_Table;
        m_Table = nullptr;
    }

    inline size_t size() const { return m_Size; }

    inline size_t capacity() const { return m_Capacity; }

    inline bool empty() const { return m_Size == 0; }
    
    bool contains(const K& element) const
    {
        return m_Table[indexOf(element, m_Table, m_Capacity)].type == Type::USED;
    }

    bool insert(const K& element)
    {
        const float loadedCapacity = m_Capacity * m_LoadFactor;
        if (m_Size + m_TombstoneCount >= loadedCapacity)
        {
            // Note: the invariant (m_Size > m_TombstoneCount).
            if (m_Size >= loadedCapacity * 0.75f)
            {
                // Table is >=75% loaded. Probably safe to double the capacity.
                resizeTable(m_Capacity << 1);
            }
            else
            {
                // Table is <75% loaded. Just rehash the table with same capacity.
                resizeTable(m_Capacity);
            }
        }

        Entry& entry = m_Table[indexOfWithReplacement(element, m_Table, m_Capacity)];
        switch (entry.type)
        {
        case USED:
            // Element already exists. Ignore the insert.
            return false;
        case TOMBSTONE:
            m_TombstoneCount--;
            [[fallthrough]];
        case EMPTY:
            entry.key = element;
            entry.type = Type::USED;
            m_Size++;
            return true;
        default:
            RF_ASSERT(false, "Unknown entry type");
            return false;
        }
    }

    bool remove(const K& element)
    {
        Entry& entry = m_Table[indexOf(element, m_Table, m_Capacity)];
        if (entry.type != Type::USED)
        {
            // Element does not exist. Ignore the remove.
            return false;
        }

        entry.type = Type::TOMBSTONE;

        m_Size--;
        m_TombstoneCount++;

        if (m_Capacity >= HASH_SET_MINIMUM_CAPACITY && m_TombstoneCount > m_Size)
        {
            // We have more tombstones than regular elements. Halve the capacity.
            resizeTable(m_Capacity >> 1);
        }

        return true;
    }

    void resizeTable(size_t newCapacity)
    {
        RF_ASSERT(isPO2(newCapacity), "newCapacity is not a power of two!");
        
        if (newCapacity < HASH_SET_MINIMUM_CAPACITY)
            newCapacity = HASH_SET_MINIMUM_CAPACITY;

        // Rehash the entire table (removing tombstones).
        Entry* newTable = new Entry[newCapacity];
        for (size_t i = 0; i < newCapacity; i++)
            newTable[i].type = Type::EMPTY;
        for (size_t i = 0; i < m_Capacity; i++)
        {
            const Entry& entry = m_Table[i];
            if (entry.type == Type::USED)
            {
                Entry& newEntry = newTable[indexOf(entry.key, newTable, newCapacity)];
                newEntry.key = entry.key;
                newEntry.type = Type::USED;
            }
        }
        delete[] m_Table;
        m_Capacity = newCapacity;
        m_TombstoneCount = 0;
        m_Table = newTable;
    }

private:
    static size_t indexOf(const K& element, const Entry* table, const size_t& capacity)
    {
        // Requires capacity to be a power of two!
        const size_t mask = capacity - 1;

        // Find first empty entry or instance of element
        size_t h = hashOf(element) & mask;
        while (table[h].type != Type::EMPTY && !(table[h].key == element))
            h = (h + 1) & mask;
        return h;
    }

    // Special version of indexOf where we allow replacement of tombstones (if element not found).
    static size_t indexOfWithReplacement(const K& element, const Entry* table, const size_t& capacity)
    {
        // Requires capacity to be a power of two!
        const size_t mask = capacity - 1;

        // Find first tombstone/empty entry or instance of element
        size_t h = hashOf(element) & mask;
        while (table[h].type == Type::USED)
        {
            if (table[h].key == element)
            {
                // Instance was found, return immediately.
                return h;
            }
            h = (h + 1) & mask;
        }
        // Continue looking for instance of element (assuming above was tombstone or empty),
        // and store the above-found value as tmp.
        size_t tmp = h;
        while (table[h].type != Type::EMPTY)
        {
            if (table[h].key == element)
            {
                // Instance found past tombstone, return immediately.
                return h;
            }
            h = (h + 1) & mask;
        }
        // Element was not found. Return early tombstone/empty entry.
        return tmp;
    }

    static size_t hashOf(const K& element)
    {
        return HashGenerator::get(element);
    }

private:
    float m_LoadFactor;
    size_t m_Capacity;
    size_t m_Size;
    size_t m_TombstoneCount;
    Entry* m_Table;

};

/* Standard specializations of basic types of hash functions */

template <>
struct Hash<uint16_t>
{
    static size_t get(const uint16_t& k)
    {
        // See https://github.com/skeeto/hash-prospector
        uint16_t h = k;
        h = (h ^ (h >> 8)) * 0x88b5U;
        h = (h ^ (h >> 7)) * 0xdb2dU;
        h = h ^ (h >> 9);
        return h;
    }
};

template <>
struct Hash<uint32_t>
{
    static size_t get(const uint32_t& k)
    {
        // See https://github.com/skeeto/hash-prospector
        uint32_t h = k;
        h = (h ^ (h >> 16)) * 0x7feb352dU;
        h = (h ^ (h >> 15)) * 0x846ca68bU;
        h = h ^ (h >> 16);
        return h;
    }
};

template <>
struct Hash<uint64_t>
{
    static size_t get(const uint64_t& k)
    {
        // See https://xorshift.di.unimi.it/splitmix64.c
        uint64_t h = k;
        h = (h ^ (h >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
        h = (h ^ (h >> 27)) * UINT64_C(0x94d049bb133111eb);
        h = h ^ (h >> 31);
        return h;
    }
};

template <>
struct Hash<int16_t>
{
    static size_t get(const int16_t& k)
    {
        return Hash<uint16_t>::get(static_cast<const uint16_t&>(k));
    }
};

template <>
struct Hash<int32_t>
{
    static size_t get(const int32_t& k)
    {
        return Hash<uint32_t>::get(static_cast<const uint32_t&>(k));
    }
};

template <>
struct Hash<int64_t>
{
    static size_t get(const int64_t& k)
    {
        return Hash<uint64_t>::get(static_cast<const uint64_t&>(k));
    }
};
