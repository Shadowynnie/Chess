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
		Sprite.setPosition(sf::Vector2f(float(X * 128), float(Y * 128)));
	}
}

void Figure::HighlightPossibleMoves(vector<Tile*>& possibleMoves)
{
	for (size_t i = 0; i < possibleMoves.size(); i++)
	{
        possibleMoves.at(i)->Highlight(true);
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