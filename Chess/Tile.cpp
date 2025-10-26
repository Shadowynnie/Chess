#include "Tile.h"
#include "Figurines/Figure.h"

Tile::Tile(int x, int y, bool isOccupied)
    : _x(x), _y(y), _occupant(nullptr), _isHighlighted(false), TileShape(sf::Vector2f(128.f, 128.f)), HighLight(20.f)
{}

bool Tile::IsOccupied() const { return _occupant != nullptr; }
bool Tile::IsHighlighted() const { return _isHighlighted; }
bool Tile::IsInCheck() const { return _isInCheck; }
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

void Tile::Highlight(bool isHighlighted)
{
    _isHighlighted = isHighlighted;
}

void Tile::SetInCheck(bool isInCheck)
{
	_isInCheck = isInCheck;
}

Figure* Tile::GetFigure() const
{
	return _occupant;
}
