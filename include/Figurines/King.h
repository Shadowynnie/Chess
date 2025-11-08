#pragma once
#include "Figure.h"

class King : public Figure
{
	bool _hasMoved = false; // To track if the king has moved (for castling rules)
public:
	King(int _x, int _y, bool _isWhite);
	~King() = default; // Destructor
	bool CanCastle(Tile tiles[8][8], vector<Figure*>& enemyFigures, int rookX, int targetKingX);
	vector<Tile*> GetPossibleMoves(Tile tiles[8][8], vector<Figure*>& enemyFigures) override;
	vector<Tile*> GetPossibleMoves(Tile tiles[8][8]) override;
	void Move(Tile* tile, Tile* previousTile, Tile tiles[8][8] = {}) override;
	bool IsThreatened(Tile tiles[8][8], std::vector<Figure*>& enemyFigures);
	King() = default;
};