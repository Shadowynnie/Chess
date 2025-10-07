#include "AssetManager.h"


map<string, sf::Texture> AssetManager::_textures;

map<string, string> AssetManager::_fileNames = {
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

void AssetManager::LoadTextures() 
{
	for (auto& [key, path] : _fileNames)
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
			_textures[key] = tex;
		}
	}
}

// Accessors
sf::Texture& AssetManager::GetTexture(const string& key)
{
	return _textures.at(key);
}

sf::Sprite AssetManager::GetSprite(const string& key)
{
	sf::Sprite sprite(_textures.at(key));
	sprite.setScale(sf::Vector2f(0.5f, 0.5f));
	return sprite;
}
