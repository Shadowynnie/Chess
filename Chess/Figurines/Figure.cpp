#include "Figure.h"

Figure::Figure(int x, int y, bool isWhite) : Sprite(Texture)
{
	X = x;
	Y = y;
	IsWhite = isWhite;
	// Load the texture based on the color of the figure
	Sprite.setTexture(Texture);
}

sf::Sprite Figure::GetSprite() const
{
	return Sprite;
}

void Figure::Move(int newX, int newY)
{
	// Update the position of the figurine
	X = newX;
	Y = newY;
	Sprite.setPosition(sf::Vector2f(static_cast<float>(X * 64), static_cast<float>(Y * 64))); // Update sprite position
}