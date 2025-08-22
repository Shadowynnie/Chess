#include "Knight.h"

Knight::Knight(int _x, int _y, bool _isWhite)
{
	x = _x;
	y = _y;
	isWhite = _isWhite;
	
	/*
	if (isWhite)
	{
		sprite = assetManager.knightSpriteWhite; // Use the asset manager to get the white bishop sprite
	}
	else
	{
		sprite = assetManager.knightSpriteBlack; // Use the asset manager to get the black bishop sprite
	}*/
	string key = isWhite ? "knight_white" : "knight_black";
	sprite = assetManager.getSprite(key);
	sprite.setPosition(sf::Vector2f(float(x * 64), float(y * 64)));
}

vector<Tile> Knight::getPossibleMoves(const Tile tiles[8][8]) const
{
	vector<Tile> possibleMoves;
	// Knight moves in an "L" shape: two squares in one direction and one square perpendicular
	int knightMoves[8][2] = {
		{2, 1}, {2, -1}, {-2, 1}, {-2, -1},
		{1, 2}, {1, -2}, {-1, 2}, {-1, -2}
	};
	for (const auto& move : knightMoves)
	{
		int newX = x + move[0];
		int newY = y + move[1];
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
	return possibleMoves;
}