#pragma once
#include "Figure.h"


class Bishop : public Figure
{
public:
	Bishop(int _x, int _y, bool _isWhite);
	virtual ~Bishop() = default; // Destructor
	vector<Tile> getPossibleMoves(const Tile tiles[8][8]) const;
	Bishop() = default;
};