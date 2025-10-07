#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <string>

#include "../Tile.h"
#include "../AssetManager.h"

using std::string;
using std::vector;

//static const char* TexturePathWhite = ""; // Path to white texture
//static const char* TexturePathBlack = ""; // Path to black texture
//static AssetManager assetManager; // Asset manager instance to handle textures

class Figure
{
protected:
	sf::Sprite Sprite;
	sf::Texture Texture;
	int X; // X position on the board
	int Y; // Y position on the board
	bool IsWhite; // True if the figurine is white, false if black
public:
	Figure(int x, int y, bool isWhite); // Constructor
	Figure() : X(0), Y(0), IsWhite(true), Sprite(Texture) {} // Default constructor
	virtual ~Figure() = default; // Virtual destructor for proper cleanup of derived classes
	virtual void Move(int newX, int newY);
	virtual vector<Tile> GetPossibleMoves(Tile tiles[8][8]) = 0; // Pure virtual function to get possible moves
	virtual sf::Sprite GetSprite() const;
};