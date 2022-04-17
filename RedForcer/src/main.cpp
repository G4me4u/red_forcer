#include <iostream>

#include "rfpch.h"
#include "world/state.h"

#include <unordered_set>
#include <chrono>

#define N 10000000
#define MESSAGE 0

void testContains(const HashSet<int32_t>& set, int32_t elem)
{
#if MESSAGE
    std::cout << "contains(" << elem << "): " << (set.contains(elem) ? "yes" : "no") << std::endl;
#endif
}

void testMyImpl()
{
    HashSet<int32_t> set;

    set.insert(0);
    set.insert(23);
    set.insert(5);
    set.insert(23);
    set.insert(67);
    set.insert(1);
    set.insert(2);

    testContains(set, 23);
    testContains(set, 1);
    testContains(set, 4);
    testContains(set, 2);
    testContains(set, 3);

    for (int32_t i = 0; i < N; i++)
    {
        if (!set.insert(i))
        {
#if MESSAGE
            std::cout << "unable to insert " << i << " as it already exists." << std::endl;
#endif
        }
    }

    for (int32_t i = 0; i < N; i++)
    {
        if (set.insert(i))
        {
            std::cout << "error! Able to insert " << i << std::endl;
        }
    }

    for (int32_t i = 0; i < N; i++)
    {
        if (!set.contains(i))
        {
            std::cout << "error! Should contain " << i << std::endl;
        }
    }

    std::cout << "size: " << set.size() << std::endl;

    for (int32_t i = N - 1; i >= 0; i--)
    {
        size_t prevSize = set.size();
        size_t prevCap = set.capacity();
        if (!set.remove(i))
        {
            std::cout << "error! Should be able to remove " << i << std::endl;
        }

        if (prevSize - 1 != set.size())
        {
            std::cout << "error! Size did not decrease" << std::endl;
        }

        if (prevCap != set.capacity())
        {
#if MESSAGE
            std::cout << "set capacity changed from " << prevCap << " to " << set.capacity() << ". Size: " << set.size() << std::endl;
#endif
        }
    }

    std::cout << "size: " << set.size() << std::endl;
}

bool contains(const std::unordered_set<int32_t>& set, int32_t elem)
{
    return set.find(elem) != set.end();
}

void testContainsSTL(const std::unordered_set<int32_t>& set, int32_t elem)
{
#if MESSAGE
    std::cout << "contains(" << elem << "): " << (contains(set, elem) ? "yes" : "no") << std::endl;
#endif
}

void testSTLImpl()
{
    std::unordered_set<int32_t> set;

    set.insert(0);
    set.insert(23);
    set.insert(5);
    set.insert(23);
    set.insert(67);
    set.insert(1);
    set.insert(2);

    testContainsSTL(set, 23);
    testContainsSTL(set, 1);
    testContainsSTL(set, 4);
    testContainsSTL(set, 2);
    testContainsSTL(set, 3);

    for (int32_t i = 0; i < N; i++)
    {
        if (contains(set, i))
        {
#if MESSAGE
            std::cout << "unable to insert " << i << " as it already exists." << std::endl;
#endif
        }

        set.insert(i);
    }

    for (int32_t i = 0; i < N; i++)
    {
        if (!contains(set, i))
        {
            std::cout << "error! Able to insert " << i << std::endl;
        }

        set.insert(i);
    }

    for (int32_t i = 0; i < N; i++)
    {
        if (!contains(set, i))
        {
            std::cout << "error! Should contain " << i << std::endl;
        }
    }

    std::cout << "size: " << set.size() << std::endl;

    for (int32_t i = N - 1; i >= 0; i--)
    {
        size_t prevSize = set.size();
        if (!set.erase(i))
        {
            std::cout << "error! Should be able to remove " << i << std::endl;
        }

        if (prevSize - 1 != set.size())
        {
            std::cout << "error! Size did not decrease" << std::endl;
        }
    }

    std::cout << "size: " << set.size() << std::endl;
}

int main()
{
    std::chrono::steady_clock::time_point begin, end;

    begin = std::chrono::steady_clock::now();
    testMyImpl();
    end = std::chrono::steady_clock::now();
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms" << std::endl;

    begin = std::chrono::steady_clock::now();
    testSTLImpl();
    end = std::chrono::steady_clock::now();
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms" << std::endl;

    return 0;
}
