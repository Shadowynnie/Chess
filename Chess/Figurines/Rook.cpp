#include "Rook.h"

Rook::Rook(int _x, int _y, bool _isWhite)
{
	x = _x;
	y = _y;
	isWhite = _isWhite;
	texturePathBlack = "Assets/rook_black.png"; // Path to black rook texture
	texturePathWhite = "Assets/rook_white.png"; // Path to white rook texture
	if (!texture.loadFromFile(isWhite ? texturePathWhite : texturePathBlack))
	{
		std::cerr << "Failed to load texture." << std::endl;
		return;
	}
	sprite = sf::Sprite(texture); // Now assign the loaded texture
	sprite.setScale(sf::Vector2f(0.5f, 0.5f));
	sprite.setPosition(sf::Vector2f(float(x * 64), float(y * 64)));
}

vector<Tile> Rook::getPossibleMoves(const Tile tiles[8][8]) const
{
	vector<Tile> possibleMoves;
	// Check horizontal and vertical directions
	for (int dx = -1; dx <= 1; dx += 2) // Horizontal movement
	{
		int newX = x + dx;
		while (newX >= 0 && newX < 8)
		{
			if (!tiles[newX][y].HasFigure())
			{
				possibleMoves.push_back(tiles[newX][y]);
			}
			else
			{
				// If there's a figure, we can capture it if it's of the opposite color
				if ((isWhite && !tiles[newX][y].HasFigure()) || (!isWhite && tiles[newX][y].HasFigure()))
				{
					possibleMoves.push_back(tiles[newX][y]);
				}
				break; // Stop checking this direction after encountering a figure
			}
			newX += dx;
		}
	}
	for (int dy = -1; dy <= 1; dy += 2) // Vertical movement
	{
		int newY = y + dy;
		while (newY >= 0 && newY < 8)
		{
			if (!tiles[x][newY].HasFigure())
			{
				possibleMoves.push_back(tiles[x][newY]);
			}
			else
			{
				// If there's a figure, we can capture it if it's of the opposite color
				if ((isWhite && !tiles[x][newY].HasFigure()) || (!isWhite && tiles[x][newY].HasFigure()))
				{
					possibleMoves.push_back(tiles[x][newY]);
				}
				break; // Stop checking this direction after encountering a figure
			}
			newY += dy;
		}
	}
	return possibleMoves;
}