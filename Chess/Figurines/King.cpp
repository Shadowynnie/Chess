#include "King.h"

King::King(int _x, int _y, bool _isWhite)
{
	X = _x;
	Y = _y;
	IsWhite = _isWhite;

	/*
	if (isWhite)
	{
		sprite = assetManager.kingSpriteWhite; // Use the asset manager to get the white bishop sprite
	}
	else
	{
		sprite = assetManager.kingSpriteBlack; // Use the asset manager to get the black bishop sprite
	}*/
	string key = IsWhite ? "king_white" : "king_black";
	Sprite = AssetManager::GetSprite(key);
	Sprite.setPosition(sf::Vector2f(float(X * 128), float(Y * 128)));
}

vector<Tile> King::GetPossibleMoves(Tile tiles[8][8])
{
	vector<Tile> possibleMoves;
	// Check all adjacent squares (8 directions)
	for (int dx = -1; dx <= 1; dx++)
	{
		for (int dy = -1; dy <= 1; dy++)
		{
			if (dx == 0 && dy == 0) continue; // Skip the current position
			int newX = X + dx;
			int newY = Y + dy;
			if (newX >= 0 && newX < 8 && newY >= 0 && newY < 8)
			{
				if (!tiles[newX][newY].HasFigure() ||
					(tiles[newX][newY].HasFigure() &&
						((IsWhite && !tiles[newX][newY].HasFigure()) ||
							(!IsWhite && tiles[newX][newY].HasFigure()))))
				{
					possibleMoves.push_back(tiles[newX][newY]);
				}
			}
		}
	}
	return possibleMoves;
}