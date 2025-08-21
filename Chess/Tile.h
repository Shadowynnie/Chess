#pragma once


class Tile
{
	
	int x;
	int y;
public:
	bool hasFigure; // To check if there is a chess piece on the tile
	Tile(int _x, int _y, bool _hasFigure = false);
	Tile() = default;
	bool HasFigure() const;
	int GetX() const;
	int GetY() const;
	void SetPosition(int _x, int _y);
};