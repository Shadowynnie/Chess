#include "Figure.h"

Figure::Figure(int _x, int _y, bool _isWhite) : sprite(texture)
{
	x = _x;
	y = _y;
	isWhite = _isWhite;
	// Load the texture based on the color of the figurine
	//if (isWhite) {
	//	texture.loadFromFile("path/to/white/figurine.png"); // Replace with actual path
	//} else {
	//	texture.loadFromFile("path/to/black/figurine.png"); // Replace with actual path
	//}
	sprite.setTexture(texture);
}

sf::Sprite Figure::getSprite() const
{
	return sprite;
}

void Figure::move(int newX, int newY)
{
	// Update the position of the figurine
	x = newX;
	y = newY;
	sprite.setPosition(sf::Vector2f(static_cast<float>(x * 64), static_cast<float>(y * 64))); // Update sprite position
}