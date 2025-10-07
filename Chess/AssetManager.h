#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include <iostream>
#include <filesystem>

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::map;

class AssetManager
{
public:
    // Accessors
    static sf::Texture& GetTexture(const string& key);
    static sf::Sprite GetSprite(const string& key);
    static void LoadTextures();

private:
	static map<string, string> _fileNames;
    static map<string, sf::Texture> _textures;
};
