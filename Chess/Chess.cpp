// Chess.cpp : Defines the entry point for the application.
//
#include <SFML/Graphics.hpp>

#include "GameManager.h"
//#include "AssetManager.h"
#include "Chess.h"
#include "Tile.h"
#include "Figurines/Figure.h"
#include "Figurines/Bishop.h"
#include "Figurines/King.h"
#include "Figurines/Knight.h"
#include "Figurines/Rook.h"
#include "Figurines/Queen.h"
#include "Figurines/Pawn.h"

using std::cout;
using std::endl;
using std::function;

int main()
{
	AssetManager::LoadTextures();
    GameManager::MainMenu();
	
 //   GameState currentState = GameState::MAIN_MENU;

 //   map<GameState, function<void()>> stateFunctions = 
	//{
	//	{ GameState::MAIN_MENU, [&]() { gameManager.MainMenu(); } },
	//	{ GameState::HOST_GAME, []() { cout << "Host Game State" << endl; } },
	//	{ GameState::CONNECT_TO_GAME, []() { cout << "Connect to Game State" << endl; } },
	//	{ GameState::PLAYING, []() { cout << "Playing State" << endl; } },
	//	{ GameState::GAME_OVER, []() { cout << "Game Over State" << endl; } },
	//	{ GameState::CLOSED, []() { cout << "Closed State" << endl; } }
 //   };

	//
 //   stateFunctions[currentState]();



	//GameManager gameManager;
	//AssetManager::loadTextures();
	//gameManager.InitializeBoard();
	//gameManager.Update();
	return 0;
}
