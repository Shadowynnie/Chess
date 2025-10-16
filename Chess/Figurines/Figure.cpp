#include "Figure.h"

Figure::Figure(int x, int y, bool isWhite) : Sprite(Texture)
{
	X = x;
	Y = y;
	IsWhite = isWhite;
	// Load the texture based on the color of the figure
	//Sprite.setTexture(Texture);
}

sf::Sprite Figure::GetSprite() const
{
	return Sprite;
}

void Figure::Move(Tile* tile) // Move the figure to the specified tile position
{
	if (tile != nullptr)
	{
        X = tile->GetX();
        Y = tile->GetY();
	}
}

int Figure::GetX() const
{
	return X;
}

int Figure::GetY() const
{
	return Y;
}

bool Figure::GetColor() const
{
	return IsWhite;
}