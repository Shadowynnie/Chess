#include "AssetManager.h"
#include <iostream>
#include <filesystem>

AssetManager::AssetManager()
{
	const char* fileNames[] = {
		"Assets/pawn_white.png",
		"Assets/rook_white.png",
		"Assets/knight_white.png",
		"Assets/bishop_white.png",
		"Assets/queen_white.png",
		"Assets/king_white.png",
		"Assets/pawn_black.png",
		"Assets/rook_black.png",
		"Assets/knight_black.png",
		"Assets/bishop_black.png",
		"Assets/queen_black.png",
		"Assets/king_black.png"
	};
	// Load the textures
	std::cout << "Loading texture: " << fileNames[0] << std::endl;
	if (!pawnTexWhite.loadFromFile(fileNames[0])) 
	{
		std::cerr << "Failed to load " << fileNames[0] << std::endl;
	}

	if (!rookTexWhite.loadFromFile(fileNames[1])) 
	{
		std::cerr << "Failed to load " << fileNames[1] << std::endl;
	}

	if (!knightTexWhite.loadFromFile(fileNames[2])) 
	{
		std::cerr << "Failed to load " << fileNames[2] << std::endl;
	}

	if (!bishopTexWhite.loadFromFile(fileNames[3])) 
	{
		std::cerr << "Failed to load " << fileNames[3] << std::endl;
	}

	if (!queenTexWhite.loadFromFile(fileNames[4])) 
	{
		std::cerr << "Failed to load " << fileNames[4] << std::endl;
	}

	if (!kingTexWhite.loadFromFile(fileNames[5])) 
	{
		std::cerr << "Failed to load " << fileNames[5] << std::endl;
	}

	if (!pawnTexBlack.loadFromFile(fileNames[6])) 
	{
		std::cerr << "Failed to load " << fileNames[6] << std::endl;
	}

	if (!rookTexBlack.loadFromFile(fileNames[7])) 
	{
		std::cerr << "Failed to load " << fileNames[7] << std::endl;
	}

	if (!knightTexBlack.loadFromFile(fileNames[8])) 
	{
		std::cerr << "Failed to load " << fileNames[8] << std::endl;
	}

	if (!bishopTexBlack.loadFromFile(fileNames[9])) 
	{
		std::cerr << "Failed to load " << fileNames[9] << std::endl;
	}

	if (!queenTexBlack.loadFromFile(fileNames[10])) 
	{
		std::cerr << "Failed to load " << fileNames[10] << std::endl;
	}

	if (!kingTexBlack.loadFromFile(fileNames[11])) 
	{
		std::cerr << "Failed to load " << fileNames[11] << std::endl;
	}

	// Create the sprites AFTER textures are loaded
	pawnSpriteWhite = sf::Sprite(pawnTexWhite);
	rookSpriteWhite = sf::Sprite(rookTexWhite);
	knightSpriteWhite = sf::Sprite(knightTexWhite);
	bishopSpriteWhite = sf::Sprite(bishopTexWhite);
	queenSpriteWhite = sf::Sprite(queenTexWhite);
	kingSpriteWhite = sf::Sprite(kingTexWhite);

	pawnSpriteBlack = sf::Sprite(pawnTexBlack);
	rookSpriteBlack = sf::Sprite(rookTexBlack);
	knightSpriteBlack = sf::Sprite(knightTexBlack);
	bishopSpriteBlack = sf::Sprite(bishopTexBlack);
	queenSpriteBlack = sf::Sprite(queenTexBlack);
	kingSpriteBlack = sf::Sprite(kingTexBlack);

	// Set the initial positions of the sprites

	pawnSpriteWhite.setPosition(sf::Vector2f(100, 100));
	rookSpriteWhite.setPosition(sf::Vector2f(200, 100));
	knightSpriteWhite.setPosition(sf::Vector2f(300, 100));
	bishopSpriteWhite.setPosition(sf::Vector2f(400, 100));
	queenSpriteWhite.setPosition(sf::Vector2f(500, 100));
	kingSpriteWhite.setPosition(sf::Vector2f(600, 100));

	pawnSpriteBlack.setPosition(sf::Vector2f(100, 200));
	rookSpriteBlack.setPosition(sf::Vector2f(200, 200));
	knightSpriteBlack.setPosition(sf::Vector2f(300, 200));
	bishopSpriteBlack.setPosition(sf::Vector2f(400, 200));
	queenSpriteBlack.setPosition(sf::Vector2f(500, 200));
	kingSpriteBlack.setPosition(sf::Vector2f(600, 200));

	// Set the scale of the sprites (originally they were 128x128, now scaled down to 64x64)

	pawnSpriteWhite.setScale(sf::Vector2f(0.5f, 0.5f));
	rookSpriteWhite.setScale(sf::Vector2f(0.5f, 0.5f));
	knightSpriteWhite.setScale(sf::Vector2f(0.5f, 0.5f));
	bishopSpriteWhite.setScale(sf::Vector2f(0.5f, 0.5f));
	queenSpriteWhite.setScale(sf::Vector2f(0.5f, 0.5f));
	kingSpriteWhite.setScale(sf::Vector2f(0.5f, 0.5f));

	pawnSpriteBlack.setScale(sf::Vector2f(0.5f, 0.5f));
	rookSpriteBlack.setScale(sf::Vector2f(0.5f, 0.5f));
	knightSpriteBlack.setScale(sf::Vector2f(0.5f, 0.5f));
	bishopSpriteBlack.setScale(sf::Vector2f(0.5f, 0.5f));
	queenSpriteBlack.setScale(sf::Vector2f(0.5f, 0.5f));
	kingSpriteBlack.setScale(sf::Vector2f(0.5f, 0.5f));
}
