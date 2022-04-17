#pragma once

#include <cstdint>

template<class T>
static T nextPO2MinusOne(T x)
{
	// Assume 16-bit
	x = x | (x >> 1);
	x = x | (x >> 2);
	x = x | (x >> 4);
	x = x | (x >> 8);
	return x;
}

static uint32_t nextPO2MinusOne(uint32_t x)
{
	x = x | (x >> 1);
	x = x | (x >> 2);
	x = x | (x >> 4);
	x = x | (x >> 8);
	x = x | (x >> 16);
	return x;
}

static uint64_t nextPO2MinusOne(uint64_t x)
{
	x = x | (x >> 1);
	x = x | (x >> 2);
	x = x | (x >> 4);
	x = x | (x >> 8);
	x = x | (x >> 16);
	x = x | (x >> 32);
	return x;
}

template<typename T>
static T leastZeroBits(T x)
{
	// Return an integer where exactly all the
	// least significant zero bits are set.
	x = x | (x << 1);
	x = x | (x << 2);
	x = x | (x << 4);
	x = x | (x << 8);
	// Invert x to reveal zero bits
	return ~x;
}

static uint32_t leastZeroBits(uint32_t x)
{
	x = x | (x << 1);
	x = x | (x << 2);
	x = x | (x << 4);
	x = x | (x << 8);
	x = x | (x << 16);
	return ~x;
}

static uint64_t leastZeroBits(uint64_t x)
{
	x = x | (x << 1);
	x = x | (x << 2);
	x = x | (x << 4);
	x = x | (x << 8);
	x = x | (x << 16);
	x = x | (x << 32);
	return ~x;
}

template<typename T>
inline static bool isPO2(T x)
{
	return (x & (x - 1)) == 0;
}

template<typename T>
inline static bool isPO2MinusOne(T x)
{
	return (x & (x + 1)) == 0;
}

template <typename T>
static uint8_t popcount(T x)
{
	static const uint8_t nibblePopcount[16] = {
		/*0b0000: */ 0, /*0b0001: */ 1, /*0b0010: */ 1, /*0b0011: */ 2,
		/*0b0100: */ 1, /*0b0101: */ 2, /*0b0110: */ 2, /*0b0111: */ 3,
		/*0b1000: */ 1, /*0b1001: */ 2, /*0b1010: */ 2, /*0b1011: */ 3,
		/*0b1100: */ 2, /*0b1101: */ 3, /*0b1110: */ 3, /*0b1111: */ 4
	};

	// Runs in log(x)/4. Alternatively switch to constant time algorithm.
	uint8_t count = 0;
	while (x)
	{
		count += nibblePopcount[x & 0b1111];
		x >>= 4;
	}
	return count;
}

template <typename T>
inline static T ceilDiv(T a, T b)
{
	return (a + b - 1) / b;
}
