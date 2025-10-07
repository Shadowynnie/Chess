#include "Queen.h"

Queen::Queen(int _x, int _y, bool _isWhite)
{
	X = _x;
	Y = _y;
	IsWhite = _isWhite;
	
	/*
	if (isWhite)
	{
		sprite = assetManager.queenSpriteWhite; // Use the asset manager to get the white bishop sprite
	}
	else
	{
		sprite = assetManager.queenSpriteBlack; // Use the asset manager to get the black bishop sprite
	}*/
	string key = IsWhite ? "queen_white" : "queen_black";
	Sprite = AssetManager::GetSprite(key);
	Sprite.setPosition(sf::Vector2f(float(X * 128), float(Y * 128)));
}

vector<Tile> Queen::GetPossibleMoves(Tile tiles[8][8])
{
	vector<Tile> possibleMoves;
	// Check all directions: horizontal, vertical, and diagonal
	for (int dx = -1; dx <= 1; dx++)
	{
		for (int dy = -1; dy <= 1; dy++)
		{
			if (dx == 0 && dy == 0) continue; // Skip the case where both dx and dy are zero
			int newX = X + dx;
			int newY = Y + dy;
			while (newX >= 0 && newX < 8 && newY >= 0 && newY < 8)
			{
				if (!tiles[newX][newY].HasFigure())
				{
					possibleMoves.push_back(tiles[newX][newY]);
				}
				else
				{
					// If there's a figure, we can capture it if it's of the opposite color
					if ((IsWhite && !tiles[newX][newY].HasFigure()) || (!IsWhite && tiles[newX][newY].HasFigure())) 
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