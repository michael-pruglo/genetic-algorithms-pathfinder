#pragma once

#include <vector>

/*
 * Path represented as sorted sequence of pairs of coordinates
 * First step is last in the sequence
 * Goal is first in the sequence
 */
class Path
{
	friend class Renderer;

	std::vector<std::pair<int, int>> path;
public:
	void add(int x, int y) { path.push_back({ x,y }); }
	void clear() { path.clear(); }
	void proceed() { path.pop_back(); }
	inline int length() const { return path.size(); }
	int currentX() const { return path.back().first; }
	int currentY() const { return path.back().second; }
};

