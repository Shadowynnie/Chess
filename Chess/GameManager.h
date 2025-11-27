#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <map>
#include <functional>
#include <algorithm>

#include "Tile.h"

enum class GameState
{
	MAIN_MENU,
	HOST_GAME,
	CONNECT_TO_GAME,
	SINGLEPLAYER,
	GAME_OVER,
	CLOSED,
	SETTINGS
};

enum class PieceType
{
	ROOK,	// 0
	KNIGHT,	// 1
	BISHOP,	// 2
	QUEEN,	// 3
	KING,	// 4
	PAWN	// 5
};

enum class GameResult
{
	BLACK_WINS,
	WHITE_WINS,
	DRAW
};

static bool _currentRound = true; // True for player 1's turn, false for player 2's turn
static bool _localIsWhite = true; // True = this instance plays White, False = Black

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
    static void DrawStatusBar(); // Function to draw the status bar (current turn, etc.)
    static void ClearHighlitghts(); // Function to clear highlighted tiles
	static void CheckForCheck(); // Function to check if a king is in check
	static void PromotePawn(Figure* pawn, bool isMultiplayer); // Function to handle pawn promotion
	static void ResetEnPassantFlags(); // Function to reset en passant flags for all pawns
	static void Update(bool isMultiplayer); // Function to update the game state
	static void MainMenu(); // Function to display the main menu
	static void HostGame(); // Function to host a game
	static void ConnectToGame(); // Function to connect to a game
	static void PlayGame(bool isMultiplayer); // Singleplayer or multiplayer game loop
    static void StopNetworkThread(); // Function to stop the network thread
    static void ChangeRound(); // Function to change the current round/player turn
    static void ApplyIncomingMove(int fromX, int fromY, int toX, int toY); // Function to apply incoming move from the network
    static void ApplyIncomingPromotion(int x, int y, PieceType newType); // Function to apply incoming pawn promotion from the network
	//static void EnqueueIncomingMove(int fx, int fy, int tx, int ty);

	// Endgame / check utilities
	static bool IsKingInCheck(bool isWhite);
	static bool HasAnyLegalMoves(bool isWhite);
	static bool OnlyKingsLeft();
	static bool EvaluateEndGame(); // returns true when game ended (prints to console and triggers cleanup)
	static void EndGameMenu();
    static void SettingsMenu(); // Function to display settings menu
    static void PausedMenu(); // Function to display paused menu
	static void DisconnectedMenu();
	static void WaitToConnectMenu();
    static bool GetCurrentRound() { return _currentRound; } // Getter for current round
};