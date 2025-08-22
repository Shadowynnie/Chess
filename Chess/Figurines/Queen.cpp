#include "Queen.h"

Queen::Queen(int _x, int _y, bool _isWhite)
{
	x = _x;
	y = _y;
	isWhite = _isWhite;
	
	/*
	if (isWhite)
	{
		sprite = assetManager.queenSpriteWhite; // Use the asset manager to get the white bishop sprite
	}
	else
	{
		sprite = assetManager.queenSpriteBlack; // Use the asset manager to get the black bishop sprite
	}*/
	string key = isWhite ? "queen_white" : "queen_black";
	sprite = assetManager.getSprite(key);
	sprite.setPosition(sf::Vector2f(float(x * 64), float(y * 64)));
}

vector<Tile> Queen::getPossibleMoves(const Tile tiles[8][8]) const
{
	vector<Tile> possibleMoves;
	// Check all directions: horizontal, vertical, and diagonal
	for (int dx = -1; dx <= 1; dx++)
	{
		for (int dy = -1; dy <= 1; dy++)
		{
			if (dx == 0 && dy == 0) continue; // Skip the case where both dx and dy are zero
			int newX = x + dx;
			int newY = y + dy;
			while (newX >= 0 && newX < 8 && newY >= 0 && newY < 8)
			{
				if (!tiles[newX][newY].HasFigure())
				{
					possibleMoves.push_back(tiles[newX][newY]);
				}
				else
				{
					// If there's a figure, we can capture it if it's of the opposite color
					if ((isWhite && !tiles[newX][newY].HasFigure()) || (!isWhite && tiles[newX][newY].HasFigure())) 
					{
						possibleMoves.push_back(tiles[newX][newY]);
					}
					break; // Stop checking this direction after encountering a figure
				}
				newX += dx;
				newY += dy;
			}
		}
	}
	return possibleMoves;
}