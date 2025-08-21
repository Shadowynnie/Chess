#pragma once
#include "Figure.h"

class Rook : public Figure
{
	public:
	Rook(int _x, int _y, bool _isWhite);
	virtual ~Rook() = default; // Destructor
	vector<Tile> getPossibleMoves(const Tile tiles[8][8]) const;
	Rook() = default;
};