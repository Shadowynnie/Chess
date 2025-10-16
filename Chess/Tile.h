#pragma once

class Figure; // Forward declaration — avoids circular dependency

class Tile
{
	int _x;
	int _y;
	//bool _isOccupied = false;
	Figure* _occupant = nullptr; // Pointer to the figure occupying the tile
public:
	Tile(int x, int y, bool isOccupied);
	Tile() = default;
	bool IsOccupied() const;
	int GetX() const;
	int GetY() const;
	void SetPosition(int x, int y);
	void SetFigure(Figure* figure); // Set figure pointer
	Figure* GetFigure() const;      // Getter for occupant
};
