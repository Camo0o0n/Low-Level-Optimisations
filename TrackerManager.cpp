#include "TrackerManager.h"

Tracker* TrackerManager::createTracker(std::string name)
{
    Tracker* newTracker = (Tracker*)malloc((size_t)sizeof(Tracker));
	strncpy_s(newTracker->name, name.c_str(), sizeof(newTracker->name) - 1);
	newTracker->totalBytes = 0;
	newTracker->runningTotalBytes = 0;
	newTracker->numOfInvalids = 0;
	newTracker->count = 0;
	newTracker->pHeapStart = nullptr;
	newTracker->pHeapEnd = nullptr;
	newTracker->pCurrentHeader = nullptr;
	newTracker->pCurrentHeaderAddress = nullptr;
	newTracker->pCurrentFooter = nullptr;
	newTracker->pCurrentFooterAddress = nullptr;

	trackerVector.push_back(newTracker);
	return newTracker;
}

void TrackerManager::printAlloc()
{
	int count = 0;
	std::cout << "\nTotaled on initialisation number of bytes:" << std::endl;
	for (Tracker* tracker : trackerVector) {
		std::cout << tracker->name << ": " << tracker->totalBytes << std::endl;
	}
}
