#pragma once

#include <SFML/Graphics.hpp>
#include "Field.hpp"
#include "Bot.hpp"
#include "Path.hpp"

class Renderer
{
	sf::RenderWindow& renderWindow;
	const int SCALE;
public:
	Renderer(sf::RenderWindow& renderWindow, const int scale)
		: renderWindow(renderWindow), SCALE(scale)
	{}
public:
	void renderField(const Field& field);
	void renderBot(const Bot& bot, int x, int y, sf::Color color = sf::Color::Black);
	void renderPath(const Path& path, sf::Color color = sf::Color::Black);
	void renderGoal(int goalX, int goalY, sf::Color color = sf::Color::Black);
	void renderText(std::string text, int x, int y, int size = 30, sf::Color color = sf::Color::Black);
};

