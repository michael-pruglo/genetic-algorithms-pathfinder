#include "Renderer.hpp"
#include <iostream>

void Renderer::renderField(const Field & field)
{
	const sf::Color FREE_CELL_COLOR = sf::Color(50, 53, 50); 
	const sf::Color OBSTACLE_COLOR  = sf::Color(200, 200, 200);
	const int WIDTH = field.columnsAmount(), HEIGHT = field.rowsAmount();

	sf::Image fieldImage;
	fieldImage.create(WIDTH*SCALE, HEIGHT*SCALE, FREE_CELL_COLOR);
	for (auto& obs : field.obstacles)
		for (int i = 0; i < SCALE; ++i)
			for (int j = 0; j < SCALE; ++j)
				fieldImage.setPixel(SCALE*obs.first+i, SCALE*obs.second+j, OBSTACLE_COLOR);

	sf::Sprite fieldSprite;	
	sf::Texture fieldTexture;
	fieldTexture.loadFromImage(fieldImage);
	fieldSprite.setTexture(fieldTexture);
	renderWindow.draw(fieldSprite);
}

void Renderer::renderBot(const Bot & bot, int x, int y, sf::Color color)
{
	x = SCALE*x + SCALE / 2;
	y = SCALE*y + SCALE / 2;
	sf::VertexArray pointer(sf::TriangleStrip, 5);
	pointer[0] = sf::Vertex(sf::Vector2f(x,    y),    color);
	pointer[1] = sf::Vertex(sf::Vector2f(x-10, y-5),  color);
	pointer[2] = sf::Vertex(sf::Vector2f(x+10, y-5),  color);
	pointer[3] = sf::Vertex(sf::Vector2f(x-10, y-25), color);
	pointer[4] = sf::Vertex(sf::Vector2f(x+10, y-25), color);
	renderWindow.draw(pointer);

	renderPath(bot.path, color);
}

void Renderer::renderPath(const Path & path, sf::Color color)
{
	sf::VertexArray trajectory(sf::Points, path.path.size());
	for (auto& point : path.path)
		for (int i = 0; i < SCALE; ++i)
			for (int j = 0; j < SCALE; ++j)
				trajectory.append(sf::Vertex(sf::Vector2f(SCALE*point.first+i, SCALE*point.second+j), color));
	renderWindow.draw(trajectory);
}

void Renderer::renderGoal(int goalX, int goalY, sf::Color color)
{
	color = sf::Color(0, 200, 0, 200);

	sf::CircleShape x(SCALE * 3);
	x.setFillColor(sf::Color(0, 0, 0, 0));
	x.setOutlineColor(color);
	x.setOutlineThickness(4);
	x.setOrigin(x.getRadius(), x.getRadius());
	x.setPosition(SCALE*goalX+SCALE/2, SCALE*goalY+SCALE/2);
	renderWindow.draw(x);

	sf::RectangleShape y(sf::Vector2f(SCALE, SCALE));
	y.setFillColor(color);
	y.setPosition(SCALE*goalX, SCALE*goalY);
	renderWindow.draw(y);
}

void Renderer::renderText(std::string text, int x, int y, int size, sf::Color color)
{
	sf::Font font;
	if (!font.loadFromFile("consolab.ttf"))
	{
		std::cout << "Could not load font consolab.ttf\n";
		return;
	}

	sf::Text t(text, font, size);
	t.setFillColor(color);
	t.setPosition(x, y);
	renderWindow.draw(t);
}
