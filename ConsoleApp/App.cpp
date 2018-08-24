#include "std_lib_facilities.h"
#include "App.h"

App::App() {

}
App::~App() {
	delete[] m_windows;
}
void App::Render() {
	if (system("CLS")) system("clear");
	// for every line
	for (int line = 0; line < m_consoleSize.y; line++) {
		// for every character
		for (int index = 0; index < m_consoleSize.x; index++) {
			char character = ' ';
			// sample each window for a character
			for (int z = 0; z < m_maxWindows; z++) {
				// sample window
				char sample = m_windows[z].Sample(Vec2(index, line));
				if (sample) {
					character = sample;
				}
			}
			// sample for cursor
			if (index == m_cursor.x && line == m_cursor.y) {
				character = '^';
			}
			cout << character;
		}
		cout << "\n";
	}
}
void App::Initialize() {
	m_consoleSize = Vec2(128, 128);
	m_cursor = Vec2(30, 30);
	m_maxWindows = 10;
	m_windows = new Window[m_maxWindows];
	// create the first window
	m_windows[0].Resize(Vec2(30, 30));
	m_windows[0].Open();

	m_windows[1].Resize(Vec2(40, 40));
	m_windows[1].Move(Vec2(20, 10));
	m_windows[1].Open();
	Render();
	while (true) {
		char input = cin.get();
		if (input == 'd') {
			m_windows[1].Move(Vec2(1, 0));
			Render();
		}
		else {
			break;
		}
	}

}