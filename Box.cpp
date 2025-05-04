#include "Box.h"
#include "CustomNewDelete.h"



Box::Box()  {
	object = "box";
}

void* Box::operator new(size_t size, Tracker* trackerPtr)
{
	return ::operator new(size, trackerPtr);
}

void* Box::operator new(size_t size, MemoryPool* pool, Tracker* trackerPtr)
{
	return ::operator new(size, pool, trackerPtr);
}


void Box::operator delete(void* pMem)
{
	::operator delete(pMem);
}
