#pragma once
#include "Tracker.h"
#include <vector>
#include <iostream>
class TrackerManager
{
public:
	std::vector<Tracker*> trackerVector;

	Tracker* createTracker(std::string name);

	void printAlloc();
};

