#pragma once
#include "Component.h"
#include "std_lib_facilities.h"
class ComponentB :
	public Component
{
public:
	ComponentB();
	~ComponentB();
	virtual string Name() override;

	double data;
};

