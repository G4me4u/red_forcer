#pragma once

#include "../rfpch.h"

struct Block
{
	int8_t type; /* block id */
	uint8_t state; /* block state data */
	uint16_t data; /* block entity data */
};

struct BlockPos
{
	const uint32_t index; /* encoded index (x + y * yStride + z * zStride) */
};

enum ScheduledPriority
{
	VERY_HIGH = -2,
	HIGH,
	NORMAL,
	LOW,
	VERY_LOW
};

struct ScheduledInfo
{
	ScheduledPriority priority;
	uint32_t timestamp; /* timestamp update should be executed */
	int8_t expectedType; /* expected block type at position */
	BlockPos pos;
};

class State
{
public:
	State::State(uint32_t width, uint32_t height, uint32_t depth) :
		width(width),
		height(height),
		depth(depth),

		yStride(width),
		zStride(width * height),
		/* Store (x, y, z) at index(x + y * yStride + z * zStride) */
		blocks(new Block[width * height * depth]),

		time(0),

		scheduledUpdates()
	{ }

	State::~State()
	{
		delete[] blocks;
		blocks = nullptr;
	}

	inline void setBlock(BlockPos pos, Block block)
	{
		blocks[pos.index] = block;
	}

	inline Block getBlock(BlockPos pos)
	{
		return blocks[pos.index];
	}

	inline BlockPos makePos(const uint32_t x, const uint32_t y, const uint32_t z)
	{
		return { x + y * yStride + z * zStride };
	}

	inline BlockPos offsetWest(const BlockPos pos)
	{
		return { pos.index - 1 };
	}

	inline BlockPos offsetEast(const BlockPos pos)
	{
		return { pos.index + 1 };
	}

	inline BlockPos offsetUp(const BlockPos pos)
	{
		// Note: up is positive in MC
		return { pos.index + yStride };
	}

	inline BlockPos offsetDown(const BlockPos pos)
	{
		// Note: down is negative in MC
		return { pos.index - yStride };
	}

	inline BlockPos offsetNorth(const BlockPos pos)
	{
		return { pos.index - zStride };
	}

	inline BlockPos offsetSouth(const BlockPos pos)
	{
		return { pos.index + zStride };
	}

	inline void schedule(const BlockPos pos, uint32_t delay, ScheduledPriority priority = NORMAL)
	{
		//scheduledUpdates.insert({ priority, time + delay, getBlock(pos).type, pos });
	}

private:
	const uint32_t width;
	const uint32_t height;
	const uint32_t depth;

	const uint32_t yStride;
	const uint32_t zStride;
	Block* blocks;

	uint32_t time;

	MinHeap<int> scheduledUpdates;


};
