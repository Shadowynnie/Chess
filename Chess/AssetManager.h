#pragma once
#include <SFML/Graphics.hpp>

class AssetManager
{
public:
	AssetManager();

	sf::Texture pawnTexWhite;
	sf::Texture rookTexWhite;
	sf::Texture knightTexWhite;
	sf::Texture bishopTexWhite;
	sf::Texture queenTexWhite;
	sf::Texture kingTexWhite;

	sf::Texture pawnTexBlack;
	sf::Texture rookTexBlack;
	sf::Texture knightTexBlack;
	sf::Texture bishopTexBlack;
	sf::Texture queenTexBlack;
	sf::Texture kingTexBlack;

	sf::Sprite pawnSpriteWhite{ pawnTexWhite };	
	sf::Sprite rookSpriteWhite{ rookTexWhite };
	sf::Sprite knightSpriteWhite{ knightTexWhite };
	sf::Sprite bishopSpriteWhite{ bishopTexWhite };
	sf::Sprite queenSpriteWhite{ queenTexWhite };
	sf::Sprite kingSpriteWhite{ kingTexWhite };

	sf::Sprite pawnSpriteBlack{ pawnTexBlack };
	sf::Sprite rookSpriteBlack{ rookTexBlack };
	sf::Sprite knightSpriteBlack{ knightTexBlack };
	sf::Sprite bishopSpriteBlack{ bishopTexBlack };
	sf::Sprite queenSpriteBlack{ queenTexBlack };
	sf::Sprite kingSpriteBlack{ kingTexBlack };

	//sf::RectangleShape mouseShape{ sf::Vector2f(5, 5) };
};
