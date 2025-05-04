#include "MemoryPool.h"


MemoryPool::MemoryPool(size_t ObjectSize, size_t poolSize) : objectSize(ObjectSize), poolSize(poolSize), freeList(nullptr)
{
	if (ObjectSize >= sizeof(void*))
	{
		allocatePool();
	};
}

MemoryPool::~MemoryPool()
{
	for (void* block : blockVector) {
		free(block);
	}
}

void* MemoryPool::Alloc(size_t size)
{
	if (!freeList) {
		allocatePool();
	}

	Node* freeChunk = freeList;
	freeList = freeList->next;
	return (void*)freeChunk;
}

void MemoryPool::Free(void* p, size_t size)
{
	Header* temp = (Header*)p;
	temp->pTracker->RemoveFromTracker(temp);
	Node* node = (Node*)p;
	node->next = freeList;
	freeList = node;
}

void MemoryPool::allocatePool()
{
	void* block = malloc(objectSize * poolSize);
	if (!block) { throw std::bad_alloc(); }
	blockVector.push_back(block);

	char* current = (char*)block;
	for (int i = 0; i < poolSize - 1; i++) {
		Node* node = reinterpret_cast<Node*>(current);
		node->next = reinterpret_cast<Node*>(current + objectSize);
		current += objectSize;
	}

	Node* lastNode = reinterpret_cast<Node*>(current);
	lastNode->next = nullptr;

	freeList = reinterpret_cast<Node*>(block);

}
