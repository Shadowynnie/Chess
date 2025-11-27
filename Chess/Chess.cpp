// Chess.cpp : Defines the entry point for the application.
//
#include <SFML/Graphics.hpp>
#include "GameManager.h"
#include "AssetManager.h"
#include "Configuration/ConfigManager.h"

int main()
{
	AssetManager::LoadTextures();
    ConfigManager::Load();
    GameManager::MainMenu();
    GameManager::StopNetworkThread();
	return 0;
}
