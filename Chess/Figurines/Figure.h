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

	virtual void Move(Tile* tile, Tile* previousTile, Tile tiles[8][8]={});
	virtual void Move(Tile* tile, Tile* previousTile, vector<Figure*>& enemyFigures);
	virtual vector<Tile*> GetPossibleMoves(Tile tiles[8][8])=0;
	virtual vector<Tile*> GetPossibleMoves(Tile tiles[8][8], vector<Figure*>& enemyFigures);
    void HighlightPossibleMoves(vector<Tile*>& possibleMoves);
	virtual sf::Sprite GetSprite() const;

	int GetX() const;
	int GetY() const;
	bool GetColor() const;
    Tile* GetCurrentTile(Tile tiles[8][8]) const;

	//Testing functions
    void setPosition(int x, int y) { X = x; Y = y; }
};
