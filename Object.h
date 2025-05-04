#pragma once
#include <iostream>
class Object
{
public:
	static void* operator new(size_t size);
	static void operator delete(void* p, size_t size);

	Object()
	{
		std::cout << "Object initialiser" << std::endl;
	}

	~Object()
	{
		std::cout << "Object destructor" << std::endl;
	}
};

