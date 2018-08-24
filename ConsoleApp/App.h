#pragma once
#include "Window.h"
#include "Vec2.h"

class App {
public:
	App();
	~App();
	void Initialize();
private:
	int m_maxWindows;
	Window * m_windows;
	Vec2 m_consoleSize;
	Vec2 m_cursor;
	
	void Render();
};