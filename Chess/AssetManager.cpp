#include "AssetManager.h"


map<string, sf::Texture> AssetManager::_textures;

map<string, string> AssetManager::_fileNames = {
    // Load chess piece assets
	{ "pawn_white",   "Assets/Figurines/pawn_white.png" },
	{ "rook_white",   "Assets/Figurines/rook_white.png" },
	{ "knight_white", "Assets/Figurines/knight_white.png" },
	{ "bishop_white", "Assets/Figurines/bishop_white.png" },
	{ "queen_white",  "Assets/Figurines/queen_white.png" },
	{ "king_white",   "Assets/Figurines/king_white.png" },
	{ "pawn_black",   "Assets/Figurines/pawn_black.png" },
	{ "rook_black",   "Assets/Figurines/rook_black.png" },
	{ "knight_black", "Assets/Figurines/knight_black.png" },
	{ "bishop_black", "Assets/Figurines/bishop_black.png" },
	{ "queen_black",  "Assets/Figurines/queen_black.png" },
	{ "king_black",   "Assets/Figurines/king_black.png" },
	// Load Main Menu assets
	// TODO: Replace menu these items by drawable strings using SFML functions
	{ "menu_host", "Assets/MenuItems/MainMenu/menu_host_game.png" },
	{ "menu_join", "Assets/MenuItems/MainMenu/menu_join_game.png" },
    { "menu_quit", "Assets/MenuItems/MainMenu/menu_quit.png" },
	{ "menu_singleplayer", "Assets/MenuItems/MainMenu/menu_singleplayer.png" },
};

void AssetManager::LoadTextures() 
{
	for (auto& [key, path] : _fileNames)
	{
		sf::Texture tex;
		if (!tex.loadFromFile(path))
			cerr << "Failed to load " << path << "\n";
		else
		{
			cout << "Loaded " << path << "\n";
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
