#include "Rook.h"

Rook::Rook(int _x, int _y, bool _isWhite)
{
	X = _x;
	Y = _y;
	IsWhite = _isWhite;

	/*
	if (isWhite)
	{
		sprite = assetManager.rookSpriteWhite; // Use the asset manager to get the white bishop sprite
	}
	else
	{
		sprite = assetManager.rookSpriteBlack; // Use the asset manager to get the black bishop sprite
	}*/
	string key = IsWhite ? "rook_white" : "rook_black";
	Sprite = AssetManager::GetSprite(key);
	Sprite.setPosition(sf::Vector2f(float(X * 128), float(Y * 128)));
}

vector<Tile> Rook::GetPossibleMoves(Tile tiles[8][8])
{
	vector<Tile> possibleMoves;
	// Check horizontal and vertical directions
	for (int dx = -1; dx <= 1; dx += 2) // Horizontal movement
	{
		int newX = X + dx;
		while (newX >= 0 && newX < 8)
		{
			if (!tiles[newX][Y].HasFigure())
			{
				possibleMoves.push_back(tiles[newX][Y]);
			}
			else
			{
				// If there's a figure, we can capture it if it's of the opposite color
				if ((IsWhite && !tiles[newX][Y].HasFigure()) || (!IsWhite && tiles[newX][Y].HasFigure()))
				{
					possibleMoves.push_back(tiles[newX][Y]);
				}
				break; // Stop checking this direction after encountering a figure
			}
			newX += dx;
		}
	}

	// Vertical movement
	for (int dy = -1; dy <= 1; dy += 2) 
	{
		int newY = Y + dy;
		while (newY >= 0 && newY < 8)
		{
			if (!tiles[X][newY].HasFigure())
			{
				possibleMoves.push_back(tiles[X][newY]);
			}
			else
			{
				// If there's a figure, we can capture it if it's of the opposite color
				if ((IsWhite && !tiles[X][newY].HasFigure()) || (!IsWhite && tiles[X][newY].HasFigure()))
				{
					possibleMoves.push_back(tiles[X][newY]);
				}
				break; // Stop checking this direction after encountering a figure
			}
			newY += dy;
		}
	}
	return possibleMoves;
}