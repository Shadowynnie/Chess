#pragma once


class Tile
{
	
	int _x;
	int _y;
	bool _hasFigure; // To check if there is a chess piece on the tile
public:
	Tile(int x, int y, bool hasFigure = false);
	Tile() = default;
	bool HasFigure();
	void SetFigure(bool hasFigure);
	int GetX() const;
	int GetY() const;
	void SetPosition(int x, int y);
};