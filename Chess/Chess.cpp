// Chess.cpp : Defines the entry point for the application.
//
#include <SFML/Graphics.hpp>

#include "GameManager.h"
#include "AssetManager.h"


int main()
{
	AssetManager::LoadTextures();
    GameManager::MainMenu();

    GameManager::StopNetworkThread();
	//GameManager gameManager;
	//AssetManager::loadTextures();
	//gameManager.InitializeBoard();
	//gameManager.Update();
	return 0;
}
