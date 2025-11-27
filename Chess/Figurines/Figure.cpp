#include "Figure.h"

Figure::Figure(int x, int y, bool isWhite) : Sprite(Texture)
{
	X = x;
	Y = y;
	IsWhite = isWhite;
	// Load the texture based on the color of the figure
	//Sprite.setTexture(Texture);
}

//sf::Sprite Figure::GetSprite() const
//{
//	return Sprite;
//}

const sf::Sprite& Figure::GetSprite() const 
{
	return Sprite;
}


void Figure::Move(Tile* tile, Tile* previousTile, Tile tiles[8][8]) // Move the figure to the specified tile position
{
	if (tile != nullptr)
	{
		if (previousTile != nullptr)
			previousTile->SetInCheck(false);
        X = tile->GetX();
        Y = tile->GetY();
		Sprite.setPosition(sf::Vector2f(float(X * 128), float(Y * 128)));
        tile->SetFigure(this);
        previousTile->SetFigure(nullptr);
	}
}

void Figure::Move(Tile* tile, Tile* previousTile, vector<Figure*>& enemyFigures)
{
	if (tile != nullptr)
	{
		if (tile->IsOccupied())
		{
			Figure* capturedFigure = tile->GetFigure();
			auto it = std::find(enemyFigures.begin(), enemyFigures.end(), capturedFigure);
			if (it != enemyFigures.end())
			{
				enemyFigures.erase(it);
				delete capturedFigure;
			}
		}
		X = tile->GetX();
		Y = tile->GetY();
		Sprite.setPosition(sf::Vector2f(float(X * 128), float(Y * 128)));
		tile->SetFigure(this);
        previousTile->SetInCheck(false);
		previousTile->SetFigure(nullptr);
	}
}

vector<Tile*> Figure::GetPossibleMoves(Tile tiles[8][8], vector<Figure*>& enemyFigures)
{
    // Default behaior for figures that do not consider enemy figures
	return GetPossibleMoves(tiles);
}

void Figure::HighlightPossibleMoves(vector<Tile*>& possibleMoves)
{
	for (size_t i = 0; i < possibleMoves.size(); i++)
        possibleMoves.at(i)->Highlight(true);
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

Tile* Figure::GetCurrentTile(Tile tiles[8][8]) const
{
	return &tiles[X][Y];
}