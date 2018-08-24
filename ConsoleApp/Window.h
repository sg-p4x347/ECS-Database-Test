#pragma once
#include "Vec2.h"

class Window {
public:
	Window();
	~Window();

	int m_index; // rendering index

	char Sample(Vec2 pos);
	void Resize(Vec2 size);
	void Move(Vec2 pos);
	void Open();
	void Close();
private:
	bool m_open = false;
	Vec2 m_pos = Vec2(0,0);
	Vec2 m_size = Vec2(10,10);
};