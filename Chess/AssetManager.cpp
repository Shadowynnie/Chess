#include "AssetManager.h"
#include <iostream>
#include <filesystem>

using std::cout;
using std::cerr;
using std::endl;


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

	sf::Texture textures[] = {
		pawnTexWhite,
		rookTexWhite,
		knightTexWhite,
		bishopTexWhite,
		queenTexWhite,
		kingTexWhite,
		pawnTexBlack,
		rookTexBlack,
		knightTexBlack,
		bishopTexBlack,
		queenTexBlack,
		kingTexBlack
	};

	sf::Sprite sprites[] = {
		pawnSpriteWhite,
		rookSpriteWhite,
		knightSpriteWhite,
		bishopSpriteWhite,
		queenSpriteWhite,
		kingSpriteWhite,
		pawnSpriteBlack,
		rookSpriteBlack,
		knightSpriteBlack,
		bishopSpriteBlack,
		queenSpriteBlack,
		kingSpriteBlack
	};

	//size_t fileCount = sizeof(fileNames) / sizeof(fileNames[0]);
	// Loop through the file names and attempt to load each texture
	for (size_t i = 0; i < (sizeof(fileNames) / sizeof(fileNames[0])); i++)
	{
		cout << "Loading texture: " << fileNames[i] << endl;
		if (!textures[i].loadFromFile(fileNames[i]))
		{
			cerr << "Failed to load " << fileNames[i] << endl;
		}
		else
		{
			cout << "Successfully loaded " << fileNames[i] << endl;
			sprites[i] = sf::Sprite(textures[i]);
			sprites[i].setScale(sf::Vector2f(0.5f, 0.5f));
		}
	}

	/*
	// Load the textures
	cout << "Loading texture: " << fileNames[0] << endl;
	if (!pawnTexWhite.loadFromFile(fileNames[0])) 
	{
		cerr << "Failed to load " << fileNames[0] << endl;
	}
	else
	{
		cout << "Successfully loaded " << fileNames[0] << endl;
	}

	cout << "Loading texture: " << fileNames[1] << endl;
	if (!rookTexWhite.loadFromFile(fileNames[1])) 
	{
		cerr << "Failed to load " << fileNames[1] << endl;
	}
	else
	{
		cout << "Successfully loaded " << fileNames[1] << endl;
	}

	cout << "Loading texture: " << fileNames[2] << endl;
	if (!knightTexWhite.loadFromFile(fileNames[2])) 
	{
		cerr << "Failed to load " << fileNames[2] << endl;
	}
	else
	{
		cout << "Successfully loaded " << fileNames[2] << endl;
	}

	cout << "Loading texture: " << fileNames[3] << endl;
	if (!bishopTexWhite.loadFromFile(fileNames[3])) 
	{
		cerr << "Failed to load " << fileNames[3] << endl;
	}
	else
	{
		cout << "Successfully loaded " << fileNames[3] << endl;
	}

	cout << "Loading texture: " << fileNames[4] << endl;
	if (!queenTexWhite.loadFromFile(fileNames[4])) 
	{
		cerr << "Failed to load " << fileNames[4] << endl;
	}
	else
	{
		cout << "Successfully loaded " << fileNames[4] << endl;
	}

	cout << "Loading texture: " << fileNames[5] << endl;
	if (!kingTexWhite.loadFromFile(fileNames[5])) 
	{
		cerr << "Failed to load " << fileNames[5] << endl;
	}
	else
	{
		cout << "Successfully loaded " << fileNames[5] << endl;
	}

	cout << "Loading texture: " << fileNames[6] << endl;
	if (!pawnTexBlack.loadFromFile(fileNames[6])) 
	{
		cerr << "Failed to load " << fileNames[6] << endl;
	}
	else
	{
		cout << "Successfully loaded " << fileNames[6] << endl;
	}

	cout << "Loading texture: " << fileNames[7] << endl;
	if (!rookTexBlack.loadFromFile(fileNames[7])) 
	{
		cerr << "Failed to load " << fileNames[7] << endl;
	}
	else
	{
		cout << "Successfully loaded " << fileNames[7] << endl;
	}

	cout << "Loading texture: " << fileNames[8] << endl;
	if (!knightTexBlack.loadFromFile(fileNames[8])) 
	{
		cerr << "Failed to load " << fileNames[8] << endl;
	}
	else
	{
		cout << "Successfully loaded " << fileNames[8] << endl;
	}

	cout << "Loading texture: " << fileNames[9] << endl;
	if (!bishopTexBlack.loadFromFile(fileNames[9])) 
	{
		cerr << "Failed to load " << fileNames[9] << endl;
	}
	else
	{
		cout << "Successfully loaded " << fileNames[9] << endl;
	}

	cout << "Loading texture: " << fileNames[10] << endl;
	if (!queenTexBlack.loadFromFile(fileNames[10])) 
	{
		cerr << "Failed to load " << fileNames[10] << endl;
	}
	else
	{
		cout << "Successfully loaded " << fileNames[10] << endl;
	}

	cout << "Loading texture: " << fileNames[11] << endl;
	if (!kingTexBlack.loadFromFile(fileNames[11])) 
	{
		cerr << "Failed to load " << fileNames[11] << endl;
	}
	else
	{
		cout << "Successfully loaded " << fileNames[11] << endl;
	}*/

	// Create the sprites AFTER textures are loaded
	/*
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
	kingSpriteBlack = sf::Sprite(kingTexBlack);*/

	// Set the initial positions of the sprites
	/*
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
	kingSpriteBlack.setPosition(sf::Vector2f(600, 200));*/

	// Set the scale of the sprites (originally they were 128x128, now scaled down to 64x64)
	/*
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
	kingSpriteBlack.setScale(sf::Vector2f(0.5f, 0.5f));*/
}
