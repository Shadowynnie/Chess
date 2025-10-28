#pragma once
#include "Figure.h"

class Rook : public Figure
{
    bool _hasMoved = false; // To track if the rook has moved (for castling rules)
public:
	Rook(int _x, int _y, bool _isWhite);
	~Rook() = default; // Destructor
	vector<Tile*> GetPossibleMoves(Tile tiles[8][8]) override;
	void Move(Tile* tile, Tile* previousTile, Tile tiles[8][8] = {}) override;
	bool HasMoved() const;
	Rook() = default;
};