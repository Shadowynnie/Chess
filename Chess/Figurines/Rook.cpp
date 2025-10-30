#include "Rook.h"

Rook::Rook(int _x, int _y, bool _isWhite)
{
	X = _x;
	Y = _y;
	IsWhite = _isWhite;

	string key = IsWhite ? "rook_white" : "rook_black";
	Sprite = AssetManager::GetSprite(key);
	Sprite.setPosition(sf::Vector2f(float(X * 128), float(Y * 128)));
}

vector<Tile*> Rook::GetPossibleMoves(Tile tiles[8][8])
{
	vector<Tile*> possibleMoves;
	// Directions: up, down, left, right
	const int directions[4][2] = 
	{
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
				possibleMoves.push_back(&tiles[nx][ny]); // Empty tile
			else
			{
				if (tiles[nx][ny].GetFigure()->GetColor() != IsWhite)
					possibleMoves.push_back(&tiles[nx][ny]); // Capture opponent's piece
				break; // Stop in this direction after hitting any piece
			}
			nx += dx;
			ny += dy;
		}
	}
	return possibleMoves;
}

void Rook::Move(Tile* tile, Tile* previousTile, Tile tiles[8][8])
{
	_hasMoved = true;
	Figure::Move(tile, previousTile);
}

bool Rook::HasMoved() const
{
	return _hasMoved;
}