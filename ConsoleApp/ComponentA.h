#pragma once
#include "Component.h"
#include "std_lib_facilities.h"
class ComponentA :
	public Component
{
public:
	ComponentA();
	~ComponentA();
	virtual string Name() override;

	string data;
};

