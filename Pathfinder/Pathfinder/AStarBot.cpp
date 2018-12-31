#include "AStarBot.hpp"
#include <iostream>
#include <queue>
#include <unordered_map>

std::pair<int, int> AStarBot::findPath(const Field& field, int startX, int startY, int goalX, int goalY)
{
	Location start = { startX, startY }, goal = { goalX, goalY };

	PriorityQueue<Location, int> frontier;
	frontier.put(start, 0);

	std::unordered_map<Location, Location, LocationHasher> came_from;
	std::unordered_map<Location, int, LocationHasher> cost_so_far;
	came_from[start] = start;
	cost_so_far[start] = 0;

	int maxFSize = 0;

	while (!frontier.empty())
	{
		Location current = frontier.get();
		if (current == goal)
			break;

		for (auto next : field.neighbors(current))
		{
			int new_cost = cost_so_far[current] + 1;
			if (!cost_so_far.count(next) || new_cost < cost_so_far[next])
			{
				cost_so_far[next] = new_cost;
				int priority = new_cost + 1.31*field.heuristic(next, goal); //1.0001 to slow down, 1.51 to make faster
				frontier.put(next, priority);
				came_from[next] = current;
			}
		}

		if (maxFSize < frontier.size())
			maxFSize = frontier.size();
	}

	Location current = goal;
	while (current != start)
	{
		path.add(current.first, current.second);
		current = came_from[current];
	}
	path.add(startX, startY);

	return{ maxFSize * (sizeof(Location) + sizeof(int))
		+ came_from.size() * 2 * sizeof(Location)
		+ cost_so_far.size() * (sizeof(Location) + sizeof(int)),

		path.length()
	};
}

