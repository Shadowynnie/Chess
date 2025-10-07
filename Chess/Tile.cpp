#include "Tile.h"

Tile::Tile(int x, int y, bool hasFigure) 
{
	_x = x;
	_y = y;
	_hasFigure = hasFigure;
}

bool Tile::HasFigure() 
{
	return _hasFigure;
}

void Tile::SetFigure(bool hasFigure)
{
	_hasFigure = hasFigure;
}

int Tile::GetX() const 
{
	return _x;
}

int Tile::GetY() const 
{
	return _y;
}

void Tile::SetPosition(int x, int y) 
{
	_x = x;
	_y = y;
}