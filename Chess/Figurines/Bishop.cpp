#include "Bishop.h"

Bishop::Bishop(int _x, int _y, bool _isWhite)
{
	x = _x;
	y = _y;
	isWhite = _isWhite;

	if (!texture.loadFromFile(isWhite ? "Assets/bishop_white.png" : "Assets/bishop_black.png"))
	{
		std::cerr << "Failed to load texture." << std::endl;
		return;
	}
	sprite = sf::Sprite(texture); // Now assign the loaded texture
	sprite.setScale(sf::Vector2f(0.5f, 0.5f));
	sprite.setPosition(sf::Vector2f(0.f, 0.f));
}

vector<Tile> Bishop::getPossibleMoves(const Tile tiles[8][8]) const
{
	vector<Tile> possibleMoves;
	// Check diagonals in all four directions
	for (int dx = -1; dx <= 1; dx += 2)
	{
		for (int dy = -1; dy <= 1; dy += 2)
		{
			int newX = x + dx;
			int newY = y + dy;
			while (newX >= 0 && newX < 8 && newY >= 0 && newY < 8)
			{
				if (!tiles[newX][newY].HasFigure())
				{
					possibleMoves.push_back(tiles[newX][newY]);
				}
				else
				{
					// If there's a figure, we can capture it if it's of the opposite color
					if ((isWhite && !tiles[newX][newY].HasFigure()) || (!isWhite && tiles[newX][newY].HasFigure())) 
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
