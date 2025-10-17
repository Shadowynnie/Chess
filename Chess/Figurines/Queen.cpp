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

vector<Tile*> Queen::GetPossibleMoves(Tile tiles[8][8])
{
	vector<Tile*> possibleMoves;
	// Directions: N, NE, E, SE, S, SW, W, NW
	int directions[8][2] = {
		{0, -1}, {1, -1}, {1, 0}, {1, 1},
		{0, 1}, {-1, 1}, {-1, 0}, {-1, -1}
	};
	for (auto& dir : directions)
	{
		int dx = dir[0];
		int dy = dir[1];
		int nx = X + dx;
		int ny = Y + dy;
		while (nx >= 0 && nx < 8 && ny >= 0 && ny < 8)
		{
			if (!tiles[nx][ny].IsOccupied())
			{
				possibleMoves.push_back(&tiles[nx][ny]);
			}
			else
			{
				if (tiles[nx][ny].GetFigure()->GetColor() != IsWhite)
				{
					possibleMoves.push_back(&tiles[nx][ny]); // Can capture opponent's piece
				}
				break; // Can't move past occupied tile
			}
			nx += dx;
			ny += dy;
		}
	}
	// Highlight the tiles
	HighlightPossibleMoves(possibleMoves);
	return possibleMoves;
}