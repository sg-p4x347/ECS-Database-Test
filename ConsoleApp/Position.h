#pragma once
#include "Component.h"
struct Vector3 {
	Vector3() : Vector3(0.f,0.f,0.f) {}
	Vector3(float p_x, float p_y, float p_z) : x(p_x), y(p_y), z(p_z) {}

	float x;
	float y;
	float z;
};

class Position :
	public Component
{
public:
	Position();
	Position(Vector3 pos);
	~Position();

	Vector3 Pos;
	Vector3 Rot;

	// Inherited via Component
	virtual string Name() override;
};

