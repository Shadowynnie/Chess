#include "Tile.h"
#include "Figurines/Figure.h"

Tile::Tile(int x, int y, bool isOccupied)
	: _x(x), _y(y), _occupant(nullptr) 
{}

bool Tile::IsOccupied() const { return _occupant != nullptr; }

int Tile::GetX() const { return _x; }
int Tile::GetY() const { return _y; }

void Tile::SetPosition(int x, int y)
{
	_x = x;
	_y = y;
}

void Tile::SetFigure(Figure* figure)
{
	_occupant = figure;
	//_isOccupied = (figure != nullptr);
}

Figure* Tile::GetFigure() const
{
	return _occupant;
}
