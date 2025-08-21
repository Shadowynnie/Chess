#include "King.h"

King::King(int _x, int _y, bool _isWhite)
{
	x = _x;
	y = _y;
	isWhite = _isWhite;
	texturePathBlack = "Assets/king_black.png"; // Path to black king texture
	texturePathWhite = "Assets/king_white.png"; // Path to white king texture
	if (!texture.loadFromFile(isWhite ? texturePathWhite : texturePathBlack))
	{
		std::cerr << "Failed to load texture." << std::endl;
		return;
	}
	sprite = sf::Sprite(texture); // Now assign the loaded texture
	sprite.setScale(sf::Vector2f(0.5f, 0.5f));
	sprite.setPosition(sf::Vector2f(1.f, 1.f));
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