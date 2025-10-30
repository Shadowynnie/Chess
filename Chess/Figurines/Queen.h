#pragma once
#include "Figure.h"

class Queen : public Figure
{
	public:
	Queen(int _x, int _y, bool _isWhite);
	~Queen() = default; // Destructor
	vector<Tile*> GetPossibleMoves(Tile tiles[8][8]) override;
	Queen() = default;
};