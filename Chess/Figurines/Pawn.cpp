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

vector<Tile> Pawn::GetPossibleMoves(Tile tiles[8][8])
{
	vector<Tile> possibleMoves;
	int direction = IsWhite ? -1 : 1; // White moves up, black moves down
	int startRow = IsWhite ? 6 : 1; // Starting row for pawns
	// Move forward by one square
	int newX = X + direction;
	if (newX >= 0 && newX < 8 && !tiles[newX][Y].HasFigure())
	{
		possibleMoves.push_back(tiles[newX][Y]);
	}
	// Move forward by two squares from the starting position
	if (X == startRow && !tiles[newX][Y].HasFigure() && !tiles[newX + direction][Y].HasFigure())
	{
		possibleMoves.push_back(tiles[newX + direction][Y]);
	}
	// Capture diagonally
	for (int dx = -1; dx <= 1; dx += 2)
	{
		newX = X + direction;
		int newY = Y + dx;
		if (newY >= 0 && newY < 8 && newX >= 0 && newX < 8)
		{
			if (tiles[newX][newY].HasFigure() &&
				((IsWhite && !tiles[newX][newY].HasFigure()) || (!IsWhite && tiles[newX][newY].HasFigure())))
			{
				possibleMoves.push_back(tiles[newX][newY]);
			}
		}
	}
	return possibleMoves;
}