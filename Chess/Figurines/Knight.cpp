#include "Knight.h"

Knight::Knight(int _x, int _y, bool _isWhite)
{
	X = _x;
	Y = _y;
	IsWhite = _isWhite;
	
	/*
	if (isWhite)
	{
		sprite = assetManager.knightSpriteWhite; // Use the asset manager to get the white bishop sprite
	}
	else
	{
		sprite = assetManager.knightSpriteBlack; // Use the asset manager to get the black bishop sprite
	}*/
	string key = IsWhite ? "knight_white" : "knight_black";
	Sprite = AssetManager::GetSprite(key);
	Sprite.setPosition(sf::Vector2f(float(X * 128), float(Y * 128)));
}

vector<Tile> Knight::GetPossibleMoves(Tile tiles[8][8])
{
	vector<Tile> possibleMoves;
	// Knight moves in an "L" shape: two squares in one direction and one square perpendicular
	int knightMoves[8][2] = {
		{2, 1}, {2, -1}, {-2, 1}, {-2, -1},
		{1, 2}, {1, -2}, {-1, 2}, {-1, -2}
	};
	for (const auto& move : knightMoves)
	{
		int newX = X + move[0];
		int newY = Y + move[1];
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
	return possibleMoves;
}