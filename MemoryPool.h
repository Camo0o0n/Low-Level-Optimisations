#pragma once
#include <vector>
#include "globals.h"

class MemoryPool
{
public:
	MemoryPool(size_t ObjectSize, size_t poolSize = 1024);
	~MemoryPool();

	void* Alloc(size_t size);
	void Free(void* p, size_t size);

private:
	struct Node {
		Node* next;
	};

	size_t objectSize;
	size_t poolSize;
	Node* freeList;
	std::vector<void*> blockVector;

	void allocatePool();
};

