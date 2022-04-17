#pragma once

#include "util/debug.h"

#define HEAP_MINIMUM_CAPACITY 8

template <class T>
class MinHeap
{
public:
	MinHeap() :
		MinHeap(HEAP_MINIMUM_CAPACITY)
	{ }

	MinHeap(size_t initialCapacity) :
		m_Size(0),
		m_Capacity(initialCapacity)
	{
		RF_ASSERT(initialCapacity >= HEAP_MINIMUM_CAPACITY, "initialCapacity must at least "#HEAP_MINIMUM_CAPACITY);

		m_Heap = new T[m_Capacity];
	}

	MinHeap(const MinHeap& heap) :
		m_Size(heap.m_Size),
		m_Capacity(heap.m_Capacity)
	{
		RF_ASSERT(heap.m_Heap != nullptr, "other heap has been deleted");

		m_Heap = new T[m_Capacity];
		for (size_t i = 1; i <= m_Size; i++)
			m_Heap[i] = heap.m_Heap[i];
	}

	~MinHeap()
	{
		delete[] m_Heap;
		m_Heap = nullptr;
	}

	inline size_t size() const { return m_Size; }

	inline size_t capacity() const { return m_Capacity; }

	inline bool empty() const { return m_Size == 0; }

	T peek() const
	{
		RF_ASSERT(m_Size > 0, "min-heap is empty");

		return m_Heap[1];
	}

	T extract()
	{
		RF_ASSERT(m_Size > 0, "min-heap is empty");

		T value = m_Heap[1];
		m_Heap[1] = m_Heap[m_Size];

		m_Size--;
		size_t halfCapacity = m_Capacity >> 1;
		if (m_Size < (halfCapacity >> 1))
			resizeHeap(halfCapacity);

		bubbleDown(1);

		return value;
	}

	void insert(T element)
	{
		m_Size++;
		if (m_Size >= m_Capacity)
			resizeHeap(m_Capacity << 1);

		m_Heap[m_Size] = element;
		bubbleUp(m_Size);
	}

private:
	void bubbleUp(size_t index)
	{
		while (index > 1) // Stop at root (1)
		{
			const size_t parent = index >> 1;
			if (m_Heap[index] < m_Heap[parent])
			{
				// Heap order violated. Swap with parent.
				swapElements(parent, index);
				index = parent;
			}
			else
			{
				// Heap order is not violated.
				break;
			}
		}
	}

	void bubbleDown(size_t index)
	{
		const size_t halfSize = m_Size >> 1;
		while (index <= halfSize) // Stop at a leaf
		{
			const size_t left = index << 1;
			const size_t right = left + 1;

			if (m_Heap[left] < m_Heap[index] || m_Heap[right] < m_Heap[index])
			{
				// Heap order is violated by one of the children. Take smallest and swap.
				if (m_Heap[left] < m_Heap[right])
				{
					swapElements(left, index);
					index = left;
				}
				else
				{
					swapElements(right, index);
					index = right;
				}
			}
			else
			{
				// Heap order is not violated.
				break;
			}
		}
	}

	void swapElements(size_t i0, size_t i1)
	{
		const T tmp = m_Heap[i0];
		m_Heap[i0] = m_Heap[i1];
		m_Heap[i1] = tmp;
	}

	void resizeHeap(size_t newCapacity)
	{
		if (newCapacity < HEAP_MINIMUM_CAPACITY)
			newCapacity = HEAP_MINIMUM_CAPACITY;

		if (m_Capacity != newCapacity)
		{
			T* newHeap = new T[newCapacity];
			for (size_t i = 1; i <= m_Size; i++)
				newHeap[i] = m_Heap[i];
			delete[] m_Heap;
			m_Capacity = newCapacity;
			m_Heap = newHeap;
		}
	}

private:
	size_t m_Size;
	size_t m_Capacity;
	T* m_Heap;
};
