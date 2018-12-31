#pragma once


#include "Bot.hpp"

class AStarBot :
	public Bot
{
public:
	std::pair<int, int> findPath(const Field& field, int startX, int startY, int goalX, int goalY);

	typedef Field::Location Location;
	typedef Field::LocationHasher LocationHasher;
};

