#pragma once
#include "Figure.h"

class Pawn : public Figure
{
public:
	Pawn(int _x, int _y, bool _isWhite);
	virtual ~Pawn() = default; // Destructor
	vector<Tile> GetPossibleMoves(Tile tiles[8][8]) override;
	Pawn() = default;
};