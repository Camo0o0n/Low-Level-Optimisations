#pragma once
#include "ColliderObject.h"
#include "globals.h"
#include "Vec3.h"
#include <vector>
#include <list>
#include <thread>

struct CollStruct
{
	std::vector<std::vector<ColliderObject*>*> collObjList;
};

class BoundingBox {
public:
	Vec3 centre;
	float halfSize;
	Vec3 minBound;
	Vec3 maxBound;
	BoundingBox(const Vec3 center, float halfSize) : centre(center), halfSize(halfSize) { minBound = { centre.x - halfSize, centre.y - halfSize, centre.z - halfSize }; maxBound = { centre.x + halfSize, centre.y + halfSize, centre.z + halfSize }; }
	bool doesItContain(Vec3 position);
};

class OctreeNode {
public:
	BoundingBox* boundingBox;
	std::vector<ColliderObject*> objects;
	OctreeNode* childrenNode[8] = { nullptr };
	int depth;
	int maxDepth;
	OctreeNode(BoundingBox* bounds, int depth, int maxDepth);
	void OctreeSetup();
	void initObjects(std::vector<ColliderObject*> objectList);
	void initObjects();
	void getObjectList(CollStruct* collStruct);
	int validateObjects(bool setup);
	int validateObjects(ColliderObject* object);
	void sortSingleObjectStart(ColliderObject* object);
	void sortSingleObject(ColliderObject* object);
	int removeObject(ColliderObject* object);
};

class Octree {
public:
	OctreeNode* root;
	Octree(BoundingBox* bounds, int maxDepth) : root(new OctreeNode(bounds, 0, maxDepth)) {}
	void InitObjects(std::vector<ColliderObject*> objectList) { root->initObjects(objectList); }
	void getObjectList(CollStruct* collStruct) { root->getObjectList(collStruct); }
	void validateObjects() { root->validateObjects(true); }
	void addObject(ColliderObject* object) { root->sortSingleObjectStart(object); root->objects.push_back(object); }
	void removeObject(ColliderObject* object) { root->removeObject(object); }
};

