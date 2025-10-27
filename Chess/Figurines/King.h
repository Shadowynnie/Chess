#pragma once
#include "Figure.h"

class King : public Figure
{
public:
	King(int _x, int _y, bool _isWhite);
	virtual ~King() = default; // Destructor
	vector<Tile*> GetPossibleMoves(Tile tiles[8][8], vector<Figure*>& enemyFigures) override;
	vector<Tile*> GetPossibleMoves(Tile tiles[8][8]) override;
    bool IsThreatened(Tile tiles[8][8], std::vector<Figure*>& enemyFigures);
	King() = default;
};