#pragma once
#include "../Tile.h"

#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>

using std::vector;

class Figurine
{
protected:
	sf::Sprite sprite;
	sf::Texture texture;
	int x; // X position on the board
	int y; // Y position on the board
	bool isWhite; // True if the figurine is white, false if black
public:
	Figurine(int _x, int _y, bool _isWhite); // Constructor
	Figurine() : x(0), y(0), isWhite(true), sprite(texture) {} // Default constructor
	virtual ~Figurine() = default; // Virtual destructor for proper cleanup of derived classes
	virtual void move(int newX, int newY);
	virtual vector<Tile> getPossibleMoves(const Tile tiles[8][8]) const = 0; // Pure virtual function to get possible moves
	virtual sf::Sprite getSprite() const;
};