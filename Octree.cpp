#include "Octree.h"

template <typename T>
constexpr T clamp(T value, T min, T max) {
	if (value < min) return min;
	if (value > max) return max;
	return value;
}

bool BoundingBox::doesItContain(Vec3 position) {
	Vec3 newPos;
	newPos.x = clamp(position.x, minX, maxX);
	newPos.y = clamp(position.y, minX, maxX);
	newPos.z = clamp(position.z, minZ, maxZ);

	return (newPos.x >= minBound.x && newPos.x <= maxBound.x &&
		newPos.y >= minBound.y && newPos.y <= maxBound.y &&
		newPos.z >= minBound.z && newPos.z <= maxBound.z);
}

void OctreeNode::OctreeSetup() {
	if (depth < maxDepth) {
		float newHS = boundingBox->halfSize/2;
		childrenNode[0] = new OctreeNode(new BoundingBox(boundingBox->centre + Vec3(newHS, newHS, newHS), newHS), depth+1, maxDepth);
		childrenNode[1] = new OctreeNode(new BoundingBox(boundingBox->centre + Vec3(newHS, -newHS, newHS), newHS), depth+1, maxDepth);
		childrenNode[2] = new OctreeNode(new BoundingBox(boundingBox->centre + Vec3(newHS, -newHS, -newHS), newHS), depth+1, maxDepth);
		childrenNode[3] = new OctreeNode(new BoundingBox(boundingBox->centre + Vec3(newHS, newHS, -newHS), newHS), depth+1, maxDepth);
		childrenNode[4] = new OctreeNode(new BoundingBox(boundingBox->centre + Vec3(-newHS, newHS, newHS), newHS), depth+1, maxDepth);
		childrenNode[5] = new OctreeNode(new BoundingBox(boundingBox->centre + Vec3(-newHS, -newHS, newHS), newHS), depth+1, maxDepth);
		childrenNode[6] = new OctreeNode(new BoundingBox(boundingBox->centre + Vec3(-newHS, -newHS, -newHS), newHS), depth+1, maxDepth);
		childrenNode[7] = new OctreeNode(new BoundingBox(boundingBox->centre + Vec3(-newHS, newHS, -newHS), newHS), depth+1, maxDepth);
	}
}

void OctreeNode::initObjects(std::vector<ColliderObject*> objectList) {
	for (ColliderObject* object : objectList) {
		objects.push_back(object);
		if (maxDepth == 0) { continue; }
		for (OctreeNode* node : childrenNode) {
			if (node->boundingBox->doesItContain(object->position)) {
				node->objects.push_back(object);
				break;
			}
		}
	}
	if (maxDepth == 0) { return; }
	for (OctreeNode* node : childrenNode) {
		if (node->objects.size() == 0) { continue; }
		node->initObjects();
	}
}

void OctreeNode::initObjects() {
	for (ColliderObject* object : objects) {
		for (OctreeNode* node : childrenNode) {
			if (depth < maxDepth) {
				if (node->boundingBox->doesItContain(object->position)) {
					node->objects.push_back(object);
					break;
				}
			}

		}
	}
	if (depth != maxDepth && objects.size() != 0) {
		for (OctreeNode* node : childrenNode) {
			node->initObjects();
		}
	}
}

void OctreeNode::getObjectList(CollStruct* collStruct) {
	if (objects.size() == 0) { return; }
	else if (depth == maxDepth && objects.size() != 0) { collStruct->collObjList.push_back(&objects); return; }
	for (OctreeNode* node : childrenNode) {
		node->getObjectList(collStruct);
	}
}

int OctreeNode::validateObjects(bool setup)
{
	if (depth == maxDepth) { return true; }
	if (setup == true) {
		for (ColliderObject* object : objects) {
			for (OctreeNode* child : childrenNode) {
				auto it = std::find(child->objects.begin(), child->objects.end(), object);
				if (it != child->objects.end())
				{
					int result = child->validateObjects(object);
					if (result == 1) { break; }
					else if (result == 2)
					{
						sortSingleObjectStart(object);
					}
				}
				else { continue;; }
			}
		}
	}
	return true;
}
int OctreeNode::validateObjects(ColliderObject* object)
{
	if (depth == maxDepth) {
		if (!this->boundingBox->doesItContain(object->position)) {
			objects.erase(std::remove(objects.begin(), objects.end(), object), objects.end());
			return 2;
		}
		else { return 1; }
	}
	for (OctreeNode* child : childrenNode) {
		auto it = std::find(child->objects.begin(), child->objects.end(), object);
		if (it != child->objects.end())
		{
			int result = child->validateObjects(object);
			if (result == 1) { return 1; }
			if (result == 2)
			{
				objects.erase(std::remove(objects.begin(), objects.end(), object), objects.end());
				return 2;
			}
		}
		else { continue; }
	}
	return 2;
}

void OctreeNode::sortSingleObjectStart(ColliderObject* object)
{
	if (depth <= maxDepth)
	{
		for (OctreeNode* node : childrenNode) {
			if (node->boundingBox->doesItContain(object->position)) {
				if (depth <= maxDepth) {
					node->sortSingleObject(object);
				}
				break;
			}
		}
	}
}
void OctreeNode::sortSingleObject(ColliderObject* object)
{
	objects.push_back(object);
	if (depth != maxDepth)
	{
		for (OctreeNode* node : childrenNode) {
			if (node->boundingBox->doesItContain(object->position)) {
				if (depth <= maxDepth) {
					node->sortSingleObject(object);
				}
				break;
			}
		}
	}
}

int OctreeNode::removeObject(ColliderObject* object)
{
	if (depth == maxDepth) {
		auto it = std::find(objects.begin(), objects.end(), object);
		if (it != objects.end())
		{
			objects.erase(std::remove(objects.begin(), objects.end(), object), objects.end());
			return 2;
		}
	}
	for (OctreeNode* child : childrenNode) {
		auto it = std::find(child->objects.begin(), child->objects.end(), object);
		if (it != child->objects.end())
		{
			int result = child->removeObject(object);
			if (result == 2)
			{
				objects.erase(std::remove(objects.begin(), objects.end(), object), objects.end());
				return 2;
			}
		}
		else { continue; }
	}
}

OctreeNode::OctreeNode(BoundingBox* bounds, int depth, int maxDepth) : boundingBox(bounds), depth(depth), maxDepth(maxDepth) {
	OctreeSetup();
}




