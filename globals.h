#pragma once
#include <string>
#include <iostream>
#include <cstring>
#include "Tracker.h"

#define minX -10.0f
#define maxX 30.0f
#define minZ -10.0f
#define maxZ 30.0f
#define minY 0
#define maxY 40.0f


// gravity - change it and see what happens (usually negative!)
const float gravity = -19.81f;

struct Tracker;

// Header & Footer size
struct /*alignas(64)*/ Header
{
	int size;
	Tracker* pTracker = nullptr;
	Header* pNext = nullptr;
	Header* pPrev = nullptr;
	char testCharArray[12];
	int checkValue = 50;
};

struct /*alignas(16)*/ Footer
{
	int checkValue = 50;
	/*char padding[12];*/
};