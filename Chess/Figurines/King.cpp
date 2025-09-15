#include "King.h"

King::King(int _x, int _y, bool _isWhite)
{
	x = _x;
	y = _y;
	isWhite = _isWhite;

	/*
	if (isWhite)
	{
		sprite = assetManager.kingSpriteWhite; // Use the asset manager to get the white bishop sprite
	}
	else
	{
		sprite = assetManager.kingSpriteBlack; // Use the asset manager to get the black bishop sprite
	}*/
	string key = isWhite ? "king_white" : "king_black";
	sprite = AssetManager::getSprite(key);
	sprite.setPosition(sf::Vector2f(float(x * 128), float(y * 128)));
}

vector<Tile> King::getPossibleMoves(const Tile tiles[8][8]) const
{
	vector<Tile> possibleMoves;
	// Check all adjacent squares (8 directions)
	for (int dx = -1; dx <= 1; dx++)
	{
		for (int dy = -1; dy <= 1; dy++)
		{
			if (dx == 0 && dy == 0) continue; // Skip the current position
			int newX = x + dx;
			int newY = y + dy;
			if (newX >= 0 && newX < 8 && newY >= 0 && newY < 8)
			{
				if (!tiles[newX][newY].HasFigure() ||
					(tiles[newX][newY].HasFigure() &&
						((isWhite && !tiles[newX][newY].HasFigure()) ||
							(!isWhite && tiles[newX][newY].HasFigure()))))
				{
					possibleMoves.push_back(tiles[newX][newY]);
				}
			}
		}
	}
	return possibleMoves;
}