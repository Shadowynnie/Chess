#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <map>
#include <functional>

#include "Tile.h"
#include "Figurines/Figure.h"
#include "Figurines/Bishop.h"
#include "Figurines/King.h"
#include "Figurines/Knight.h"
#include "Figurines/Rook.h"
#include "Figurines/Queen.h"
#include "Figurines/Pawn.h"

using std::vector;
using std::map;
using std::function;
using std::cerr;
using std::endl;
using std::cout;

static bool currentRound = true; // True for player 1's turn, false for player 2's turn

class GameManager
{
	private:
	Tile tiles[8][8]; // 2D array of tiles representing the chessboard
	vector<Figure*> player1figures; // Vector to hold player 1 chess pieces
	vector<Figure*> player2figures; // Vector to hold player 2 chess pieces
public:
	GameManager(); // Constructor to initialize the game manager
	~GameManager(); // Destructor to clean up dynamically allocated memory
	void InitializeBoard(); // Function to initialize the chessboard with tiles and figures
	void Draw(sf::RenderWindow& window); // Function to draw the chessboard and figures on the window
	void HandleInput(sf::Event event); // Function to handle user input events
	void Update(); // Function to update the game state
	void ResetGame(); // Function to reset the game state
};