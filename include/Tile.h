#pragma once
#include <SFML/Graphics.hpp>
class Figure; // Forward declaration — avoids circular dependency

class Tile
{
	int _x;
	int _y;
	bool _isHighlighted = false;
	bool _isInCheck = false;
	Figure* _occupant = nullptr; // Pointer to the figure occupying the tile
public:
	sf::RectangleShape TileShape;
	sf::CircleShape HighLight;

	Tile(int x, int y, bool isOccupied);
	Tile() = default;
	bool IsOccupied() const;
	bool IsHighlighted() const;
	bool IsInCheck() const; // Get check status
	int GetX() const;
	int GetY() const;
	void SetPosition(int x, int y);
	void SetFigure(Figure* figure); // Set figure pointer
	void Highlight(bool isHighlighted); // Highlight the tile
	void SetInCheck(bool isInCheck); // Set check status
	Figure* GetFigure() const;      // Getter for occupant
};
