#include "AssetManager.h"

AssetManager::AssetManager()
{
	// Load textures from files
	map<string, string> fileNames = {
		{ "pawn_white",   "Assets/pawn_white.png" },
		{ "rook_white",   "Assets/rook_white.png" },
		{ "knight_white", "Assets/knight_white.png" },
		{ "bishop_white", "Assets/bishop_white.png" },
		{ "queen_white",  "Assets/queen_white.png" },
		{ "king_white",   "Assets/king_white.png" },

		{ "pawn_black",   "Assets/pawn_black.png" },
		{ "rook_black",   "Assets/rook_black.png" },
		{ "knight_black", "Assets/knight_black.png" },
		{ "bishop_black", "Assets/bishop_black.png" },
		{ "queen_black",  "Assets/queen_black.png" },
		{ "king_black",   "Assets/king_black.png" }
	};

	// Check if files exist and load them
	for (auto& [key, path] : fileNames)
	{
		sf::Texture tex;
		if (!tex.loadFromFile(path))
		{
			cerr << "Failed to load " << path << endl;
		}
		else
		{
			cout << "Loaded " << path << endl;
			tex.setSmooth(true);
			textures[key] = tex;
		}
	}
}

// Accessors
const sf::Texture& AssetManager::getTexture(const string& key) const
{
	return textures.at(key);
}

sf::Sprite AssetManager::getSprite(const string& key) const
{
	sf::Sprite sprite(textures.at(key));
	sprite.setScale(sf::Vector2f(0.5f, 0.5f));
	return sprite;
}
