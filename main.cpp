#include <SFML/Graphics.hpp>
#include <algorithm>
#include <math.h>
#include <iostream>
#include <Windows.h>
#include <vector>

#include "Node.h"

const size_t WINDOW_SIZE = 800;
const size_t GRID_COUNT_ROW = 40;
enum Stage {
	PLACING_START=0, PLACING_END, PLACING_WALLS, SOLVING, SOLVED
};

float dist(int x1, int y1, int x2, int y2)
{
	return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

bool has_elem(std::vector<Node>& v, Node& n)
{
	for (Node& node : v) { if (node == n) return true; }
	return false;
}

void show_grid(sf::RenderWindow& window, std::vector<std::vector<Node>>& grid)
{
	for (int x = 0; x < GRID_COUNT_ROW; x++)
	{
		for (int y = 0; y < GRID_COUNT_ROW; y++)
		{
			grid[x][y].show(window);
		}
	}
}

void remove_from_vector(std::vector<Node>& v, Node& node)
{
	for (int i = 0; i < v.size(); i++)
	{
		if (v[i] == node) v.erase(v.begin()+i);
	}
}

std::vector<Node> get_neighbors(std::vector<std::vector<Node>>& grid, Node& n)
{
	int x = n.x;
	int y = n.y;

	std::vector<Node> neighbors;

	if (x > 0) neighbors.push_back(grid[x - 1][y]);
	if (y > 0) neighbors.push_back(grid[x][y - 1]);
	if (x < GRID_COUNT_ROW - 1) neighbors.push_back(grid[x + 1][y]);
	if (y < GRID_COUNT_ROW - 1) neighbors.push_back(grid[x][y + 1]);

	if (x > 0 && y > 0) neighbors.push_back(grid[x - 1][y - 1]);
	if (x < GRID_COUNT_ROW - 1 && y < GRID_COUNT_ROW - 1) neighbors.push_back(grid[x + 1][y + 1]);
	if (x > 0 && y < GRID_COUNT_ROW - 1) neighbors.push_back(grid[x - 1][y + 1]);
	if (x < GRID_COUNT_ROW - 1 && y > 0) neighbors.push_back(grid[x + 1][y - 1]);

	return neighbors;
}

std::ostream& operator<<(std::ostream& out, std::vector<Node> v)
{
	for (auto el : v) std::cout << "(" << el.x << "," << el.y << ")\n";
	return out;
}

std::vector<Node> get_path(std::vector<std::vector<Node>>& grid, Node current)
{
	std::vector<Node> path;
	Node temp = current;
	while (grid[temp.x][temp.y].came_from != nullptr)
	{
		path.push_back(temp);
		temp = *grid[temp.x][temp.y].came_from;
	}
	path.push_back(temp);
	return path;
}

int main()
{
	Stage current = PLACING_START;
	sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE, WINDOW_SIZE), "Szybka papierzowa przypominajka");

	std::vector<std::vector<Node>> grid;

	for (int x = 0; x < GRID_COUNT_ROW; x++)
	{
		grid.push_back(std::vector<Node>());
		for (int y = 0; y < GRID_COUNT_ROW; y++)
		{
			grid[x].push_back(Node(x, y, WINDOW_SIZE / GRID_COUNT_ROW, NodeType::EMPTY));
		}
	}
	std::vector<Node> open_set{};
	Node& end_node = grid[GRID_COUNT_ROW - 1][GRID_COUNT_ROW - 1];
	Node& start_node = grid[0][0];

	bool finished = false;

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) window.close();
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		{
			if (current == Stage::PLACING_WALLS) current = Stage::SOLVING;
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
		{
			if (current == SOLVED)
			{
				current = PLACING_START;
				grid.clear();
				open_set.clear();
				finished = false;
				for (int x = 0; x < GRID_COUNT_ROW; x++)
				{
					grid.push_back(std::vector<Node>());
					for (int y = 0; y < GRID_COUNT_ROW; y++)
					{
						grid[x].push_back(Node(x, y, WINDOW_SIZE / GRID_COUNT_ROW, NodeType::EMPTY));
					}
				}
			}
		}

		if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			sf::Vector2i pos = sf::Mouse::getPosition(window);
			int x = pos.x;
			int y = pos.y;

			if (x >= 0 && x < WINDOW_SIZE && y >= 0 && y < WINDOW_SIZE)
			{
				int num = WINDOW_SIZE / GRID_COUNT_ROW;

				switch (current)
				{
				case PLACING_START:
					grid[x / num][y / num].type = NodeType::START;

					grid[x / num][y / num].g = 0;
					open_set.push_back(grid[x / num][y / num]);

					Sleep(101);
					current = Stage::PLACING_END;
					break;

				case PLACING_END:
					grid[x / num][y / num].type = NodeType::END;
					end_node = grid[x / num][y / num];
					grid[open_set[0].x][open_set[0].y].f = dist(grid[open_set[0].x][open_set[0].y].x, grid[open_set[0].x][open_set[0].y].y, end_node.x, end_node.y);

					Sleep(101);
					current = Stage::PLACING_WALLS;
					break;
				case PLACING_WALLS:
					grid[x / num][y / num].type = NodeType::WALL;
					break;
				}
			}
		}


		window.clear();


		if (!open_set.empty() && !finished && current == Stage::SOLVING)
		{
			int lowest_f = 0;
			for (int i = 0; i < open_set.size(); i++)
			{
				if (open_set[i].f < open_set[lowest_f].f) lowest_f = i;
			}

			Node current_node = open_set[lowest_f];

			grid[current_node.x][current_node.y].type = NodeType::CURRENT;

			if (grid[current_node.x][current_node.y] == grid[end_node.x][end_node.y])
			{
				current = Stage::SOLVED;
				finished = true;
			}

			remove_from_vector(open_set, grid[current_node.x][current_node.y]);

			std::vector<Node> neighbors = get_neighbors(grid, grid[current_node.x][current_node.y]);

			for (Node n : neighbors)
			{
				float temp_g = grid[current_node.x][current_node.y].g + dist(n.x, n.y, current_node.x, current_node.y);
				if (temp_g < grid[n.x][n.y].g)
				{
					grid[n.x][n.y].came_from = &grid[current_node.x][current_node.y];
					grid[n.x][n.y].g = temp_g;
					grid[n.x][n.y].f = temp_g + dist(n.x, n.y, end_node.x, end_node.y);

					if (!has_elem(open_set, n) && grid[n.x][n.y].type != NodeType::WALL) open_set.push_back(grid[n.x][n.y]);
				}
			}
		}

		if (finished) {
			std::vector<Node> path = get_path(grid, grid[end_node.x][end_node.y]);
			for (Node n : path)
			{
				grid[n.x][n.y].type = NodeType::END;
			}
		}

		for (Node n : open_set)
		{
			grid[n.x][n.y].type = NodeType::PATH;
		}

		show_grid(window, grid);

		window.display();
	}
}