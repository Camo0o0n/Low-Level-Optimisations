#pragma once

#include "ColliderObject.h"
#include "MemoryPool.h"

class Box : public ColliderObject
{
public:
	static void* operator new(size_t size);
	static void* operator new(size_t size, Tracker* trackerPtr);
	static void* operator new(size_t size, MemoryPool* pool, Tracker* trackerPtr);
	static void operator delete(void* pMem);
	Box();
	void drawMesh() { glutSolidCube(1.0); }
};

