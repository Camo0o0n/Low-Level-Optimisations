#include "Tracker.h"

/// <summary>
/// need to handle walking the heap when there is nothing in it
/// </summary>

int Tracker::WalkTheHeap()
{
	runningTotalBytes = 0;
	pCurrentHeader = nullptr;
	bool a = true;
	while (a)
	{
		if (pCurrentHeader == nullptr) {
			if (pHeapStart == nullptr) { return totalBytes; }
			pCurrentHeader = pHeapStart;
			pCurrentHeaderAddress = (char*)pCurrentHeader;
			pCurrentFooterAddress = (char*)pCurrentHeader + sizeof(Header) + pCurrentHeader->size;
			pCurrentFooter = (Footer*)pCurrentFooterAddress;
		}
		else if (pCurrentHeader->pNext != nullptr) {
			pCurrentHeader = pCurrentHeader->pNext;
			pCurrentHeaderAddress = (char*)pCurrentHeader;
			pCurrentFooterAddress = (char*)pCurrentHeader + sizeof(Header) + pCurrentHeader->size;
			pCurrentFooter = (Footer*)pCurrentFooterAddress;
		}
		else {
			return runningTotalBytes;
		}
		try {
			if (pCurrentFooter == nullptr) { return runningTotalBytes; }
			else if (pCurrentHeader->checkValue == pCurrentFooter->checkValue) { runningTotalBytes += pCurrentHeader->size + sizeof(Header) + sizeof(Footer); print(pCurrentHeader, pCurrentFooter);}
			else { runningTotalBytes += pCurrentHeader->size + sizeof(Header) + sizeof(Footer); print(pCurrentHeader, pCurrentFooter);
			}
		}
		catch (std::exception& e) { std::cout << "memory invalid" << std::endl; runningTotalBytes += pCurrentHeader->size; }
	}
	return 0;
}

void Tracker::print(Header* header, Footer* footer) {
	std::cout << "Prev Header Pointer: " << header->pPrev << "\nCurrent Header Pointer:" << header << "\nNext Header Pointer: " << header->pNext << "\nTracker Pointer: " << header->pTracker << "\nSize: " << header->size << "\nHeader Check Value: " << header->checkValue << "\nFooter Check Value: " << footer->checkValue << "\nDoes Match?: " << (header->checkValue == footer->checkValue ? "Match" : "No Match") << "\n" << std::endl;
	if (header->checkValue != footer->checkValue) { std::cout << "Continue?" << std::flush; std::cin.get();}
}

Header* Tracker::AddToTracker(Header* newHeader)
{
	if (pHeapStart == nullptr) { pHeapStart = newHeader; pHeapEnd = newHeader; newHeader->pPrev = NULL; newHeader->pTracker = this; }
	else if (pHeapStart) { 
		pHeapEnd->pNext = newHeader;
		newHeader->pPrev = pHeapEnd;
		pHeapEnd = newHeader;
		newHeader->pTracker = this;
	}
	count++;
	totalBytes += sizeof(Header) + newHeader->size + sizeof(Footer);
	return newHeader;
}

void Tracker::RemoveFromTracker(Header* delHeader)
{
	Header* prev = delHeader->pPrev;
	Header* next = delHeader->pNext;
	if (!delHeader->pTracker) { return; }
	else if (prev == nullptr && next == nullptr) {
		pHeapStart = nullptr;
		pHeapEnd = nullptr;
	}
	else if (prev == nullptr) {
		pHeapStart = next;
		next->pPrev = nullptr;
	}
	else if (next == nullptr) {
		pHeapEnd = prev;
		prev->pNext = nullptr;
	}
	else {
		prev->pNext = next;
		next->pPrev = prev;
	}
	count--;
	totalBytes -= sizeof(Header) + delHeader->size + sizeof(Footer);
}