#include "AssetManager.h"

#ifndef RESOURCES_PATH
#define RESOURCES_PATH "./resources/" // fallback (shouldn’t happen if CMake defines it)
#endif

map<string, sf::Texture> AssetManager::_textures;

map<string, string> AssetManager::_fileNames = {
    // Chess pieces
    { "pawn_white",   string(RESOURCES_PATH) + "figurine_assets/pawn_white.png" },
    { "rook_white",   string(RESOURCES_PATH) + "figurine_assets/rook_white.png" },
    { "knight_white", string(RESOURCES_PATH) + "figurine_assets/knight_white.png" },
    { "bishop_white", string(RESOURCES_PATH) + "figurine_assets/bishop_white.png" },
    { "queen_white",  string(RESOURCES_PATH) + "figurine_assets/queen_white.png" },
    { "king_white",   string(RESOURCES_PATH) + "figurine_assets/king_white.png" },

    { "pawn_black",   string(RESOURCES_PATH) + "figurine_assets/pawn_black.png" },
    { "rook_black",   string(RESOURCES_PATH) + "figurine_assets/rook_black.png" },
    { "knight_black", string(RESOURCES_PATH) + "figurine_assets/knight_black.png" },
    { "bishop_black", string(RESOURCES_PATH) + "figurine_assets/bishop_black.png" },
    { "queen_black",  string(RESOURCES_PATH) + "figurine_assets/queen_black.png" },
    { "king_black",   string(RESOURCES_PATH) + "figurine_assets/king_black.png" },

    // Menu assets
    { "menu_host",         string(RESOURCES_PATH) + "menu_assets/MenuHostGame.png" },
    { "menu_join",         string(RESOURCES_PATH) + "menu_assets/MenuJoinGame.png" },
    { "menu_quit",         string(RESOURCES_PATH) + "menu_assets/MenuQuit.png" },
    { "menu_singleplayer", string(RESOURCES_PATH) + "menu_assets/MenuSingleplayer.png" }
};

void AssetManager::LoadTextures()
{

#if PRODUCTION_BUILD
    cout << "[Production Build] Loading assets from: " << RESOURCES_PATH << endl;
#else
    cout << "[Development Build] Loading assets from: " << RESOURCES_PATH << endl;
#endif

	for (auto& [key, path] : _fileNames)
	{
		sf::Texture tex;
		if (!tex.loadFromFile(path))
			cerr << "Failed to load " << path << endl;
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
