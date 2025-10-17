#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "../Tile.h"
#include "../AssetManager.h"

using std::string;
using std::vector;

class Figure
{
protected:
	sf::Sprite Sprite;
	sf::Texture Texture;
	int X;
	int Y;
	bool IsWhite;

public:
	Figure(int x, int y, bool isWhite);
	Figure() : X(0), Y(0), IsWhite(true), Sprite(Texture){}
	virtual ~Figure() = default;

	void Move(Tile* tile);
	virtual vector<Tile*> GetPossibleMoves(Tile tiles[8][8]) = 0;
    void HighlightPossibleMoves(vector<Tile*>& possibleMoves);
	virtual sf::Sprite GetSprite() const;

	int GetX() const;
	int GetY() const;
	bool GetColor() const;


	//Testing functions
    void setPosition(int x, int y) { X = x; Y = y; }
};
