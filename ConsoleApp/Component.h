#pragma once
#include "std_lib_facilities.h"
class Component
{
public:
	Component();
	Component(int id);
	~Component();
	virtual string Name() = 0;
	unsigned int ID;
};

