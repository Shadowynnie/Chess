#include "Bishop.h"


Bishop::Bishop(int _x, int _y, bool _isWhite)
{
	X = _x;
	Y = _y;
	IsWhite = _isWhite;

	string key = IsWhite ? "bishop_white" : "bishop_black";
	Sprite = AssetManager::GetSprite(key);
	Sprite.setPosition(sf::Vector2f(float(X * 128), float(Y * 128)));
}

vector<Tile*> Bishop::GetPossibleMoves(Tile tiles[8][8])
{
	vector<Tile*> possibleMoves;
	// Directions: top-left, top-right, bottom-left, bottom-right
	int directions[4][2] = { {-1, -1}, {1, -1}, {-1, 1}, {1, 1} };
	for (auto& dir : directions)
	{
		int dx = dir[0];
		int dy = dir[1];
		int x = X + dx;
		int y = Y + dy;
		while (x >= 0 && x < 8 && y >= 0 && y < 8)
		{
			if (!tiles[x][y].IsOccupied())
			{
				possibleMoves.push_back(&tiles[x][y]);
			}
			else
			{
				if (tiles[x][y].GetFigure() != nullptr && tiles[x][y].GetFigure()->GetColor() != IsWhite)
				{
					possibleMoves.push_back(&tiles[x][y]); // Can capture opponent's piece
				}
				break; // Stop if occupied
			}
			x += dx;
			y += dy;
		}
	}
	// Highlight the tiles
    HighlightPossibleMoves(possibleMoves);
	return possibleMoves;
}