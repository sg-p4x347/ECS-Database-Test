#include "Window.h"

Window::Window() {

}
Window::~Window() {

}

char Window::Sample(Vec2 pos) {
	char character = 0;
	if (m_open) {
		if (pos.x >= m_pos.x && pos.x <= m_pos.x + m_size.x && pos.y >= m_pos.y && pos.y <= m_pos.y + m_size.y) {
			// vertical edge
			bool y_axis = false;
			if ((pos.x >= m_pos.x && pos.x <= m_pos.x + 1) || (pos.x <= m_pos.x + m_size.x && pos.x >= m_pos.x + m_size.x -1)) {
				character = '|';
				y_axis = true;
			}
			// horizontal edge
			bool x_axis = false;
			if ((pos.y >= m_pos.y && pos.y <= m_pos.y + 1) || (pos.y <= m_pos.y + m_size.y && pos.y >= m_pos.y + m_size.y - 1)) {
				character = '-';
				x_axis = true;
			}
			if (x_axis && y_axis) {
				// corner
				character = '+';
			} else if (!x_axis && !y_axis) {
				// interior
				character = '#';
			}
		}
		
	}
	return character;
}
void Window::Resize(Vec2 size) {
	m_size = size;
}
void Window::Move(Vec2 pos) {
	m_pos.x += pos.x;
	m_pos.y += pos.y;
}
void Window::Open() {
	m_open = true;
}
void Window::Close() {
	m_open = false;
}