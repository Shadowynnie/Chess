#pragma once
#include "Figure.h"

class Knight : public Figure
{
public:
	Knight(int _x, int _y, bool _isWhite);
	virtual ~Knight() = default; // Destructor
	vector<Tile> GetPossibleMoves(Tile tiles[8][8]) override;
	Knight() = default;
};