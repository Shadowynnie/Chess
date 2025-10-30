#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <map>
#include <functional>

#include "Tile.h"

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

    // Drawing functions
	static void DrawGame(); // Function to draw the chessboard and figures on the window
	static void DrawTiles(sf::RenderWindow& window, Tile tiles[8][8]);
	static void DrawFigures(sf::RenderWindow& window, const std::vector<Figure*>& white, const std::vector<Figure*>& black);
	static void DrawHighlights(sf::RenderWindow& window, Tile tiles[8][8]);
	static void ClearHighlitghts();
    static void CheckForCheck(); // Function to check if a king is in check
    static void PromotePawn(Figure* pawn); // Function to handle pawn promotion
    static void ResetEnPassantFlags(); // Function to reset en passant flags for all pawns
	static void Update(); // Function to update the game state
	static void MainMenu(); // Function to display the main menu
    static void HostGame(); // Function to host a game
    static void ConnectToGame(); // Function to connect to a game
    static void PlayGame(); // Singleplayer or multiplayer game loop
    //static void ShowPossibleMoves(Figure* figure); // Function to highlight possible moves for a selected figure
    //static void HandleMenuEvents(); // Function to handle menu events
    //static void HandleGameEvents(); // Function to handle game events
};