#include "Pawn.h"

Pawn::Pawn(int _x, int _y, bool _isWhite)
{
	x = _x;
	y = _y;
	isWhite = _isWhite;

	/*
	if (isWhite)
	{
		sprite = assetManager.pawnSpriteWhite; // Use the asset manager to get the white bishop sprite
	}
	else
	{
		sprite = assetManager.pawnSpriteBlack; // Use the asset manager to get the black bishop sprite
	}*/
	string key = isWhite ? "pawn_white" : "pawn_black";
	sprite = AssetManager::getSprite(key);
	sprite.setPosition(sf::Vector2f(float(x * 128), float(y * 128)));
}

vector<Tile> Pawn::getPossibleMoves(const Tile tiles[8][8]) const
{
	vector<Tile> possibleMoves;
	int direction = isWhite ? -1 : 1; // White moves up, black moves down
	int startRow = isWhite ? 6 : 1; // Starting row for pawns
	// Move forward by one square
	int newX = x + direction;
	if (newX >= 0 && newX < 8 && !tiles[newX][y].HasFigure())
	{
		possibleMoves.push_back(tiles[newX][y]);
	}
	// Move forward by two squares from the starting position
	if (x == startRow && !tiles[newX][y].HasFigure() && !tiles[newX + direction][y].HasFigure())
	{
		possibleMoves.push_back(tiles[newX + direction][y]);
	}
	// Capture diagonally
	for (int dx = -1; dx <= 1; dx += 2)
	{
		newX = x + direction;
		int newY = y + dx;
		if (newY >= 0 && newY < 8 && newX >= 0 && newX < 8)
		{
			if (tiles[newX][newY].HasFigure() &&
				((isWhite && !tiles[newX][newY].HasFigure()) || (!isWhite && tiles[newX][newY].HasFigure())))
			{
				possibleMoves.push_back(tiles[newX][newY]);
			}
		}
	}
	return possibleMoves;
}