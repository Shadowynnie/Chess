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

enum class GameState
{
	MAIN_MENU,
	HOST_GAME,
	CONNECT_TO_GAME,
	SINGLEPLAYER,
	GAME_OVER,
	CLOSED
};

enum class PieceType
{
	ROOK,
	KNIGHT,
	BISHOP,
	QUEEN,
	KING,
	PAWN
};

class GameManager
{
public:
	//GameManager(); // Constructor to initialize the game manager
	//~GameManager(); // Destructor to clean up dynamically allocated memory
	static void InitializeBoard(); // Function to initialize the chessboard with tiles and figures
    static void DeinitializeBoard(); // Function to clean up the board and figures
	static void DrawGame(); // Function to draw the chessboard and figures on the window
	static void Update(); // Function to update the game state
	static void MainMenu(); // Function to display the main menu
    static void HostGame(); // Function to host a game
    static void ConnectToGame(); // Function to connect to a game
    static void PlayGame(); // Singleplayer or multiplayer game loop
    //static void ShowPossibleMoves(Figure* figure); // Function to highlight possible moves for a selected figure
    static void HandleMenuEvents(); // Function to handle menu events
    static void HandleGameEvents(); // Function to handle game events
};