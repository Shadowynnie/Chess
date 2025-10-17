#include "Knight.h"

Knight::Knight(int _x, int _y, bool _isWhite)
{
	X = _x;
	Y = _y;
	IsWhite = _isWhite;
	
	/*
	if (isWhite)
	{
		sprite = assetManager.knightSpriteWhite; // Use the asset manager to get the white bishop sprite
	}
	else
	{
		sprite = assetManager.knightSpriteBlack; // Use the asset manager to get the black bishop sprite
	}*/
	string key = IsWhite ? "knight_white" : "knight_black";
	Sprite = AssetManager::GetSprite(key);
	Sprite.setPosition(sf::Vector2f(float(X * 128), float(Y * 128)));
}

vector<Tile*> Knight::GetPossibleMoves(Tile tiles[8][8])
{
	vector<Tile*> possibleMoves;
	// All 8 possible moves for a knight
	int moveOffsets[8][2] = {
		{2, 1}, {2, -1}, {-2, 1}, {-2, -1},
		{1, 2}, {1, -2}, {-1, 2}, {-1, -2}
	};
	for (auto& offset : moveOffsets)
	{
		int newX = X + offset[0];
		int newY = Y + offset[1];
		// Check if the new position is within bounds
		if (newX >= 0 && newX < 8 && newY >= 0 && newY < 8)
		{
			Tile* targetTile = &tiles[newX][newY];
			// If the tile is not occupied or occupied by an opponent's piece
			if (!targetTile->IsOccupied() || (targetTile->IsOccupied() && targetTile->GetFigure()->GetColor() != IsWhite))
			{
				possibleMoves.push_back(targetTile);
			}
		}
	}
	// Highlight the tiles
	HighlightPossibleMoves(possibleMoves);
	return possibleMoves;
}