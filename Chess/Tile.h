#pragma once
#include <SFML/Graphics.hpp>
class Figure; // Forward declaration — avoids circular dependency

class Tile
{
	int _x;
	int _y;
	bool _isHighlighted = false;
	Figure* _occupant = nullptr; // Pointer to the figure occupying the tile
public:
	sf::RectangleShape TileShape;
	sf::CircleShape HighLight;

	Tile(int x, int y, bool isOccupied);
	Tile() = default;
	bool IsOccupied() const;
    bool IsHighlighted() const;
	int GetX() const;
	int GetY() const;
	void SetPosition(int x, int y);
	void SetFigure(Figure* figure); // Set figure pointer
    void Highlight(bool isHighlighted); // Highlight the tile
	Figure* GetFigure() const;      // Getter for occupant
};
