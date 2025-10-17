#include "Pawn.h"

Pawn::Pawn(int _x, int _y, bool _isWhite)
{
	X = _x;
	Y = _y;
	IsWhite = _isWhite;

	/*
	if (isWhite)
	{
		sprite = assetManager.pawnSpriteWhite; // Use the asset manager to get the white bishop sprite
	}
	else
	{
		sprite = assetManager.pawnSpriteBlack; // Use the asset manager to get the black bishop sprite
	}*/
	string key = IsWhite ? "pawn_white" : "pawn_black";
	Sprite = AssetManager::GetSprite(key);
	Sprite.setPosition(sf::Vector2f(float(X * 128), float(Y * 128)));
}

vector<Tile*> Pawn::GetPossibleMoves(Tile tiles[8][8])
{
	vector<Tile*> possibleMoves;
	int direction = IsWhite ? 1 : -1; // White moves "up" (increasing Y), Black moves "down" (decreasing Y)
	// Move forward one tile
	if (Y + direction >= 0 && Y + direction < 8 && !tiles[X][Y + direction].IsOccupied())
	{
		possibleMoves.push_back(&tiles[X][Y + direction]);
		// Move forward two tiles from starting position
		if ((IsWhite && Y == 1) || (!IsWhite && Y == 6))
		{
			if (!tiles[X][Y + 2 * direction].IsOccupied())
			{
				possibleMoves.push_back(&tiles[X][Y + 2 * direction]);
			}
		}
	}
	// Capture diagonally
	if (X - 1 >= 0 && Y + direction >= 0 && Y + direction < 8 && tiles[X - 1][Y + direction].IsOccupied() &&
		tiles[X - 1][Y + direction].GetFigure()->GetColor() != IsWhite)
	{
		possibleMoves.push_back(&tiles[X - 1][Y + direction]);
	}
	if (X + 1 < 8 && Y + direction >= 0 && Y + direction < 8 && tiles[X + 1][Y + direction].IsOccupied() &&
		tiles[X + 1][Y + direction].GetFigure()->GetColor() != IsWhite)
	{
		possibleMoves.push_back(&tiles[X + 1][Y + direction]);
	}
	return possibleMoves;
}