#include "CustomNewDelete.h"

void* operator new (size_t size)
{
	//std::cout << "general initialiser" << std::endl;
	size_t nRequestedBytes = size + sizeof(Header) + sizeof(Footer) /*+ 4*/;

	char* pMem = (char*)malloc(nRequestedBytes);
	//Align of
	Header* pHeader = (Header*)pMem;
	pHeader->size = size;
	pHeader->checkValue = 50;
	pHeader->pNext = nullptr;
	pHeader->pPrev = nullptr;
	pHeader->pTracker = nullptr;

	void* pFooterAddr = pMem + sizeof(Header) + size;
	Footer* pFooter = (Footer*)pFooterAddr;
	pFooter->checkValue = 50;

	void* pStartMemBlock = pMem + sizeof(Header);
	return pStartMemBlock;
	//char* pMem = (char*)malloc(size + 4);
	//void* pStartMemBlock = pMem;
	//return pStartMemBlock;
}

void* operator new(size_t size, Tracker* trackerPtr)
{
	size_t nRequestedBytes = size + sizeof(Header) + sizeof(Footer) /*+ 4*/;

	char* alignas(8) pMem = (char*)malloc(nRequestedBytes);
	//calloc
	Header* pHeader = (Header*)pMem;
	pHeader->size = size;
	pHeader->checkValue = 50;
	pHeader->pNext = nullptr;
	pHeader->pPrev = nullptr;
	pHeader->pTracker = trackerPtr;

	void* pFooterAddr = pMem + sizeof(Header) + size;
	Footer* pFooter = (Footer*)pFooterAddr;
	pFooter->checkValue = 50;

	void* pStartMemBlock = pMem + sizeof(Header);
	trackerPtr->AddToTracker(pHeader);
	return pStartMemBlock;
}

void* operator new[](size_t size, Tracker* trackerPtr)
{
	size_t nRequestedBytes = size + sizeof(Header) + sizeof(Footer);

	char* alignas(8) pMem = (char*)malloc(nRequestedBytes);

	Header* pHeader = (Header*)pMem;
	pHeader->size = size;
	pHeader->checkValue = 50;
	pHeader->pNext = nullptr;
	pHeader->pPrev = nullptr;
	pHeader->pTracker = trackerPtr;

	void* pFooterAddr = pMem + sizeof(Header) + size;
	Footer* pFooter = (Footer*)pFooterAddr;
	pFooter->checkValue = 50;

	void* pStartMemBlock = pMem + sizeof(Header);
	trackerPtr->AddToTracker(pHeader);
	return pStartMemBlock;
}

void* operator new(size_t size, MemoryPool* pool, Tracker* trackerPtr)
{

	char* alignas(8) pMem = (char*)pool->Alloc(sizeof(Header) + sizeof(Footer) + size /*+ 4*/);

	Header* pHeader = (Header*)pMem;
	pHeader->size = size;
	pHeader->checkValue = 50;
	pHeader->pNext = nullptr;
	pHeader->pPrev = nullptr;
	pHeader->pTracker = trackerPtr;

	void* pFooterAddr = pMem + sizeof(Header) + size;
	Footer* pFooter = (Footer*)pFooterAddr;
	pFooter->checkValue = 50;

	void* pStartMemBlock = pMem + sizeof(Header);
	trackerPtr->AddToTracker(pHeader);
	return pStartMemBlock;
}

void operator delete (void* pMem)
{
	//std::cout << "general deconstructor" << std::endl;
	Header* pHeader = (Header*)((char*)pMem - sizeof(Header));
	Footer* pFooter = (Footer*)((char*)pMem + pHeader->size);
	if (pHeader->pTracker != nullptr) {
		pHeader->pTracker->RemoveFromTracker(pHeader);
	}
	free(pHeader);
	pHeader = nullptr;
}
void operator delete[] (void* pMem)
{
	//std::cout << "general deconstructor" << std::endl;
	Header* pHeader = (Header*)((char*)pMem - sizeof(Header));
	Footer* pFooter = (Footer*)((char*)pMem + pHeader->size);
	if (pHeader->pTracker != nullptr) {
		pHeader->pTracker->RemoveFromTracker(pHeader);
	}
	free(pHeader);
	pHeader = nullptr;
}