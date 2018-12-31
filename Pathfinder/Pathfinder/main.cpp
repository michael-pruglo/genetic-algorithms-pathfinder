#include <iostream>
#include <sstream>
#include <cstdlib>
#include "Field.hpp"
#include "GeneticBot.hpp"
#include "AStarBot.hpp"
#include "Renderer.hpp"
#include <SFML/Graphics.hpp>

int main()
{
	const int WIN_HEIGHT = 700, WIN_WIDTH = 1200, FIELD_WIDTH = 1000, TESTS_AMOUNT = 3;
	sf::RenderWindow window(sf::VideoMode(WIN_WIDTH, WIN_HEIGHT), "Pathfinding");

	struct {
		std::string fieldFilename;
		int startXAStar, startYAStar;
		int startXGenetic, startYGenetic;
		int goalX, goalY;
	} configurations[TESTS_AMOUNT] = {
		{ "field1.bmp", 5, 60, 5, 60, 180, 61 },
		{ "field2.bmp", 5, 60, 5, 60, 180, 61 },
		{ "field3.bmp", 5, 60, 5, 60, 180, 61 }
	};
	int currentConfiguration = 0;

	Field field(configurations[currentConfiguration].fieldFilename);
	AStarBot ashley;
	GeneticBot gina;

	struct Stats
	{
		int timeMicroseconds;
		std::string time;
		int memoryBytes;
		std::string memory;
		int lengthCells;
		std::string length;
	} ginaStats, ashleyStats;
	std::stringstream ss;

	Renderer renderer(window, 5);
	
	bool aFound = false, gFound = false;
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyPressed)
			{
				switch (event.key.code)
				{
					case sf::Keyboard::Right:
						if (currentConfiguration < TESTS_AMOUNT - 1)
						{
							currentConfiguration++;
							field.setFile(configurations[currentConfiguration].fieldFilename);
							aFound = gFound = false;
							ashley.reset();
							ashleyStats = Stats();
							gina.reset();
							ginaStats = Stats();
						}
						break;
					case sf::Keyboard::Left:
						if (currentConfiguration > 0)
						{
							currentConfiguration--;
							field.setFile(configurations[currentConfiguration].fieldFilename);
							aFound = gFound = false;
							ashley.reset();
							ashleyStats = Stats();
							gina.reset();
							ginaStats = Stats();
						}
						break;
					case sf::Keyboard::R:
						field.setFile(configurations[currentConfiguration].fieldFilename);
						aFound = gFound = false;
						ashley.reset();
						ashleyStats = Stats();
						gina.reset();
						ginaStats = Stats();
						break;
					default:
						break;
				}
			}
		}

		window.clear();
		
		renderer.renderField(field);
		renderer.renderBot(ashley, configurations[currentConfiguration].startXAStar, configurations[currentConfiguration].startYAStar, sf::Color(240, 30, 30, 160));
		renderer.renderText("Test #"/*+std::string(itoa(currentConfiguration+1, new char[5], 10))*/, FIELD_WIDTH + 10, 10, 40, sf::Color(200, 200, 200));
		renderer.renderText("A*", FIELD_WIDTH+10, 70, 40, sf::Color(200, 66, 66));
		renderer.renderText(ashleyStats.time, FIELD_WIDTH + 10, 130, 20, sf::Color(200, 66, 66));
		renderer.renderText(ashleyStats.memory, FIELD_WIDTH + 10, 160, 20, sf::Color(200, 66, 66));
		renderer.renderText(ashleyStats.length, FIELD_WIDTH + 10, 190, 20, sf::Color(200, 66, 66));
		renderer.renderBot(gina, configurations[currentConfiguration].startXGenetic, configurations[currentConfiguration].startYGenetic, sf::Color(68, 140, 250, 160));
		renderer.renderText("Genetic", FIELD_WIDTH + 10, 350, 40, sf::Color(73, 120, 214));
		renderer.renderText(ginaStats.time, FIELD_WIDTH + 10, 410, 20, sf::Color(73, 120, 214));
		renderer.renderText(ginaStats.memory, FIELD_WIDTH + 10, 440, 20, sf::Color(73, 120, 214));
		renderer.renderText(ginaStats.length, FIELD_WIDTH + 10, 470, 20, sf::Color(73, 120, 214));
		renderer.renderGoal(configurations[currentConfiguration].goalX, configurations[currentConfiguration].goalY, sf::Color(70, 240, 48, 128));

		window.display();

		if (!aFound)
		{
			sf::Clock clock;
			auto results = ashley.findPath(
				field,
				configurations[currentConfiguration].startXAStar,
				configurations[currentConfiguration].startYAStar,
				configurations[currentConfiguration].goalX,
				configurations[currentConfiguration].goalY
			);
			ashleyStats.memoryBytes = results.first;
			ashleyStats.lengthCells = results.second;
			ashleyStats.timeMicroseconds = clock.getElapsedTime().asMicroseconds();
			ss << std::noshowpos << ashleyStats.timeMicroseconds/1000.0 << " ms";
			ashleyStats.time = ss.str();
			ss.str("");
			ss << std::noshowpos << ashleyStats.memoryBytes << " b";
			ashleyStats.memory = ss.str();
			ss.str("");
			ss << std::noshowpos << ashleyStats.lengthCells << " cells";
			ashleyStats.length = ss.str();
			ss.str("");
			aFound = true; 
		}
		if (!gFound)
		{
			sf::Clock clock;
			auto results = gina.findPath(
				field,
				configurations[currentConfiguration].startXGenetic,
				configurations[currentConfiguration].startYGenetic,
				configurations[currentConfiguration].goalX,
				configurations[currentConfiguration].goalY
			);
			ginaStats.memoryBytes = results.first;
			ginaStats.lengthCells = results.second;
			ginaStats.timeMicroseconds = clock.getElapsedTime().asMicroseconds();
			ss << std::noshowpos << ginaStats.timeMicroseconds / 1000.0 << " ms";
			ss << std::showpos << " (" << int(100.0*(ginaStats.timeMicroseconds - ashleyStats.timeMicroseconds)/ashleyStats.timeMicroseconds) << "%)";
			ginaStats.time = ss.str();
			ss.str("");
			ss << std::noshowpos << ginaStats.memoryBytes << " b";
			ss << std::showpos << " (" << int(100.0*(ginaStats.memoryBytes - ashleyStats.memoryBytes) / ashleyStats.memoryBytes) << "%)";
			ginaStats.memory = ss.str();
			ss.str("");
			ss << std::noshowpos << ginaStats.lengthCells << " cells";
			ss << std::showpos << " (" << int(100.0*(ginaStats.lengthCells - ashleyStats.lengthCells) / ashleyStats.lengthCells) << "%)";
			ginaStats.length = ss.str();
			ss.str("");
			gFound = true;
		}
	}

	return 0;
}	