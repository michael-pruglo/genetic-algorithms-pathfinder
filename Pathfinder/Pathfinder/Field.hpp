#pragma once

#include<string>
#include<vector>

/*
 * Field, on which the pathfinding takes place
 * Represented by obstacles coordinates array
 */
class Field
{
	friend class Renderer;

	std::vector<std::pair<int, int>> obstacles;
	int n, m;
	std::vector<std::vector<int>> up, down, right, left; //amount of cells to an obstacle
	void resize2DVector(std::vector<std::vector<int>>& v, int rows, int columns);
public:
	Field(int width, int height) : n(height), m(width) {}
	Field(std::string filename);
public:
	void setFile(std::string filename);
	bool withinBounds(int x, int y) const { return x >= 0 && x < m && y >= 0 && y < n; }
	bool walkable(int x, int y) const { return up[y][x]; }
	int upToObstacle(int x, int y) const { return up[y][x]; }
	int downToObstacle(int x, int y) const { return down[y][x]; }
	int rightToObstacle(int x, int y) const { return right[y][x]; }
	int leftToObstacle(int x, int y) const { return left[y][x]; }
	int rowsAmount() const { return n; }
	int columnsAmount() const { return m; }

public:
	typedef std::pair<int, int> Location;
	struct LocationHasher
	{
		std::size_t operator()(const Location& k) const
		{
			return std::hash<int>()(k.first * 1000000 + k.second);
		}
	};
	std::vector<Location> neighbors(Location loc) const;
	inline int heuristic(Location a, Location b) const
	{
		//Manhattan
		int dx = std::abs(a.first - b.first), dy = std::abs(a.second - b.second);
		return dx + dy;
	}
};