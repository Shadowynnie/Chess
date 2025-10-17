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

vector<Tile*> King::GetPossibleMoves(Tile tiles[8][8])
{
	vector<Tile*> possibleMoves;
	// King moves one square in any direction
	int kingMoves[8][2] = {
		{1, 0}, {-1, 0}, {0, 1}, {0, -1},
		{1, 1}, {1, -1}, {-1, 1}, {-1, -1}
	};
	for (const auto& move : kingMoves)
	{
		int newX = X + move[0];
		int newY = Y + move[1];
		if (newX >= 0 && newX < 8 && newY >= 0 && newY < 8)
		{
			if (!tiles[newX][newY].IsOccupied() ||
				(tiles[newX][newY].IsOccupied() &&
				 (tiles[newX][newY].GetFigure() != nullptr && 
				  tiles[newX][newY].GetFigure()->GetColor() != IsWhite)))
			{
				possibleMoves.push_back(&tiles[newX][newY]);
			}
		}
	}
	// Highlight the tiles
    HighlightPossibleMoves(possibleMoves);
	return possibleMoves;
}