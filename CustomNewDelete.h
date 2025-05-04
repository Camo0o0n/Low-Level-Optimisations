#pragma once
#include <cstdlib>
#include <cstddef>
#include <iostream>
#include "globals.h"
#include "MemoryPool.h"

void* operator new (size_t size);
void* operator new (size_t size, Tracker* trackerPtr);
void* operator new[] (size_t size, Tracker* trackerPtr);
void* operator new (size_t size, MemoryPool* pool, Tracker* trackerPtr);
void operator delete (void* pMem);
void operator delete[] (void* pMem);