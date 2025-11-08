#pragma once
#include "Figure.h"

class Pawn : public Figure
{
	bool _enPassantAvailable = false; // To track if en passant is available
public:
	Pawn(int _x, int _y, bool _isWhite);
	~Pawn() = default; // Destructor
	vector<Tile*> GetPossibleMoves(Tile tiles[8][8]) override;
	bool CanPromote() const;
	bool IsEnPassantAvailable() const;
	void EnableEnPassant(bool enable);
	void Move(Tile* tile, Tile* previousTile, vector<Figure*>& enemyFigures, Tile tiles[8][8] = {});
	Pawn() = default;
};