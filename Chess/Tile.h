#pragma once
class Tile
{
	bool hasFigure; // To check if there is a chess piece on the tile
	int x;
	int y;
public:
	Tile(int _x, int _y, bool _hasFigure = false);
	Tile() : hasFigure(false), x(0), y(0) {}
	bool HasFigure() const;
	void SetHasFigure(bool _hasFigure);
	int GetX() const;
	int GetY() const;
	void SetPosition(int _x, int _y);
};