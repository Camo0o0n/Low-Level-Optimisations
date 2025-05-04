#pragma once
#include "ColliderObject.h"
class Sphere :
    public ColliderObject
{
public:
	static void* operator new(size_t size, Tracker* trackerPtr);
	static void operator delete(void* pMem);
	Sphere();
    void drawMesh() { glutSolidSphere(0.5, 5, 5); }
}; 

