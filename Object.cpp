#include "Object.h"

void* Object::operator new(size_t size)
{
	std::cout << "New new initialiser" << std::endl;
	return::operator new(size);
}

void Object::operator delete(void* p, size_t size)
{
	std::cout << "New new destructor" << std::endl;
	::operator delete(p);
}