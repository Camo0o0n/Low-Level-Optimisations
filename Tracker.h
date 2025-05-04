#pragma once
#include "globals.h"

struct Header;
struct Footer;

class Tracker
{
public:
	char name[64];
	int totalBytes = 0;
	int runningTotalBytes = 0;
	int numOfInvalids = 0;
	int count = 0;
	Header* pHeapStart = nullptr;
	Header* pHeapEnd = nullptr;
	Header* pCurrentHeader = nullptr;
	void* pCurrentHeaderAddress = nullptr;
	Footer* pCurrentFooter = nullptr;
	void* pCurrentFooterAddress = nullptr;
	Header* AddToTracker(Header*);
	void RemoveFromTracker(Header*);
	int WalkTheHeap();
	void print(Header* header, Footer* footer);
};

