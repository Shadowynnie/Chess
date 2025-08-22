#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <string>

#include "../Tile.h"
#include "../AssetManager.h"

using std::string;
using std::vector;

static const char* texturePathWhite = ""; // Path to white bishop texture
static const char* texturePathBlack = ""; // Path to black bishop texture
static AssetManager assetManager; // Asset manager instance to handle textures

class Figure
{

protected:

	sf::Sprite sprite;
	sf::Texture texture;
	int x; // X position on the board
	int y; // Y position on the board
	bool isWhite; // True if the figurine is white, false if black
public:
	Figure(int _x, int _y, bool _isWhite); // Constructor
	Figure() : x(0), y(0), isWhite(true), sprite(texture) {} // Default constructor
	virtual ~Figure() = default; // Virtual destructor for proper cleanup of derived classes
	virtual void move(int newX, int newY);
	virtual vector<Tile> getPossibleMoves(const Tile tiles[8][8]) const = 0; // Pure virtual function to get possible moves
	virtual sf::Sprite getSprite() const;
};