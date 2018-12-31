#include "Field.hpp"
#include <fstream>
#include <iostream>
#include <SFML/Graphics.hpp>

void Field::resize2DVector(std::vector<std::vector<int>>& v, int rows, int columns)
{
	v.clear();
	v.resize(rows);
	for (auto& row : v)
		row.resize(columns);
}

Field::Field(std::string filename)
{
	setFile(filename);
}

void Field::setFile(std::string filename)
{
	obstacles.clear();
	
	sf::Image field;
	if (!field.loadFromFile(filename))
		std::cout << "Could not load picture " << filename << " for the field.\n";

	n = field.getSize().y;
	m = field.getSize().x;
	resize2DVector(up, n, m);
	resize2DVector(down, n, m);
	resize2DVector(right, n, m);
	resize2DVector(left, n, m);

	for (int i = 0; i < m; ++i)
		for (int j = 0; j < n; ++j)
		{
			if (field.getPixel(i, j) == sf::Color::White)
				obstacles.push_back({ i, j });
			else
			{
				up[j][i] = j ? up[j-1][i]+1 : 1;
				left[j][i] = i ? left[j][i-1]+1 : 1;
			}
		}
	for (int i = m-1; i >= 0; --i)
		for (int j = n-1; j >= 0; --j)
			if (up[j][i] != 0)
			{
				down[j][i] = j<n-1 ? down[j+1][i]+1 : 1;
				right[j][i] = i<m-1 ? right[j][i+1]+1 : 1;
			}
}

std::vector<Field::Location> Field::neighbors(Location loc) const
{
	std::vector<Location> result;

	int a, b;
	a = loc.first - 1; b = loc.second;  if (withinBounds(a, b) && walkable(a, b)) result.push_back({ a, b });
	a = loc.first; b = loc.second - 1;  if (withinBounds(a, b) && walkable(a, b)) result.push_back({ a, b });
	a = loc.first; b = loc.second + 1;  if (withinBounds(a, b) && walkable(a, b)) result.push_back({ a, b });
	a = loc.first + 1; b = loc.second;  if (withinBounds(a, b) && walkable(a, b)) result.push_back({ a, b });

	return result;
}