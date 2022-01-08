#pragma once
#include <math.h>
#include <random>

enum NodeType {
	EMPTY = 0, CURRENT, WALL, START, END, PATH
};

struct Node
{
	int x, y;
	int size;
	float f, g, h;

	NodeType type;

	Node(int x, int y, int size, NodeType type)
		: x(x), y(y), size(size), f(9999999999.0f), g(99999999999.0f), h(0)
	{
		this->type = type;

		/*std::random_device dev;
		std::mt19937 rng(dev());
		std::uniform_real_distribution<double> dist(0.0f, 1.0f);

		if (dist(rng) < 0.3) this->type = NodeType::WALL;*/
	}

	bool operator==(const Node& rhs)
	{
		return rhs.x == x && rhs.y == y;
	}

	void show(sf::RenderWindow& window)
	{
		sf::RectangleShape node(sf::Vector2f(size-1, size-1));
		node.setFillColor(get_color(this->type));
		node.setPosition(sf::Vector2f(x * size, y * size));
		window.draw(node);
	}

	Node* came_from = nullptr;

private:
	sf::Color get_color(NodeType type)
	{
		switch (type) {
		case EMPTY:
			return sf::Color(255, 255, 255, 255);
			break;
		case WALL:
			return sf::Color(0, 0, 0, 255);
			break;
		case START:
			return sf::Color(0, 255, 0, 255);
			break;
		case END:
			return sf::Color(255, 0, 0, 255);
			break;
		case PATH:
			return sf::Color(0, 105, 0, 255);
			break;
		case CURRENT:
			return sf::Color(0, 105, 255, 255);
			break;
		}
	}
};