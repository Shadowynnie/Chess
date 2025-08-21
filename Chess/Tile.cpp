#include "Tile.h"

Tile::Tile(int _x, int _y, bool _hasFigure) 
{
	x = _x;
	y = _y;
	hasFigure = _hasFigure;
}

bool Tile::HasFigure() const 
{
	return hasFigure;
}

int Tile::GetX() const 
{
	return x;
}

int Tile::GetY() const 
{
	return y;
}

void Tile::SetPosition(int _x, int _y) 
{
	x = _x;
	y = _y;
}