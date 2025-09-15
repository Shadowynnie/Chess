#include "Rook.h"

Rook::Rook(int _x, int _y, bool _isWhite)
{
	x = _x;
	y = _y;
	isWhite = _isWhite;

	/*
	if (isWhite)
	{
		sprite = assetManager.rookSpriteWhite; // Use the asset manager to get the white bishop sprite
	}
	else
	{
		sprite = assetManager.rookSpriteBlack; // Use the asset manager to get the black bishop sprite
	}*/
	string key = isWhite ? "rook_white" : "rook_black";
	sprite = AssetManager::getSprite(key);
	sprite.setPosition(sf::Vector2f(float(x * 128), float(y * 128)));
}

vector<Tile> Rook::getPossibleMoves(const Tile tiles[8][8]) const
{
	vector<Tile> possibleMoves;
	// Check horizontal and vertical directions
	for (int dx = -1; dx <= 1; dx += 2) // Horizontal movement
	{
		int newX = x + dx;
		while (newX >= 0 && newX < 8)
		{
			if (!tiles[newX][y].HasFigure())
			{
				possibleMoves.push_back(tiles[newX][y]);
			}
			else
			{
				// If there's a figure, we can capture it if it's of the opposite color
				if ((isWhite && !tiles[newX][y].HasFigure()) || (!isWhite && tiles[newX][y].HasFigure()))
				{
					possibleMoves.push_back(tiles[newX][y]);
				}
				break; // Stop checking this direction after encountering a figure
			}
			newX += dx;
		}
	}

	// Vertical movement
	for (int dy = -1; dy <= 1; dy += 2) 
	{
		int newY = y + dy;
		while (newY >= 0 && newY < 8)
		{
			if (!tiles[x][newY].HasFigure())
			{
				possibleMoves.push_back(tiles[x][newY]);
			}
			else
			{
				// If there's a figure, we can capture it if it's of the opposite color
				if ((isWhite && !tiles[x][newY].HasFigure()) || (!isWhite && tiles[x][newY].HasFigure()))
				{
					possibleMoves.push_back(tiles[x][newY]);
				}
				break; // Stop checking this direction after encountering a figure
			}
			newY += dy;
		}
	}
	return possibleMoves;
}