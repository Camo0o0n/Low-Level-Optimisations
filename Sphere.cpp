#include "Sphere.h"
#include "CustomNewDelete.h"

Sphere::Sphere() {
	object = "sphere";
}

void* Sphere::operator new(size_t size, Tracker* trackerPtr)
{
	return::operator new(size, trackerPtr);
}

void Sphere::operator delete(void* pMem)
{
	::operator delete(pMem);
}