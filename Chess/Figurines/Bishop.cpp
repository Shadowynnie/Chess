#include "Bishop.h"


Bishop::Bishop(int _x, int _y, bool _isWhite)
{
	X = _x;
	Y = _y;
	IsWhite = _isWhite;

	/*
	if (isWhite)
	{
		sprite = assetManager.bishopSpriteWhite; // Use the asset manager to get the white bishop sprite
	}
	else
	{
		sprite = assetManager.bishopSpriteBlack; // Use the asset manager to get the black bishop sprite
	}*/

	string key = IsWhite ? "bishop_white" : "bishop_black";
	Sprite = AssetManager::GetSprite(key);
	Sprite.setPosition(sf::Vector2f(float(X * 128), float(Y * 128)));

}

vector<Tile> Bishop::GetPossibleMoves(Tile tiles[8][8])
{
	vector<Tile> possibleMoves;
	// Check diagonals in all four directions
	for (int dx = -1; dx <= 1; dx += 2)
	{
		for (int dy = -1; dy <= 1; dy += 2)
		{
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
