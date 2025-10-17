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

vector<Tile*> Rook::GetPossibleMoves(Tile tiles[8][8])
{
	vector<Tile*> possibleMoves;
	// Directions: up, down, left, right
	const int directions[4][2] = {
		{0, 1},   // Up
		{0, -1},  // Down
		{-1, 0},  // Left
		{1, 0}    // Right
	};
	for (const auto& dir : directions)
	{
		int dx = dir[0];
		int dy = dir[1];
		int nx = X + dx;
		int ny = Y + dy;
		while (nx >= 0 && nx < 8 && ny >= 0 && ny < 8) // Stay within board bounds
		{
			if (!tiles[nx][ny].IsOccupied())
			{
				possibleMoves.push_back(&tiles[nx][ny]); // Empty tile
			}
			else
			{
				if (tiles[nx][ny].GetFigure()->GetColor() != IsWhite)
				{
					possibleMoves.push_back(&tiles[nx][ny]); // Capture opponent's piece
				}
				break; // Stop in this direction after hitting any piece
			}
			nx += dx;
			ny += dy;
		}
	}
	// Highlight the tiles
	HighlightPossibleMoves(possibleMoves);
	return possibleMoves;
}